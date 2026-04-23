#!/usr/bin/env python3
"""Manage immutable not-banana model images and channel promotion.

This script packages trainer outputs into immutable model images, verifies image
integrity, and promotes channel pointers (candidate/stable) for safe rollback.
"""

from __future__ import annotations

import argparse
import hashlib
import hmac
import json
import os
import re
import shutil
import socket
import sys
import tarfile
import urllib.error
import urllib.request
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

MODEL_FAMILY = "not-banana-vocabulary-v1"
DEFAULT_MODEL_DIR = Path("artifacts/not-banana-model")
DEFAULT_REGISTRY_DIR = Path("artifacts/not-banana-model-registry")
DEFAULT_REGISTRY_HISTORY_DIR = Path("data/not-banana/model-release-history")
DEFAULT_RESTORE_OUTPUT_ROOT = Path("artifacts")
REQUIRED_FILES = (
    "vocabulary.json",
    "metrics.json",
    "sessions.json",
    "banana_signal_tokens.h",
)
OPTIONAL_FILES = ("drift.json",)
DEFAULT_SIGNING_KEY_ENV = "BANANA_MODEL_SIGNING_KEY"
DEFAULT_SIGNING_KEY_ID_ENV = "BANANA_MODEL_SIGNING_KEY_ID"
DEFAULT_SNAPSHOT_DIR = Path("artifacts/not-banana-model-registry-snapshots")
DEFAULT_UPLOAD_TIMEOUT_SEC = 60


def utc_now() -> str:
    return datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")


def iso_compact_now() -> str:
    return datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(65536), b""):
            digest.update(chunk)
    return digest.hexdigest()


def load_json(path: Path) -> dict[str, Any]:
    return json.loads(path.read_text(encoding="utf-8"))


def write_json(path: Path, payload: dict[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, sort_keys=False) + "\n", encoding="utf-8")


def normalize_label(value: str) -> str:
    sanitized = re.sub(r"[^a-zA-Z0-9._-]+", "-", value.strip())
    sanitized = sanitized.strip("-._")
    return sanitized or iso_compact_now()


def resolve_cli_or_env(explicit_value: str | None, env_name: str | None) -> str | None:
    if explicit_value:
        return explicit_value

    if env_name:
        env_value = os.environ.get(env_name)
        if env_value:
            return env_value

    return None


def canonical_manifest_payload(manifest: dict[str, Any]) -> bytes:
    payload = dict(manifest)
    payload.pop("signature", None)
    return json.dumps(
        payload,
        sort_keys=True,
        separators=(",", ":"),
        ensure_ascii=True,
    ).encode("utf-8")


def sign_manifest(manifest: dict[str, Any], signing_key: str) -> str:
    return hmac.new(
        signing_key.encode("utf-8"),
        canonical_manifest_payload(manifest),
        hashlib.sha256,
    ).hexdigest()


def validate_manifest_signature(
    manifest: dict[str, Any],
    signing_key: str | None,
    require_signature: bool,
) -> list[str]:
    errors: list[str] = []
    signature = manifest.get("signature")

    if not isinstance(signature, dict):
        if require_signature:
            errors.append("Manifest is missing signature block.")
        return errors

    algorithm = str(signature.get("algorithm", "")).strip().lower()
    signature_value = str(signature.get("value", "")).strip()

    if algorithm != "hmac-sha256":
        if require_signature:
            errors.append(
                f"Manifest signature algorithm '{algorithm or '<empty>'}' is not allowed."
            )
        return errors

    if not signature_value:
        errors.append("Manifest signature value is empty.")
        return errors

    if not signing_key:
        errors.append("No signing key available to verify manifest signature.")
        return errors

    expected = sign_manifest(manifest, signing_key)
    if not hmac.compare_digest(signature_value, expected):
        errors.append("Manifest signature mismatch; image metadata may be tampered.")

    return errors


def parse_headers(header_values: list[str]) -> dict[str, str]:
    headers: dict[str, str] = {}

    for raw in header_values:
        if ":" not in raw:
            raise ValueError(
                f"Invalid upload header '{raw}'. Use the format 'Header-Name: value'."
            )
        name, value = raw.split(":", 1)
        header_name = name.strip()
        header_value = value.strip()
        if not header_name:
            raise ValueError(f"Invalid upload header '{raw}'; empty header name.")
        headers[header_name] = header_value

    return headers


def upload_file_put(
    url: str,
    file_path: Path,
    headers: dict[str, str],
    timeout_sec: int,
) -> None:
    payload = file_path.read_bytes()
    request = urllib.request.Request(url=url, data=payload, method="PUT")
    for key, value in headers.items():
        request.add_header(key, value)
    request.add_header("Content-Length", str(len(payload)))

    with urllib.request.urlopen(request, timeout=timeout_sec) as response:
        status = getattr(response, "status", 200)
        if status >= 300:
            raise ValueError(
                f"Upload to {url} failed with HTTP status {status}."
            )


def append_github_output(values: dict[str, str]) -> None:
    output_path = os.environ.get("GITHUB_OUTPUT", "").strip()
    if not output_path:
        return

    with open(output_path, "a", encoding="utf-8") as handle:
        for key, value in values.items():
            handle.write(f"{key}={value}\n")


def remove_path(path: Path) -> None:
    if path.is_dir():
        shutil.rmtree(path)
    elif path.exists():
        path.unlink()


def load_or_create_index(registry_dir: Path) -> dict[str, Any]:
    index_path = registry_dir / "index.json"
    if index_path.exists():
        return load_json(index_path)

    return {
        "schema_version": 1,
        "model_family": MODEL_FAMILY,
        "created_at_utc": utc_now(),
        "updated_at_utc": utc_now(),
        "images": [],
    }


def save_index(registry_dir: Path, index_payload: dict[str, Any]) -> None:
    index_payload["updated_at_utc"] = utc_now()
    write_json(registry_dir / "index.json", index_payload)


def read_channel_pointer(registry_dir: Path, channel: str) -> dict[str, Any] | None:
    path = registry_dir / "channels" / f"{channel}.json"
    if not path.exists():
        return None
    return load_json(path)


def write_channel_pointer(
    registry_dir: Path,
    channel: str,
    image_id: str,
    model_digest: str,
    holdout_metrics: dict[str, Any],
) -> None:
    pointer = {
        "channel": channel,
        "image_id": image_id,
        "model_digest": model_digest,
        "updated_at_utc": utc_now(),
        "metrics": {
            "precision": holdout_metrics.get("precision", 0.0),
            "recall": holdout_metrics.get("recall", 0.0),
            "f1": holdout_metrics.get("f1", 0.0),
            "accuracy": holdout_metrics.get("accuracy", 0.0),
        },
    }
    write_json(registry_dir / "channels" / f"{channel}.json", pointer)


def resolve_image_id(
    registry_dir: Path,
    image_id: str | None,
    channel: str | None,
) -> str:
    if image_id:
        return image_id

    if not channel:
        raise ValueError("Either --image-id or --channel/--from-channel is required.")

    pointer = read_channel_pointer(registry_dir, channel)
    if not pointer:
        raise ValueError(f"Channel '{channel}' is not set in the registry.")

    resolved = str(pointer.get("image_id", "")).strip()
    if not resolved:
        raise ValueError(f"Channel '{channel}' has no image_id pointer.")

    return resolved


def image_dir(registry_dir: Path, image_id: str) -> Path:
    return registry_dir / "images" / image_id


def manifest_path(registry_dir: Path, image_id: str) -> Path:
    return image_dir(registry_dir, image_id) / "manifest.json"


def load_manifest(registry_dir: Path, image_id: str) -> dict[str, Any]:
    path = manifest_path(registry_dir, image_id)
    if not path.exists():
        raise ValueError(f"Image '{image_id}' does not exist in registry.")
    return load_json(path)


def compute_model_digest(file_checksums: dict[str, str]) -> str:
    digest = hashlib.sha256()
    for file_name in REQUIRED_FILES:
        digest.update(file_name.encode("utf-8"))
        digest.update(b":")
        digest.update(file_checksums[file_name].encode("ascii"))
        digest.update(b"\n")
    return digest.hexdigest()


def image_entry_from_manifest(manifest: dict[str, Any]) -> dict[str, Any]:
    metrics = manifest.get("metrics") or {}
    channels = manifest.get("channels") or []
    training = manifest.get("training") or {}
    signature = manifest.get("signature") or {}

    return {
        "image_id": manifest.get("image_id"),
        "created_at_utc": manifest.get("created_at_utc"),
        "model_digest": manifest.get("model_digest"),
        "parent_image_id": manifest.get("parent_image_id"),
        "channels": channels,
        "training_profile": training.get("training_profile", "unknown"),
        "session_mode": training.get("session_mode", "unknown"),
        "session_count": training.get("session_count", 0),
        "signature": {
            "algorithm": signature.get("algorithm", "none"),
            "key_id": signature.get("key_id", ""),
        },
        "metrics": {
            "precision": metrics.get("precision", 0.0),
            "recall": metrics.get("recall", 0.0),
            "f1": metrics.get("f1", 0.0),
            "accuracy": metrics.get("accuracy", 0.0),
        },
    }


def upsert_index_entry(index_payload: dict[str, Any], entry: dict[str, Any]) -> None:
    images = index_payload.setdefault("images", [])
    image_id = entry.get("image_id")

    for idx, existing in enumerate(images):
        if existing.get("image_id") == image_id:
            images[idx] = entry
            return

    images.append(entry)


def create_image(args: argparse.Namespace) -> int:
    model_dir = args.model_dir
    registry_dir = args.registry_dir

    signing_key = resolve_cli_or_env(args.signing_key, args.signing_key_env)
    key_id = resolve_cli_or_env(args.key_id, args.key_id_env) or ""
    if args.require_signing_key and not signing_key:
        print(
            "::error::Manifest signing key is required but was not provided.",
            file=sys.stderr,
        )
        return 1

    missing = [name for name in REQUIRED_FILES if not (model_dir / name).exists()]
    if missing:
        print(
            f"::error::Missing required model artifacts in {model_dir}: {', '.join(missing)}",
            file=sys.stderr,
        )
        return 1

    vocabulary_payload = load_json(model_dir / "vocabulary.json")
    metrics_payload = load_json(model_dir / "metrics.json")

    holdout_metrics = metrics_payload.get("holdout") or vocabulary_payload.get("metrics") or {}
    drift_payload: dict[str, Any] | None = None
    drift_path = model_dir / "drift.json"
    if drift_path.exists():
        drift_payload = load_json(drift_path)

    file_hashes: dict[str, str] = {}
    files_to_copy = list(REQUIRED_FILES)
    for optional_name in OPTIONAL_FILES:
        if (model_dir / optional_name).exists():
            files_to_copy.append(optional_name)

    for file_name in files_to_copy:
        file_hashes[file_name] = sha256_file(model_dir / file_name)

    model_digest = compute_model_digest(file_hashes)
    image_id = args.image_id or f"nbm-{iso_compact_now()}-{model_digest[:12]}"

    target_dir = image_dir(registry_dir, image_id)
    if target_dir.exists() and not args.allow_existing_image_id:
        print(
            f"::error::Image id '{image_id}' already exists. Use --image-id with a new value.",
            file=sys.stderr,
        )
        return 1

    parent_image_id = args.parent_image_id
    if not parent_image_id and args.parent_from_channel:
        pointer = read_channel_pointer(registry_dir, args.parent_from_channel)
        if pointer:
            parent_image_id = pointer.get("image_id")

    target_dir.mkdir(parents=True, exist_ok=True)

    manifest_files: list[dict[str, Any]] = []
    for file_name in files_to_copy:
        src = model_dir / file_name
        dst = target_dir / file_name
        shutil.copy2(src, dst)
        manifest_files.append(
            {
                "path": file_name,
                "size_bytes": dst.stat().st_size,
                "sha256": file_hashes[file_name],
            }
        )

    training_payload = {
        "training_profile": vocabulary_payload.get("training_profile", "unknown"),
        "session_mode": vocabulary_payload.get("session_mode", "unknown"),
        "session_count": vocabulary_payload.get("session_count", 0),
        "selected_session_index": vocabulary_payload.get("selected_session_index", 0),
        "hyperparameters": vocabulary_payload.get("hyperparameters", {}),
        "requested_hyperparameters": vocabulary_payload.get("requested_hyperparameters", {}),
        "resource_context": vocabulary_payload.get("resource_context", {}),
    }

    drift_summary = {
        "added_count": 0,
        "removed_count": 0,
    }
    if drift_payload:
        drift_summary = {
            "added_count": len(drift_payload.get("added", [])),
            "removed_count": len(drift_payload.get("removed", [])),
        }

    manifest = {
        "schema_version": 1,
        "model_family": MODEL_FAMILY,
        "image_id": image_id,
        "created_at_utc": utc_now(),
        "model_digest": model_digest,
        "parent_image_id": parent_image_id,
        "channels": [args.channel],
        "source": {
            "host": socket.gethostname(),
            "cwd": str(Path.cwd()),
            "git_sha": os.environ.get("GITHUB_SHA", ""),
            "github_run_id": os.environ.get("GITHUB_RUN_ID", ""),
            "github_run_attempt": os.environ.get("GITHUB_RUN_ATTEMPT", ""),
            "github_workflow": os.environ.get("GITHUB_WORKFLOW", ""),
        },
        "training": training_payload,
        "metrics": {
            "precision": holdout_metrics.get("precision", 0.0),
            "recall": holdout_metrics.get("recall", 0.0),
            "f1": holdout_metrics.get("f1", 0.0),
            "accuracy": holdout_metrics.get("accuracy", 0.0),
        },
        "drift_summary": drift_summary,
        "files": manifest_files,
        "safety": {
            "lifecycle_state": "candidate",
            "notes": "Promote to stable only after validation and policy checks.",
        },
    }

    if signing_key:
        manifest["signature"] = {
            "algorithm": "hmac-sha256",
            "key_id": key_id,
            "signed_at_utc": utc_now(),
            "value": sign_manifest(manifest, signing_key),
        }
    else:
        manifest["signature"] = {
            "algorithm": "none",
            "key_id": key_id,
            "signed_at_utc": utc_now(),
            "value": "",
        }

    write_json(target_dir / "manifest.json", manifest)

    index_payload = load_or_create_index(registry_dir)
    upsert_index_entry(index_payload, image_entry_from_manifest(manifest))
    save_index(registry_dir, index_payload)

    write_channel_pointer(
        registry_dir=registry_dir,
        channel=args.channel,
        image_id=image_id,
        model_digest=model_digest,
        holdout_metrics=manifest["metrics"],
    )

    append_github_output(
        {
            "model_image_id": image_id,
            "model_digest": model_digest,
            "model_channel": args.channel,
        }
    )

    print(
        f"created image_id={image_id} channel={args.channel} model_digest={model_digest}"
    )
    return 0


def verify_image_integrity(
    registry_dir: Path,
    image_id: str,
    signing_key: str | None,
    require_signature: bool,
) -> tuple[bool, list[str]]:
    errors: list[str] = []
    manifest = load_manifest(registry_dir, image_id)
    manifest_files = manifest.get("files")
    if not isinstance(manifest_files, list) or not manifest_files:
        return False, ["Manifest has no files list."]

    computed_hashes: dict[str, str] = {}
    for entry in manifest_files:
        file_name = str(entry.get("path", "")).strip()
        expected = str(entry.get("sha256", "")).strip()
        if not file_name or not expected:
            errors.append("Manifest file entry missing path or sha256.")
            continue

        file_path = image_dir(registry_dir, image_id) / file_name
        if not file_path.exists():
            errors.append(f"Missing file '{file_name}' in image '{image_id}'.")
            continue

        actual = sha256_file(file_path)
        computed_hashes[file_name] = actual
        if actual != expected:
            errors.append(
                f"Checksum mismatch for '{file_name}' (expected={expected}, actual={actual})."
            )

    for required_name in REQUIRED_FILES:
        if required_name not in computed_hashes:
            file_path = image_dir(registry_dir, image_id) / required_name
            if file_path.exists():
                computed_hashes[required_name] = sha256_file(file_path)

    if all(name in computed_hashes for name in REQUIRED_FILES):
        digest = compute_model_digest(computed_hashes)
        expected_digest = str(manifest.get("model_digest", "")).strip()
        if digest != expected_digest:
            errors.append(
                f"Model digest mismatch (expected={expected_digest}, actual={digest})."
            )

    errors.extend(
        validate_manifest_signature(
            manifest=manifest,
            signing_key=signing_key,
            require_signature=require_signature,
        )
    )

    return len(errors) == 0, errors


def verify_command(args: argparse.Namespace) -> int:
    image_id = resolve_image_id(args.registry_dir, args.image_id, args.channel)
    signing_key = resolve_cli_or_env(args.signing_key, args.signing_key_env)
    ok, errors = verify_image_integrity(
        args.registry_dir,
        image_id,
        signing_key=signing_key,
        require_signature=args.require_signature,
    )

    if not ok:
        for error in errors:
            print(f"::error::{error}", file=sys.stderr)
        return 1

    append_github_output({"verified_image_id": image_id})
    print(f"verified image_id={image_id} integrity=ok")
    return 0


def promote_image(args: argparse.Namespace) -> int:
    image_id = resolve_image_id(args.registry_dir, args.image_id, args.from_channel)
    signing_key = resolve_cli_or_env(args.signing_key, args.signing_key_env)

    ok, errors = verify_image_integrity(
        args.registry_dir,
        image_id,
        signing_key=signing_key,
        require_signature=not args.allow_unsigned,
    )
    if not ok:
        for error in errors:
            print(f"::error::{error}", file=sys.stderr)
        return 1

    manifest = load_manifest(args.registry_dir, image_id)
    metrics = manifest.get("metrics") or {}
    f1_score = float(metrics.get("f1", 0.0))

    if args.to_channel == "stable" and f1_score < args.min_f1:
        print(
            f"::error::Refusing stable promote: f1={f1_score} below min_f1={args.min_f1}",
            file=sys.stderr,
        )
        return 1

    drift_summary = manifest.get("drift_summary") or {}
    removed_count = int(drift_summary.get("removed_count", 0))
    if (
        args.to_channel == "stable"
        and removed_count > args.max_removed_tokens
        and not args.allow_large_removal
    ):
        print(
            "::error::Refusing stable promote: removed token count "
            f"{removed_count} exceeds max_removed_tokens={args.max_removed_tokens}",
            file=sys.stderr,
        )
        return 1

    channels = list(manifest.get("channels") or [])
    if args.to_channel not in channels:
        channels.append(args.to_channel)
    manifest["channels"] = channels

    safety = dict(manifest.get("safety") or {})
    if args.to_channel == "stable":
        safety["lifecycle_state"] = "stable"
    else:
        safety["lifecycle_state"] = args.to_channel
    manifest["safety"] = safety

    write_json(manifest_path(args.registry_dir, image_id), manifest)

    write_channel_pointer(
        registry_dir=args.registry_dir,
        channel=args.to_channel,
        image_id=image_id,
        model_digest=str(manifest.get("model_digest", "")),
        holdout_metrics=metrics,
    )

    index_payload = load_or_create_index(args.registry_dir)
    upsert_index_entry(index_payload, image_entry_from_manifest(manifest))
    save_index(args.registry_dir, index_payload)

    append_github_output(
        {
            "promoted_image_id": image_id,
            "promoted_channel": args.to_channel,
        }
    )

    print(
        f"promoted image_id={image_id} to_channel={args.to_channel} "
        f"f1={f1_score} removed_tokens={removed_count}"
    )
    return 0


def status_command(args: argparse.Namespace) -> int:
    registry_dir = args.registry_dir
    index_payload = load_or_create_index(registry_dir)

    channels_dir = registry_dir / "channels"
    channels: dict[str, Any] = {}
    if channels_dir.exists():
        for path in sorted(channels_dir.glob("*.json")):
            channels[path.stem] = load_json(path)

    images = list(index_payload.get("images") or [])
    images_sorted = sorted(
        images,
        key=lambda item: str(item.get("created_at_utc", "")),
        reverse=True,
    )

    payload = {
        "registry_dir": str(registry_dir),
        "image_count": len(images_sorted),
        "updated_at_utc": index_payload.get("updated_at_utc", ""),
        "channels": channels,
        "recent_images": images_sorted[:10],
    }

    if args.json:
        print(json.dumps(payload, indent=2, sort_keys=False))
        return 0

    print(f"registry={registry_dir}")
    print(f"image_count={payload['image_count']}")

    if not channels:
        print("channels=<none>")
    else:
        for channel_name, pointer in channels.items():
            print(
                f"channel={channel_name} image_id={pointer.get('image_id')} "
                f"f1={pointer.get('metrics', {}).get('f1', 0.0)}"
            )

    for entry in payload["recent_images"]:
        print(
            "image="
            f"{entry.get('image_id')} "
            f"channels={','.join(entry.get('channels') or [])} "
            f"f1={entry.get('metrics', {}).get('f1', 0.0)} "
            f"signature={entry.get('signature', {}).get('algorithm', 'none')}"
        )

    return 0


def snapshot_command(args: argparse.Namespace) -> int:
    registry_dir = args.registry_dir
    if not registry_dir.exists() or not registry_dir.is_dir():
        print(
            f"::error::Registry directory '{registry_dir}' does not exist.",
            file=sys.stderr,
        )
        return 1

    default_label = (
        f"run-{os.environ.get('GITHUB_RUN_ID', 'local')}-"
        f"attempt-{os.environ.get('GITHUB_RUN_ATTEMPT', '1')}-"
        f"{iso_compact_now()}"
    )
    snapshot_label = normalize_label(args.label or default_label)
    archive_name = f"not-banana-model-registry-{snapshot_label}.tar.gz"

    snapshot_dir = args.snapshot_dir
    snapshot_dir.mkdir(parents=True, exist_ok=True)
    archive_path = snapshot_dir / archive_name
    with tarfile.open(archive_path, "w:gz") as archive:
        archive.add(registry_dir, arcname=registry_dir.name)

    archive_sha256 = sha256_file(archive_path)
    sha_path = snapshot_dir / f"{archive_name}.sha256"
    sha_path.write_text(
        f"{archive_sha256}  {archive_name}\n",
        encoding="utf-8",
    )

    metadata = {
        "schema_version": 1,
        "snapshot_label": snapshot_label,
        "created_at_utc": utc_now(),
        "registry_dir": str(registry_dir),
        "archive": {
            "path": str(archive_path),
            "file_name": archive_name,
            "size_bytes": archive_path.stat().st_size,
            "sha256": archive_sha256,
        },
        "sha_file": {
            "path": str(sha_path),
            "file_name": sha_path.name,
        },
    }

    try:
        headers = parse_headers(args.upload_header)
    except ValueError as exc:
        print(f"::error::{exc}", file=sys.stderr)
        return 1

    upload_url = args.upload_url
    upload_sha_url = args.upload_sha_url
    if upload_url:
        try:
            upload_file_put(
                url=upload_url,
                file_path=archive_path,
                headers=headers,
                timeout_sec=args.timeout_sec,
            )
        except (urllib.error.URLError, ValueError) as exc:
            print(f"::error::Failed to upload snapshot archive: {exc}", file=sys.stderr)
            return 1
        metadata["archive"]["upload_url"] = upload_url

    if upload_sha_url:
        try:
            upload_file_put(
                url=upload_sha_url,
                file_path=sha_path,
                headers=headers,
                timeout_sec=args.timeout_sec,
            )
        except (urllib.error.URLError, ValueError) as exc:
            print(f"::error::Failed to upload snapshot sha file: {exc}", file=sys.stderr)
            return 1
        metadata["sha_file"]["upload_url"] = upload_sha_url

    metadata_path = snapshot_dir / f"{archive_name}.json"
    write_json(metadata_path, metadata)

    append_github_output(
        {
            "registry_snapshot_archive": str(archive_path),
            "registry_snapshot_sha256": archive_sha256,
            "registry_snapshot_sha_file": str(sha_path),
            "registry_snapshot_metadata": str(metadata_path),
        }
    )

    print(
        "snapshot created "
        f"archive={archive_path} sha256={archive_sha256} metadata={metadata_path}"
    )
    return 0


def resolve_history_snapshot_id(history_dir: Path, requested_snapshot: str | None) -> str:
    snapshot = str(requested_snapshot or "").strip()
    if snapshot and snapshot.lower() != "latest":
        return snapshot

    latest_path = history_dir / "latest.json"
    if not latest_path.exists():
        raise ValueError(
            f"History metadata not found at '{latest_path}'. "
            "Use --snapshot with a timestamp folder name."
        )

    latest_payload = load_json(latest_path)
    snapshots = latest_payload.get("snapshots") or []
    if isinstance(snapshots, list):
        for item in snapshots:
            snapshot_id = str(item).strip()
            if snapshot_id:
                return snapshot_id

    updated_at_utc = str(latest_payload.get("updated_at_utc", "")).strip()
    if updated_at_utc:
        return updated_at_utc

    raise ValueError(
        f"History metadata '{latest_path}' does not contain a usable snapshot id."
    )


def restore_history_command(args: argparse.Namespace) -> int:
    history_dir = args.history_path
    if not history_dir.exists() or not history_dir.is_dir():
        print(
            f"::error::History directory '{history_dir}' does not exist.",
            file=sys.stderr,
        )
        return 1

    try:
        snapshot_id = resolve_history_snapshot_id(history_dir, args.snapshot)
    except ValueError as exc:
        print(f"::error::{exc}", file=sys.stderr)
        return 1

    snapshot_dir = history_dir / snapshot_id
    if not snapshot_dir.exists() or not snapshot_dir.is_dir():
        print(
            f"::error::Snapshot '{snapshot_id}' does not exist under '{history_dir}'.",
            file=sys.stderr,
        )
        return 1

    release_bundles: dict[str, Path] = {}
    for entry in sorted(snapshot_dir.iterdir()):
        if not entry.is_dir():
            continue
        if not entry.name.startswith("not-banana-model-"):
            continue
        release_id = entry.name[len("not-banana-model-") :]
        if release_id:
            release_bundles[release_id] = entry

    if not release_bundles:
        print(
            f"::error::No not-banana model release bundles were found in '{snapshot_dir}'.",
            file=sys.stderr,
        )
        return 1

    requested_release_ids = list(args.release_id or [])
    selected_releases: dict[str, Path]
    if requested_release_ids:
        selected_releases = {}
        missing_release_ids: list[str] = []
        for release_id in requested_release_ids:
            bundle_dir = release_bundles.get(release_id)
            if not bundle_dir:
                missing_release_ids.append(release_id)
                continue
            selected_releases[release_id] = bundle_dir

        if missing_release_ids:
            available = ", ".join(sorted(release_bundles.keys()))
            missing = ", ".join(sorted(set(missing_release_ids)))
            print(
                "::error::Requested release ids were not found in snapshot "
                f"'{snapshot_id}': {missing}. Available: {available}",
                file=sys.stderr,
            )
            return 1
    else:
        selected_releases = dict(sorted(release_bundles.items()))

    output_root = args.output_root
    output_root.mkdir(parents=True, exist_ok=True)

    restored_releases: list[dict[str, Any]] = []
    for release_id, bundle_dir in selected_releases.items():
        release_record: dict[str, Any] = {
            "release_id": release_id,
            "bundle_dir": str(bundle_dir),
            "restored": {},
        }

        source_target_pairs = (
            (
                bundle_dir / "not-banana-model" / release_id,
                output_root / "not-banana-model" / release_id,
                "not-banana-model",
            ),
            (
                bundle_dir / "not-banana-model-registry" / release_id,
                output_root / "not-banana-model-registry" / release_id,
                "not-banana-model-registry",
            ),
            (
                bundle_dir / "not-banana-model-registry-snapshots" / release_id,
                output_root / "not-banana-model-registry-snapshots" / release_id,
                "not-banana-model-registry-snapshots",
            ),
        )

        restored_any = False
        for source_dir, target_dir, category in source_target_pairs:
            if not source_dir.exists() or not source_dir.is_dir():
                continue

            if target_dir.exists():
                if not args.overwrite:
                    print(
                        "::error::Target path already exists and overwrite is disabled: "
                        f"'{target_dir}'",
                        file=sys.stderr,
                    )
                    return 1
                remove_path(target_dir)

            target_dir.parent.mkdir(parents=True, exist_ok=True)
            shutil.copytree(source_dir, target_dir)
            release_record["restored"][category] = str(target_dir)
            restored_any = True

        if not restored_any:
            print(
                "::warning::Release bundle has no recognized restore directories: "
                f"{bundle_dir}",
                file=sys.stderr,
            )
            continue

        restored_releases.append(release_record)

    if not restored_releases:
        print(
            "::error::No release artifacts were restored from the selected snapshot.",
            file=sys.stderr,
        )
        return 1

    restore_report = {
        "schema_version": 1,
        "history_dir": str(history_dir),
        "snapshot_id": snapshot_id,
        "restored_at_utc": utc_now(),
        "output_root": str(output_root),
        "release_count": len(restored_releases),
        "releases": restored_releases,
    }

    report_path = output_root / "not-banana-model-restore-report.json"
    write_json(report_path, restore_report)

    append_github_output(
        {
            "registry_history_snapshot": snapshot_id,
            "registry_restore_output_root": str(output_root),
            "registry_restore_release_ids": ",".join(
                [item["release_id"] for item in restored_releases]
            ),
            "registry_restore_report": str(report_path),
        }
    )

    release_ids = ",".join([item["release_id"] for item in restored_releases])
    print(
        "history restored "
        f"snapshot={snapshot_id} releases={release_ids} report={report_path}"
    )
    return 0


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    create_parser = subparsers.add_parser(
        "create", help="Create an immutable model image from trainer artifacts."
    )
    create_parser.add_argument("--model-dir", type=Path, default=DEFAULT_MODEL_DIR)
    create_parser.add_argument("--registry-dir", type=Path, default=DEFAULT_REGISTRY_DIR)
    create_parser.add_argument("--channel", default="candidate")
    create_parser.add_argument("--image-id", default="")
    create_parser.add_argument("--parent-image-id", default="")
    create_parser.add_argument("--parent-from-channel", default="stable")
    create_parser.add_argument("--signing-key", default="")
    create_parser.add_argument(
        "--signing-key-env",
        default=DEFAULT_SIGNING_KEY_ENV,
        help="Environment variable name containing the manifest signing key.",
    )
    create_parser.add_argument("--key-id", default="")
    create_parser.add_argument(
        "--key-id-env",
        default=DEFAULT_SIGNING_KEY_ID_ENV,
        help="Environment variable name containing the signing key identifier.",
    )
    create_parser.add_argument(
        "--require-signing-key",
        action="store_true",
        help="Fail when signing key is missing.",
    )
    create_parser.add_argument(
        "--allow-existing-image-id",
        action="store_true",
        help="Allow writing into an existing image directory.",
    )
    create_parser.set_defaults(handler=create_image)

    verify_parser = subparsers.add_parser(
        "verify", help="Verify image checksums and digest."
    )
    verify_parser.add_argument("--registry-dir", type=Path, default=DEFAULT_REGISTRY_DIR)
    verify_parser.add_argument("--image-id", default="")
    verify_parser.add_argument("--channel", default="")
    verify_parser.add_argument("--signing-key", default="")
    verify_parser.add_argument(
        "--signing-key-env",
        default=DEFAULT_SIGNING_KEY_ENV,
        help="Environment variable name containing the manifest signing key.",
    )
    verify_parser.add_argument(
        "--require-signature",
        action="store_true",
        help="Require signed manifests and validate signatures.",
    )
    verify_parser.set_defaults(handler=verify_command)

    promote_parser = subparsers.add_parser(
        "promote", help="Promote an image to a channel (for example stable)."
    )
    promote_parser.add_argument("--registry-dir", type=Path, default=DEFAULT_REGISTRY_DIR)
    promote_parser.add_argument("--image-id", default="")
    promote_parser.add_argument("--from-channel", default="")
    promote_parser.add_argument("--to-channel", required=True)
    promote_parser.add_argument("--min-f1", type=float, default=0.7)
    promote_parser.add_argument("--max-removed-tokens", type=int, default=4)
    promote_parser.add_argument("--allow-large-removal", action="store_true")
    promote_parser.add_argument("--signing-key", default="")
    promote_parser.add_argument(
        "--signing-key-env",
        default=DEFAULT_SIGNING_KEY_ENV,
        help="Environment variable name containing the manifest signing key.",
    )
    promote_parser.add_argument(
        "--allow-unsigned",
        action="store_true",
        help="Allow promotion of unsigned manifests.",
    )
    promote_parser.set_defaults(handler=promote_image)

    status_parser = subparsers.add_parser(
        "status", help="Print registry status and channel pointers."
    )
    status_parser.add_argument("--registry-dir", type=Path, default=DEFAULT_REGISTRY_DIR)
    status_parser.add_argument("--json", action="store_true")
    status_parser.set_defaults(handler=status_command)

    snapshot_parser = subparsers.add_parser(
        "snapshot",
        help="Create and optionally upload a long-term registry snapshot archive.",
    )
    snapshot_parser.add_argument("--registry-dir", type=Path, default=DEFAULT_REGISTRY_DIR)
    snapshot_parser.add_argument("--snapshot-dir", type=Path, default=DEFAULT_SNAPSHOT_DIR)
    snapshot_parser.add_argument("--label", default="")
    snapshot_parser.add_argument("--upload-url", default="")
    snapshot_parser.add_argument("--upload-sha-url", default="")
    snapshot_parser.add_argument(
        "--upload-header",
        action="append",
        default=[],
        help="Extra upload header in 'Header-Name: value' form. Repeatable.",
    )
    snapshot_parser.add_argument(
        "--timeout-sec",
        type=int,
        default=DEFAULT_UPLOAD_TIMEOUT_SEC,
    )
    snapshot_parser.set_defaults(handler=snapshot_command)

    restore_parser = subparsers.add_parser(
        "restore-history",
        help="Restore source-controlled registry-history bundles into local artifacts.",
    )
    restore_parser.add_argument(
        "--history-path",
        type=Path,
        default=DEFAULT_REGISTRY_HISTORY_DIR,
    )
    restore_parser.add_argument(
        "--snapshot",
        default="latest",
        help="Snapshot folder id to restore, or 'latest' to resolve via latest.json.",
    )
    restore_parser.add_argument(
        "--output-root",
        type=Path,
        default=DEFAULT_RESTORE_OUTPUT_ROOT,
    )
    restore_parser.add_argument(
        "--release-id",
        action="append",
        default=[],
        help="Release id to restore. Repeat to restore multiple. Defaults to all releases in snapshot.",
    )
    restore_parser.add_argument(
        "--overwrite",
        dest="overwrite",
        action="store_true",
        help="Overwrite existing target directories (default).",
    )
    restore_parser.add_argument(
        "--no-overwrite",
        dest="overwrite",
        action="store_false",
        help="Fail if any target directory already exists.",
    )
    restore_parser.set_defaults(handler=restore_history_command, overwrite=True)

    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)

    if hasattr(args, "timeout_sec") and int(args.timeout_sec) <= 0:
        raise ValueError("timeout-sec must be greater than zero.")

    if hasattr(args, "image_id") and isinstance(args.image_id, str):
        args.image_id = args.image_id.strip() or None
    if hasattr(args, "channel") and isinstance(args.channel, str):
        args.channel = args.channel.strip() or None
    if hasattr(args, "from_channel") and isinstance(args.from_channel, str):
        args.from_channel = args.from_channel.strip() or None
    if hasattr(args, "parent_image_id") and isinstance(args.parent_image_id, str):
        args.parent_image_id = args.parent_image_id.strip() or None
    if hasattr(args, "parent_from_channel") and isinstance(args.parent_from_channel, str):
        args.parent_from_channel = args.parent_from_channel.strip() or None
    if hasattr(args, "to_channel") and isinstance(args.to_channel, str):
        args.to_channel = args.to_channel.strip() or None
    if hasattr(args, "signing_key") and isinstance(args.signing_key, str):
        args.signing_key = args.signing_key.strip() or None
    if hasattr(args, "signing_key_env") and isinstance(args.signing_key_env, str):
        args.signing_key_env = args.signing_key_env.strip() or None
    if hasattr(args, "key_id") and isinstance(args.key_id, str):
        args.key_id = args.key_id.strip() or None
    if hasattr(args, "key_id_env") and isinstance(args.key_id_env, str):
        args.key_id_env = args.key_id_env.strip() or None
    if hasattr(args, "label") and isinstance(args.label, str):
        args.label = args.label.strip() or None
    if hasattr(args, "upload_url") and isinstance(args.upload_url, str):
        args.upload_url = args.upload_url.strip() or None
    if hasattr(args, "upload_sha_url") and isinstance(args.upload_sha_url, str):
        args.upload_sha_url = args.upload_sha_url.strip() or None
    if hasattr(args, "snapshot") and isinstance(args.snapshot, str):
        args.snapshot = args.snapshot.strip() or None
    if hasattr(args, "release_id") and isinstance(args.release_id, list):
        args.release_id = [
            item.strip()
            for item in args.release_id
            if isinstance(item, str) and item.strip()
        ]

    return int(args.handler(args))


if __name__ == "__main__":
    raise SystemExit(main())

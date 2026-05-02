#!/usr/bin/env python3
"""Manage immutable not-banana model images and channel pointers."""

from __future__ import annotations

import argparse
import hashlib
import hmac
import json
import shutil
import tarfile
import urllib.request
from datetime import datetime, timezone
from pathlib import Path


def utc_now() -> str:
    return (
        datetime.now(timezone.utc)
        .replace(microsecond=0)
        .isoformat()
        .replace("+00:00", "Z")
    )


def read_json(path: Path, default: dict | list | None = None):
    if not path.exists():
        if default is None:
            raise FileNotFoundError(path)
        return default
    return json.loads(path.read_text(encoding="utf-8"))


def write_json(path: Path, payload: dict | list) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")


def file_sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(65536), b""):
            digest.update(chunk)
    return digest.hexdigest()


def ensure_registry(registry_dir: Path) -> None:
    (registry_dir / "images").mkdir(parents=True, exist_ok=True)
    (registry_dir / "channels").mkdir(parents=True, exist_ok=True)


def channel_file(registry_dir: Path, channel: str) -> Path:
    return registry_dir / "channels" / f"{channel}.json"


def read_channel_image_id(registry_dir: Path, channel: str) -> str:
    payload = read_json(channel_file(registry_dir, channel))
    image_id = str(payload.get("image_id", "")).strip()
    if not image_id:
        raise ValueError(f"Channel '{channel}' does not contain an image_id.")
    return image_id


def resolve_image_id(
    registry_dir: Path, channel: str | None, image_id: str | None
) -> str:
    if image_id:
        return image_id
    if channel:
        return read_channel_image_id(registry_dir, channel)
    raise ValueError("Either --image-id or --channel/--from-channel is required.")


def manifest_payload_for_signing(payload: dict) -> bytes:
    clone = dict(payload)
    clone.pop("signature", None)
    return json.dumps(clone, sort_keys=True, separators=(",", ":")).encode("utf-8")


def sign_manifest(payload: dict, signing_key: str, key_id: str) -> dict:
    signature = hmac.new(
        signing_key.encode("utf-8"),
        manifest_payload_for_signing(payload),
        hashlib.sha256,
    ).hexdigest()
    payload["signature"] = {
        "algorithm": "hmac-sha256",
        "key_id": key_id,
        "value": signature,
    }
    return payload


def verify_signature(payload: dict, signing_key: str) -> bool:
    signature = payload.get("signature")
    if not isinstance(signature, dict):
        return False
    expected = hmac.new(
        signing_key.encode("utf-8"),
        manifest_payload_for_signing(payload),
        hashlib.sha256,
    ).hexdigest()
    return hmac.compare_digest(str(signature.get("value", "")), expected)


def update_index(
    registry_dir: Path, image_id: str, channel: str | None, metrics: dict
) -> None:
    index_path = registry_dir / "index.json"
    index = read_json(
        index_path,
        {
            "schema_version": 1,
            "updated_at_utc": utc_now(),
            "images": {},
            "channels": {},
        },
    )
    if not isinstance(index.get("images"), dict):
        index["images"] = {}
    if not isinstance(index.get("channels"), dict):
        index["channels"] = {}

    index["images"][image_id] = {
        "created_at_utc": utc_now(),
        "path": f"images/{image_id}",
        "holdout_f1": float(metrics.get("holdout_f1", 0.0)),
        "signal_score": float(metrics.get("signal_score", 0.0)),
    }
    if channel:
        index["channels"][channel] = image_id

    index["updated_at_utc"] = utc_now()
    write_json(index_path, index)


def load_metrics(model_dir: Path) -> dict:
    metrics_path = model_dir / "metrics.json"
    payload = read_json(metrics_path)
    metrics = payload.get("metrics") if isinstance(payload, dict) else None
    if not isinstance(metrics, dict):
        raise ValueError(f"Expected metrics object in {metrics_path}")
    return metrics


def create_image(args: argparse.Namespace) -> int:
    model_dir = Path(args.model_dir)
    registry_dir = Path(args.registry_dir)
    ensure_registry(registry_dir)

    required_files = [
        "metrics.json",
        "vocabulary.json",
        "sessions.json",
        "banana_signal_tokens.h",
    ]
    missing = [name for name in required_files if not (model_dir / name).exists()]
    if missing:
        raise FileNotFoundError(f"Missing required model files: {', '.join(missing)}")

    metrics = load_metrics(model_dir)
    vocabulary_digest = file_sha256(model_dir / "vocabulary.json")
    seed = f"{utc_now()}:{vocabulary_digest}:{model_dir}"
    image_id = f"{datetime.now(timezone.utc):%Y%m%dT%H%M%SZ}-{hashlib.sha256(seed.encode('utf-8')).hexdigest()[:12]}"

    image_dir = registry_dir / "images" / image_id
    if image_dir.exists():
        raise FileExistsError(f"Image directory already exists: {image_dir}")

    shutil.copytree(model_dir, image_dir / "model")

    model_files: dict[str, str] = {}
    for file_path in sorted((image_dir / "model").rglob("*")):
        if file_path.is_file():
            rel = file_path.relative_to(image_dir / "model").as_posix()
            model_files[rel] = file_sha256(file_path)

    parent_image_id = None
    if args.parent_from_channel:
        try:
            parent_image_id = read_channel_image_id(
                registry_dir, args.parent_from_channel
            )
        except FileNotFoundError:
            parent_image_id = None

    manifest = {
        "schema_version": 1,
        "image_id": image_id,
        "created_at_utc": utc_now(),
        "channel": args.channel,
        "parent_image_id": parent_image_id,
        "metrics": metrics,
        "files": model_files,
    }

    signing_key = None
    if args.signing_key_env:
        signing_key = __import__("os").environ.get(args.signing_key_env, "")

    if args.require_signing_key and not signing_key:
        raise ValueError(
            f"Required signing key env var '{args.signing_key_env}' is empty or not set."
        )

    if signing_key:
        key_id = (
            __import__("os").environ.get(args.key_id_env, "") if args.key_id_env else ""
        )
        manifest = sign_manifest(manifest, signing_key, key_id)

    write_json(image_dir / "manifest.json", manifest)

    channel_payload = {
        "schema_version": 1,
        "channel": args.channel,
        "image_id": image_id,
        "updated_at_utc": utc_now(),
    }
    write_json(channel_file(registry_dir, args.channel), channel_payload)
    update_index(registry_dir, image_id, args.channel, metrics)

    print(
        json.dumps(
            {"created": True, "image_id": image_id, "channel": args.channel}, indent=2
        )
    )
    return 0


def verify_image(args: argparse.Namespace) -> int:
    registry_dir = Path(args.registry_dir)
    image_id = resolve_image_id(registry_dir, args.channel, args.image_id)

    image_dir = registry_dir / "images" / image_id
    manifest_path = image_dir / "manifest.json"
    manifest = read_json(manifest_path)
    files = manifest.get("files", {})
    if not isinstance(files, dict):
        raise ValueError("Manifest files field is invalid.")

    mismatches: list[str] = []
    missing: list[str] = []
    for rel_path, expected_hash in files.items():
        candidate = image_dir / "model" / rel_path
        if not candidate.exists():
            missing.append(rel_path)
            continue
        actual_hash = file_sha256(candidate)
        if actual_hash != expected_hash:
            mismatches.append(rel_path)

    signature_present = isinstance(manifest.get("signature"), dict)
    signing_key = (
        __import__("os").environ.get(args.signing_key_env, "")
        if args.signing_key_env
        else ""
    )
    signature_valid = False
    if signature_present and signing_key:
        signature_valid = verify_signature(manifest, signing_key)

    if args.require_signature and not signature_present:
        signature_valid = False

    valid = not missing and not mismatches
    if args.require_signature:
        valid = valid and signature_present and signature_valid

    report = {
        "valid": valid,
        "image_id": image_id,
        "missing_files": missing,
        "mismatched_files": mismatches,
        "signature_present": signature_present,
        "signature_valid": signature_valid if signature_present else None,
    }
    print(json.dumps(report, indent=2))
    return 0 if valid else 1


def promote_image(args: argparse.Namespace) -> int:
    registry_dir = Path(args.registry_dir)
    image_id = resolve_image_id(registry_dir, args.from_channel, args.image_id)
    manifest = read_json(registry_dir / "images" / image_id / "manifest.json")

    metrics = manifest.get("metrics", {})
    holdout_f1 = float(metrics.get("holdout_f1", 0.0))
    removed_tokens = float(metrics.get("removed_tokens", 0.0))

    if holdout_f1 < float(args.min_f1):
        raise ValueError(
            f"Promotion blocked: holdout_f1={holdout_f1:.4f} below min_f1={args.min_f1}."
        )

    if removed_tokens > float(args.max_removed_tokens):
        raise ValueError(
            f"Promotion blocked: removed_tokens={removed_tokens:.0f} exceeds max={args.max_removed_tokens}."
        )

    signature_present = isinstance(manifest.get("signature"), dict)
    if not args.allow_unsigned and not signature_present:
        raise ValueError("Promotion blocked: manifest signature is required.")

    if signature_present and args.signing_key_env:
        signing_key = __import__("os").environ.get(args.signing_key_env, "")
        if signing_key and not verify_signature(manifest, signing_key):
            raise ValueError("Promotion blocked: signature verification failed.")

    pointer = {
        "schema_version": 1,
        "channel": args.to_channel,
        "image_id": image_id,
        "updated_at_utc": utc_now(),
    }
    write_json(channel_file(registry_dir, args.to_channel), pointer)
    update_index(
        registry_dir,
        image_id,
        args.to_channel,
        metrics if isinstance(metrics, dict) else {},
    )

    print(
        json.dumps(
            {
                "promoted": True,
                "image_id": image_id,
                "to_channel": args.to_channel,
                "holdout_f1": holdout_f1,
            },
            indent=2,
        )
    )
    return 0


def status_registry(args: argparse.Namespace) -> int:
    registry_dir = Path(args.registry_dir)
    index = read_json(
        registry_dir / "index.json",
        {
            "schema_version": 1,
            "updated_at_utc": utc_now(),
            "images": {},
            "channels": {},
        },
    )

    channels: dict[str, dict] = {}
    for channel_path in sorted((registry_dir / "channels").glob("*.json")):
        channel_name = channel_path.stem
        channels[channel_name] = read_json(channel_path)

    payload = {
        "registry_dir": str(registry_dir),
        "updated_at_utc": index.get("updated_at_utc"),
        "image_count": len(index.get("images", {}))
        if isinstance(index.get("images"), dict)
        else 0,
        "channels": channels,
        "images": index.get("images", {}),
    }

    if args.json:
        print(json.dumps(payload, indent=2))
    else:
        for channel_name, channel_data in channels.items():
            print(f"{channel_name}: {channel_data.get('image_id', '<unset>')}")
    return 0


def upload_file(path: Path, url: str, upload_header: str | None) -> None:
    headers = {"Content-Type": "application/octet-stream"}
    if upload_header:
        if ":" not in upload_header:
            raise ValueError("Upload header must use the format 'Name: value'.")
        name, value = upload_header.split(":", 1)
        headers[name.strip()] = value.strip()

    data = path.read_bytes()
    request = urllib.request.Request(url=url, data=data, method="PUT", headers=headers)
    with urllib.request.urlopen(request, timeout=60) as response:
        if response.status >= 300:
            raise RuntimeError(f"Upload failed with status {response.status} for {url}")


def snapshot_registry(args: argparse.Namespace) -> int:
    registry_dir = Path(args.registry_dir)
    snapshot_dir = Path(args.snapshot_dir)
    snapshot_dir.mkdir(parents=True, exist_ok=True)

    label = args.label or f"snapshot-{datetime.now(timezone.utc):%Y%m%dT%H%M%SZ}"
    archive_path = snapshot_dir / f"{label}.tar.gz"

    with tarfile.open(archive_path, "w:gz") as archive:
        archive.add(registry_dir, arcname=registry_dir.name)

    digest = file_sha256(archive_path)
    sha_path = snapshot_dir / f"{label}.tar.gz.sha256"
    sha_path.write_text(f"{digest}  {archive_path.name}\n", encoding="utf-8")

    metadata = {
        "schema_version": 1,
        "label": label,
        "created_at_utc": utc_now(),
        "archive": str(archive_path),
        "sha256": digest,
        "archive_size_bytes": archive_path.stat().st_size,
    }
    metadata_path = snapshot_dir / f"{label}.json"
    write_json(metadata_path, metadata)

    if args.upload_url:
        upload_file(archive_path, args.upload_url, args.upload_header)
    if args.upload_sha_url:
        upload_file(sha_path, args.upload_sha_url, args.upload_header)

    print(json.dumps(metadata, indent=2))
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Manage not-banana model image registry."
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    create = subparsers.add_parser("create", help="Create a new immutable model image.")
    create.add_argument("--model-dir", required=True)
    create.add_argument("--registry-dir", required=True)
    create.add_argument("--channel", required=True)
    create.add_argument("--parent-from-channel")
    create.add_argument("--require-signing-key", action="store_true")
    create.add_argument("--signing-key-env", default="BANANA_MODEL_SIGNING_KEY")
    create.add_argument("--key-id-env", default="BANANA_MODEL_SIGNING_KEY_ID")
    create.set_defaults(func=create_image)

    verify = subparsers.add_parser(
        "verify", help="Verify image integrity and optional signature."
    )
    verify.add_argument("--registry-dir", required=True)
    verify.add_argument("--channel")
    verify.add_argument("--image-id")
    verify.add_argument("--require-signature", action="store_true")
    verify.add_argument("--signing-key-env", default="BANANA_MODEL_SIGNING_KEY")
    verify.set_defaults(func=verify_image)

    promote = subparsers.add_parser("promote", help="Promote an image to a channel.")
    promote.add_argument("--registry-dir", required=True)
    promote.add_argument("--from-channel")
    promote.add_argument("--to-channel", required=True)
    promote.add_argument("--image-id")
    promote.add_argument("--min-f1", type=float, default=0.8)
    promote.add_argument("--max-removed-tokens", type=float, default=4)
    promote.add_argument("--allow-unsigned", action="store_true")
    promote.add_argument("--signing-key-env", default="BANANA_MODEL_SIGNING_KEY")
    promote.set_defaults(func=promote_image)

    status = subparsers.add_parser("status", help="Print registry status.")
    status.add_argument("--registry-dir", required=True)
    status.add_argument("--json", action="store_true")
    status.set_defaults(func=status_registry)

    snapshot = subparsers.add_parser(
        "snapshot", help="Create a registry snapshot archive."
    )
    snapshot.add_argument("--registry-dir", required=True)
    snapshot.add_argument("--snapshot-dir", required=True)
    snapshot.add_argument("--label")
    snapshot.add_argument("--upload-url")
    snapshot.add_argument("--upload-sha-url")
    snapshot.add_argument("--upload-header")
    snapshot.set_defaults(func=snapshot_registry)

    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())

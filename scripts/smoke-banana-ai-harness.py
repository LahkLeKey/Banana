#!/usr/bin/env python3
"""Smoke validation for BananaAI harness outputs and runtime promotion artifacts."""

from __future__ import annotations

import argparse
import hashlib
import json
import sys
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any


REQUIRED_MODEL_FILES = ["vocabulary.json", "metrics.json", "sessions.json"]
REQUIRED_PROMOTION_FILES = ["vocabulary.json", "metrics.json", "sessions.json", "banana_signal_tokens.h"]
EXPECTED_MODELS = ["not-banana", "banana", "ripeness"]


@dataclass
class CheckResult:
    ok: bool = True
    failures: list[str] = field(default_factory=list)
    details: dict[str, Any] = field(default_factory=dict)

    def fail(self, message: str) -> None:
        self.ok = False
        self.failures.append(message)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Smoke-check BananaAI harness outputs.")
    parser.add_argument(
        "--manifest",
        help=(
            "Path to BananaAI joint manifest (session-*.json). "
            "If omitted, the latest artifacts/training/joint/session-*.json is used."
        ),
    )
    parser.add_argument(
        "--workspace-root",
        default=".",
        help="Workspace root for resolving relative artifact paths.",
    )
    parser.add_argument(
        "--skip-promotion-check",
        action="store_true",
        help="Skip validation of not-banana runtime promotion artifacts.",
    )
    return parser.parse_args()


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(65536), b""):
            digest.update(chunk)
    return digest.hexdigest()


def resolve_manifest_path(workspace_root: Path, explicit_manifest: str | None) -> Path:
    if explicit_manifest:
        manifest_path = Path(explicit_manifest)
        return manifest_path if manifest_path.is_absolute() else workspace_root / manifest_path

    candidate_dir = workspace_root / "artifacts" / "training" / "joint"
    candidates = sorted(candidate_dir.glob("session-*.json"))
    if not candidates:
        raise FileNotFoundError(
            f"no manifest found in {candidate_dir} (expected session-*.json)"
        )
    return candidates[-1]


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise ValueError(f"json payload at {path} was not an object")
    return payload


def resolve_path(workspace_root: Path, value: str) -> Path:
    candidate = Path(value)
    return candidate if candidate.is_absolute() else workspace_root / candidate


def validate_model_outputs(workspace_root: Path, manifest: dict[str, Any], result: CheckResult) -> None:
    results = manifest.get("results")
    if not isinstance(results, list):
        result.fail("manifest.results missing or invalid")
        return

    metrics_map = manifest.get("model_metrics")
    if not isinstance(metrics_map, dict):
        result.fail("manifest.model_metrics missing or invalid")
        return

    seen_models: set[str] = set()

    for entry in results:
        if not isinstance(entry, dict):
            result.fail("manifest.results contains non-object entry")
            continue

        model = entry.get("model")
        success = entry.get("success")
        output = entry.get("output")

        if not isinstance(model, str):
            result.fail("manifest.results entry missing model")
            continue

        seen_models.add(model)

        if success is not True:
            result.fail(f"model '{model}' did not report success=true")

        if not isinstance(output, str):
            result.fail(f"model '{model}' missing output path")
            continue

        output_dir = resolve_path(workspace_root, output)
        for file_name in REQUIRED_MODEL_FILES:
            file_path = output_dir / file_name
            if not file_path.exists():
                result.fail(f"model '{model}' missing file: {file_path}")

        model_metrics = metrics_map.get(model)
        if not isinstance(model_metrics, dict):
            result.fail(f"model_metrics missing entry for '{model}'")
            continue

        if model_metrics.get("present") is not True:
            result.fail(f"model '{model}' metrics payload missing")

        if model_metrics.get("meets_thresholds") is not True:
            result.fail(f"model '{model}' did not meet thresholds")

    for required_model in EXPECTED_MODELS:
        if required_model not in seen_models:
            result.fail(f"manifest missing result for required model '{required_model}'")


def validate_not_banana_promotion(
    workspace_root: Path,
    manifest: dict[str, Any],
    result: CheckResult,
) -> None:
    promotion = manifest.get("not_banana_runtime_promotion")
    if not isinstance(promotion, dict):
        result.fail("manifest.not_banana_runtime_promotion missing")
        return

    if promotion.get("attempted") is not True:
        result.fail("not-banana runtime promotion was not attempted")

    if promotion.get("promoted") is not True:
        result.fail("not-banana runtime promotion did not complete")
        return

    source = promotion.get("source")
    destinations = promotion.get("destinations")

    if not isinstance(source, str):
        result.fail("promotion source path missing")
        return

    if not isinstance(destinations, list) or not destinations:
        result.fail("promotion destinations missing")
        return

    source_dir = resolve_path(workspace_root, source)
    if not source_dir.exists():
        result.fail(f"promotion source directory missing: {source_dir}")
        return

    source_hashes: dict[str, str] = {}
    for file_name in REQUIRED_PROMOTION_FILES:
        file_path = source_dir / file_name
        if not file_path.exists():
            result.fail(f"promotion source missing file: {file_path}")
            continue
        source_hashes[file_name] = sha256_file(file_path)

    for destination in destinations:
        if not isinstance(destination, str):
            result.fail("promotion destination path was not a string")
            continue

        destination_dir = resolve_path(workspace_root, destination)
        if not destination_dir.exists():
            result.fail(f"promotion destination missing: {destination_dir}")
            continue

        for file_name in REQUIRED_PROMOTION_FILES:
            destination_file = destination_dir / file_name
            if not destination_file.exists():
                result.fail(f"promotion destination missing file: {destination_file}")
                continue

            source_hash = source_hashes.get(file_name)
            if source_hash is None:
                continue

            destination_hash = sha256_file(destination_file)
            if destination_hash != source_hash:
                result.fail(
                    f"promotion hash mismatch for {destination_file} (expected {source_hash}, got {destination_hash})"
                )


def main() -> int:
    args = parse_args()
    workspace_root = Path(args.workspace_root).resolve()
    result = CheckResult()

    try:
        manifest_path = resolve_manifest_path(workspace_root, args.manifest)
    except FileNotFoundError as error:
        result.fail(str(error))
        print(json.dumps({"ok": result.ok, "failures": result.failures}, indent=2))
        return 1

    result.details["manifest"] = str(manifest_path)

    try:
        manifest = load_json(manifest_path)
    except (OSError, json.JSONDecodeError, ValueError) as error:
        result.fail(f"manifest_load_error:{error}")
        print(json.dumps({"ok": result.ok, "failures": result.failures}, indent=2))
        return 1

    if manifest.get("harness") != "banana-ai":
        result.fail("manifest.harness was not 'banana-ai'")

    if manifest.get("all_success") is not True:
        result.fail("manifest.all_success was not true")

    validate_model_outputs(workspace_root, manifest, result)

    if not args.skip_promotion_check:
        validate_not_banana_promotion(workspace_root, manifest, result)

    output = {
        "ok": result.ok,
        "manifest": result.details.get("manifest"),
        "failures": result.failures,
    }
    print(json.dumps(output, indent=2))
    return 0 if result.ok else 1


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Regenerate Banana.Api coverage denominator manifest from current source files."""

from __future__ import annotations

import argparse
import fnmatch
import json
import pathlib
import sys


def fail(message: str) -> None:
    print(message, file=sys.stderr)
    raise SystemExit(1)


def normalize_path(value: str) -> str:
    return value.replace("\\", "/")


def discover_files(root_dir: pathlib.Path, manifest: dict) -> list[str]:
    source_root_raw = manifest.get("sourceRoot")
    if not isinstance(source_root_raw, str) or not source_root_raw:
        fail("Manifest must define sourceRoot as a non-empty string.")

    source_root = (root_dir / source_root_raw).resolve()
    if not source_root.exists() or not source_root.is_dir():
        fail(f"Manifest sourceRoot does not exist: {source_root_raw}")

    include_patterns = manifest.get("includePatterns")
    if not isinstance(include_patterns, list) or not include_patterns:
        fail("Manifest must define includePatterns as a non-empty array.")

    exclude_patterns = manifest.get("excludePatterns", [])
    if not isinstance(exclude_patterns, list):
        fail("Manifest excludePatterns must be an array.")

    files: set[str] = set()
    for pattern in include_patterns:
        if not isinstance(pattern, str) or not pattern:
            fail("Manifest includePatterns entries must be non-empty strings.")

        for candidate in source_root.glob(pattern):
            if not candidate.is_file():
                continue

            root_relative = normalize_path(str(candidate.relative_to(root_dir)))
            source_relative = normalize_path(str(candidate.relative_to(source_root)))

            excluded = False
            for exclude_pattern in exclude_patterns:
                if not isinstance(exclude_pattern, str):
                    fail("Manifest excludePatterns entries must be strings.")
                if fnmatch.fnmatch(root_relative, exclude_pattern) or fnmatch.fnmatch(source_relative, exclude_pattern):
                    excluded = True
                    break

            if not excluded:
                files.add(root_relative)

    return sorted(files)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--manifest",
        default="src/c-sharp/asp.net/coverage-denominator.json",
        help="Path to Banana.Api coverage denominator manifest.",
    )
    args = parser.parse_args()

    root_dir = pathlib.Path(__file__).resolve().parents[1]
    manifest_path = pathlib.Path(args.manifest)
    if not manifest_path.is_absolute():
        manifest_path = root_dir / manifest_path

    if not manifest_path.exists():
        fail(f"Manifest not found: {manifest_path}")

    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as exc:
        fail(f"Manifest is not valid JSON: {exc}")

    if not isinstance(manifest, dict):
        fail("Manifest root must be a JSON object.")

    if manifest.get("schemaVersion") != 1:
        fail("Manifest schemaVersion must be 1.")

    if manifest.get("project") != "Banana.Api":
        fail("Manifest project must be 'Banana.Api'.")

    files = discover_files(root_dir, manifest)
    updated_manifest = dict(manifest)
    updated_manifest["files"] = files

    serialized = json.dumps(updated_manifest, indent=4) + "\n"
    current = manifest_path.read_text(encoding="utf-8")
    if current == serialized:
        print(f"Banana.Api coverage denominator manifest already current ({len(files)} files).")
        return 0

    manifest_path.write_text(serialized, encoding="utf-8")
    print(f"Updated Banana.Api coverage denominator manifest ({len(files)} files): {manifest_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

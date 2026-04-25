#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

MANIFEST_PATH="${1:-src/c-sharp/asp.net/coverage-denominator.json}"

python - "$ROOT_DIR" "$MANIFEST_PATH" <<'PY'
import fnmatch
import json
import pathlib
import sys


def fail(message: str) -> None:
    print(message, file=sys.stderr)
    raise SystemExit(1)


root_dir = pathlib.Path(sys.argv[1]).resolve()
manifest_arg = pathlib.Path(sys.argv[2])
manifest_path = manifest_arg if manifest_arg.is_absolute() else (root_dir / manifest_arg)

if not manifest_path.exists():
    fail(f"Coverage denominator manifest not found: {manifest_path}")

try:
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
except json.JSONDecodeError as exc:
    fail(f"Coverage denominator manifest is not valid JSON: {exc}")

if not isinstance(manifest, dict):
    fail("Coverage denominator manifest root must be a JSON object.")

if manifest.get("schemaVersion") != 1:
    fail("Coverage denominator manifest schemaVersion must be 1.")

project_name = manifest.get("project")
if project_name != "Banana.Api":
    fail("Coverage denominator manifest project must be 'Banana.Api'.")

source_root_raw = manifest.get("sourceRoot")
if not isinstance(source_root_raw, str) or not source_root_raw:
    fail("Coverage denominator manifest must define sourceRoot as a non-empty string.")

source_root = (root_dir / source_root_raw).resolve()
if not source_root.exists() or not source_root.is_dir():
    fail(f"Coverage denominator sourceRoot does not exist: {source_root_raw}")

include_patterns = manifest.get("includePatterns")
if not isinstance(include_patterns, list) or not include_patterns:
    fail("Coverage denominator manifest must define a non-empty includePatterns array.")

exclude_patterns = manifest.get("excludePatterns", [])
if not isinstance(exclude_patterns, list):
    fail("Coverage denominator manifest excludePatterns must be an array when provided.")

manifest_files = manifest.get("files")
if not isinstance(manifest_files, list):
    fail("Coverage denominator manifest must define files as an array.")

normalized_manifest_files = []
for item in manifest_files:
    if not isinstance(item, str) or not item:
        fail("Coverage denominator manifest files entries must be non-empty strings.")
    normalized_manifest_files.append(item.replace("\\", "/"))

if normalized_manifest_files != sorted(normalized_manifest_files):
    fail("Coverage denominator manifest files must be sorted lexicographically.")

if len(normalized_manifest_files) != len(set(normalized_manifest_files)):
    fail("Coverage denominator manifest files contains duplicate entries.")


def is_excluded(root_relative: str, source_relative: str) -> bool:
    for pattern in exclude_patterns:
        if not isinstance(pattern, str):
            fail("Coverage denominator manifest excludePatterns entries must be strings.")
        if fnmatch.fnmatch(root_relative, pattern) or fnmatch.fnmatch(source_relative, pattern):
            return True
    return False


actual_files = []
for pattern in include_patterns:
    if not isinstance(pattern, str) or not pattern:
        fail("Coverage denominator manifest includePatterns entries must be non-empty strings.")

    for candidate in source_root.glob(pattern):
        if not candidate.is_file():
            continue

        root_relative = candidate.relative_to(root_dir).as_posix()
        source_relative = candidate.relative_to(source_root).as_posix()

        if is_excluded(root_relative, source_relative):
            continue

        actual_files.append(root_relative)

actual_files = sorted(set(actual_files))
expected_set = set(normalized_manifest_files)
actual_set = set(actual_files)

missing_from_manifest = sorted(actual_set - expected_set)
stale_in_manifest = sorted(expected_set - actual_set)

if missing_from_manifest or stale_in_manifest:
    print("Banana.Api coverage denominator drift detected.", file=sys.stderr)
    if missing_from_manifest:
        print("Missing entries in manifest:", file=sys.stderr)
        for path in missing_from_manifest:
            print(f"  + {path}", file=sys.stderr)
    if stale_in_manifest:
        print("Stale entries in manifest:", file=sys.stderr)
        for path in stale_in_manifest:
            print(f"  - {path}", file=sys.stderr)
    raise SystemExit(1)

print(f"Banana.Api coverage denominator manifest is current ({len(actual_files)} files).")
PY

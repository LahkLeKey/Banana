#!/usr/bin/env python3
"""Synchronize or validate the Banana.Api coverage denominator manifest."""

from __future__ import annotations

import argparse
import json
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
API_ROOT = ROOT / "src" / "c-sharp" / "asp.net"
DEFAULT_MANIFEST = API_ROOT / "coverage-denominator.json"


def utc_now() -> str:
    return datetime.now(timezone.utc).replace(microsecond=0).isoformat().replace("+00:00", "Z")


def collect_cs_files(api_root: Path) -> list[str]:
    files: list[str] = []
    for candidate in sorted(api_root.rglob("*.cs")):
        relative_parts = candidate.relative_to(api_root).parts
        if "obj" in relative_parts or "bin" in relative_parts:
            continue
        files.append(candidate.relative_to(api_root).as_posix())
    return files


def build_manifest(files: list[str]) -> dict:
    return {
        "schema_version": 1,
        "project": "Banana.Api",
        "generated_at_utc": utc_now(),
        "tracked_file_count": len(files),
        "tracked_files": files,
    }


def check_manifest(manifest_path: Path, expected_files: list[str]) -> tuple[bool, dict]:
    if not manifest_path.exists():
        return False, {
            "ok": False,
            "error": f"Manifest missing at {manifest_path}",
            "expected_count": len(expected_files),
        }

    payload = json.loads(manifest_path.read_text(encoding="utf-8"))
    tracked_files = payload.get("tracked_files", [])
    if not isinstance(tracked_files, list):
        return False, {"ok": False, "error": "tracked_files must be an array."}

    tracked_set = set(str(item) for item in tracked_files)
    expected_set = set(expected_files)
    added = sorted(expected_set - tracked_set)
    removed = sorted(tracked_set - expected_set)

    count_value = payload.get("tracked_file_count")
    count_matches = count_value == len(expected_files)
    list_matches = not added and not removed
    ok = bool(count_matches and list_matches)

    report = {
        "ok": ok,
        "manifest_path": str(manifest_path),
        "tracked_file_count": payload.get("tracked_file_count"),
        "expected_file_count": len(expected_files),
        "added": added,
        "removed": removed,
    }
    return ok, report


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Sync or validate coverage-denominator manifest.")
    parser.add_argument("--manifest", default=str(DEFAULT_MANIFEST))
    parser.add_argument("--check", action="store_true", help="Validate manifest instead of writing.")
    parser.add_argument("--quiet", action="store_true", help="Suppress report output.")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    manifest_path = Path(args.manifest)

    expected_files = collect_cs_files(API_ROOT)
    if args.check:
        ok, report = check_manifest(manifest_path, expected_files)
        if not args.quiet:
            print(json.dumps(report, indent=2))
        return 0 if ok else 1

    manifest = build_manifest(expected_files)
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    if not args.quiet:
        print(json.dumps({"written": True, "manifest": str(manifest_path), "count": len(expected_files)}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

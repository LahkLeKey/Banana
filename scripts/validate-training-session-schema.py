#!/usr/bin/env python3
"""Validate persisted training-session history under data/*/sessions."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

REQUIRED_FIELDS = {
    "session_id",
    "lane",
    "corpus_id",
    "training_profile",
    "session_mode",
    "max_sessions",
    "selected_session",
    "thresholds",
    "metrics",
    "run_fingerprint",
    "generated_at_utc",
    "artifact_paths",
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Validate persisted training-session JSONL history.")
    parser.add_argument(
        "paths",
        nargs="*",
        help="Optional files to validate. If omitted, validates data/*/sessions/history.jsonl.",
    )
    return parser.parse_args()


def discover_files(paths: list[str]) -> list[Path]:
    if paths:
        return [Path(item) for item in paths]
    return sorted(Path("data").glob("*/sessions/history.jsonl"))


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def _is_relative_path(value: Any) -> bool:
    if not isinstance(value, str) or not value.strip():
        return False
    return not Path(value).is_absolute()


def validate_record(record: dict[str, Any], path: Path, line_number: int) -> None:
    missing = sorted(REQUIRED_FIELDS - set(record.keys()))
    expect(not missing, f"{path}:{line_number}: missing required fields: {', '.join(missing)}")

    expect(record.get("schema_version") == 1, f"{path}:{line_number}: schema_version must equal 1")
    expect(isinstance(record["thresholds"], dict), f"{path}:{line_number}: thresholds must be an object")
    expect(isinstance(record["metrics"], dict), f"{path}:{line_number}: metrics must be an object")
    expect(isinstance(record["artifact_paths"], dict), f"{path}:{line_number}: artifact_paths must be an object")

    for key in ("output_dir", "vocabulary", "metrics", "sessions", "header"):
        expect(
            _is_relative_path(record["artifact_paths"].get(key)),
            f"{path}:{line_number}: artifact_paths.{key} must be a non-empty relative path",
        )


def validate_file(path: Path) -> int:
    expect(path.exists(), f"Missing training-session file: {path}")
    count = 0
    for line_number, raw_line in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        if not raw_line.strip():
            continue
        try:
            record = json.loads(raw_line)
        except json.JSONDecodeError as error:
            raise ValueError(f"{path}:{line_number}: invalid JSON line ({error})") from error
        expect(isinstance(record, dict), f"{path}:{line_number}: record must be a JSON object")
        validate_record(record, path, line_number)
        count += 1
    expect(count > 0, f"{path}: expected at least one training-session record")
    return count


def main() -> int:
    args = parse_args()
    files = discover_files(args.paths)
    expect(bool(files), "No training-session history files found to validate.")

    summary: dict[str, int] = {}
    for file_path in files:
        summary[str(file_path)] = validate_file(file_path)

    print(json.dumps({"validated": summary}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

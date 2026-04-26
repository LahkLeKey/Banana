#!/usr/bin/env python3
"""Validate data/*/corpus.json files against the shared schema contract."""

from __future__ import annotations

import argparse
import difflib
import json
import re
from pathlib import Path
from typing import Any

NORMALIZE_SPACE_RE = re.compile(r"\s+")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Validate Banana corpus schema files.")
    parser.add_argument(
        "paths",
        nargs="*",
        help="Specific corpus.json paths. If empty, validates data/*/corpus.json.",
    )
    return parser.parse_args()


def discover_paths(args: argparse.Namespace) -> list[Path]:
    if args.paths:
        return [Path(item) for item in args.paths]
    return sorted(Path("data").glob("*/corpus.json"))


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise ValueError(message)


def validate_sample(sample: Any, index: int, path: Path) -> None:
    expect(isinstance(sample, dict), f"{path}: sample #{index} must be an object")
    for key in ("id", "label", "text"):
        value = sample.get(key)
        expect(isinstance(value, str) and value.strip(), f"{path}: sample #{index} missing non-empty '{key}'")


def normalize_text(value: str) -> str:
    lowered = value.strip().lower()
    collapsed = NORMALIZE_SPACE_RE.sub(" ", lowered)
    return collapsed


def near_duplicate_score(a: str, b: str) -> float:
    return difflib.SequenceMatcher(None, a, b, autojunk=False).ratio()


def validate_corpus(path: Path) -> None:
    expect(path.exists(), f"missing corpus file: {path}")
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"{path}: root must be an object")
    expect(isinstance(payload.get("schema_version"), int), f"{path}: schema_version must be an int")
    expect(isinstance(payload.get("description"), str) and payload["description"].strip(), f"{path}: description must be non-empty")
    samples = payload.get("samples")
    expect(isinstance(samples, list) and samples, f"{path}: samples must be a non-empty array")
    normalized_texts: list[tuple[int, str]] = []
    for idx, sample in enumerate(samples, start=1):
        validate_sample(sample, idx, path)
        normalized_texts.append((idx, normalize_text(str(sample.get("text", "")))))

    seen: dict[str, int] = {}
    for idx, normalized in normalized_texts:
        previous = seen.get(normalized)
        if previous is not None:
            raise ValueError(
                f"{path}: duplicate sample text detected between samples #{previous} and #{idx}"
            )
        seen[normalized] = idx

    for left_index in range(len(normalized_texts)):
        left_number, left_text = normalized_texts[left_index]
        for right_index in range(left_index + 1, len(normalized_texts)):
            right_number, right_text = normalized_texts[right_index]
            similarity = near_duplicate_score(left_text, right_text)
            if similarity >= 0.93:
                raise ValueError(
                    f"{path}: near-duplicate sample text detected between samples "
                    f"#{left_number} and #{right_number} (similarity={similarity:.3f})"
                )


def main() -> int:
    args = parse_args()
    paths = discover_paths(args)
    if not paths:
        raise FileNotFoundError("No corpus files found to validate.")

    for path in paths:
        validate_corpus(path)
    print(json.dumps({"validated": [str(p) for p in paths]}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

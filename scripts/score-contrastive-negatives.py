#!/usr/bin/env python3
"""Score contrastive hard-negative set for not-banana precision floor."""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter
from pathlib import Path

TOKEN_PATTERN = re.compile(r"[a-z0-9']+")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Score contrastive hard negatives")
    parser.add_argument(
        "--dataset",
        type=Path,
        default=Path("data/not-banana/hard-negatives.json"),
        help="Path to hard-negative dataset",
    )
    parser.add_argument(
        "--vocabulary",
        type=Path,
        default=Path("artifacts/training/not-banana/local/vocabulary.json"),
        help="Path to not-banana vocabulary artifact",
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=Path("artifacts/training/not-banana/local/contrastive-negatives.json"),
        help="Path to write contrastive summary",
    )
    parser.add_argument(
        "--min-contrastive-precision",
        type=float,
        default=0.5,
        help="Minimum accuracy floor over hard-negative examples",
    )
    parser.add_argument("--strict", action="store_true", help="Fail when floor is not met")
    return parser.parse_args()


def load_weights(path: Path) -> dict[str, float]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    weights: dict[str, float] = {}
    for row in payload.get("vocabulary", []):
        if not isinstance(row, dict):
            continue
        token = str(row.get("token", "")).strip().lower()
        if token:
            weights[token] = float(row.get("weight", 0.0))
    return weights


def tokenize(text: str) -> set[str]:
    return {raw.strip("'") for raw in TOKEN_PATTERN.findall(text.lower()) if raw.strip("'")}


def predict_label(text: str, weights: dict[str, float]) -> str:
    score = sum(weights.get(token, 0.0) for token in tokenize(text))
    return "banana" if score > 0 else "not-banana"


def safe_div(a: float, b: float) -> float:
    return 0.0 if b == 0 else a / b


def main() -> int:
    args = parse_args()
    dataset = json.loads(args.dataset.read_text(encoding="utf-8"))
    samples = dataset.get("samples", [])
    if not isinstance(samples, list) or not samples:
        raise ValueError(f"{args.dataset}: samples must be non-empty")

    weights = load_weights(args.vocabulary)
    per_category_total: Counter[str] = Counter()
    per_category_correct: Counter[str] = Counter()
    false_positive_samples: list[dict[str, str]] = []

    total = 0
    correct = 0
    for item in samples:
        if not isinstance(item, dict):
            continue
        text = str(item.get("text", "")).strip()
        expected = str(item.get("expected_label", "not-banana")).strip().lower().replace("_", "-")
        category = str(item.get("category", "unknown")).strip() or "unknown"
        if not text:
            continue
        predicted = predict_label(text, weights)
        matched = predicted == expected

        total += 1
        per_category_total[category] += 1
        if matched:
            correct += 1
            per_category_correct[category] += 1
        else:
            false_positive_samples.append(
                {
                    "id": str(item.get("id", "")),
                    "category": category,
                    "expected": expected,
                    "predicted": predicted,
                    "text": text,
                }
            )

    category_accuracy = {
        k: round(safe_div(per_category_correct[k], v), 6)
        for k, v in sorted(per_category_total.items())
    }
    contrastive_precision = round(safe_div(correct, total), 6)

    summary = {
        "schema_version": 1,
        "dataset_path": str(args.dataset),
        "vocabulary_path": str(args.vocabulary),
        "sample_count": total,
        "contrastive_precision": contrastive_precision,
        "min_contrastive_precision": round(float(args.min_contrastive_precision), 6),
        "per_category_accuracy": category_accuracy,
        "false_positive_count": len(false_positive_samples),
        "false_positive_samples": false_positive_samples,
    }

    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))

    if args.strict and contrastive_precision < float(args.min_contrastive_precision):
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

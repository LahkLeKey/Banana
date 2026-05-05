#!/usr/bin/env python3
"""Score banana concept generalization benchmark against trained vocabulary."""

from __future__ import annotations

import argparse
import json
import re
from collections import Counter, defaultdict
from pathlib import Path
from typing import Any

TOKEN_PATTERN = re.compile(r"[a-z0-9']+")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Score banana generalization benchmark")
    parser.add_argument(
        "--benchmark",
        type=Path,
        default=Path("data/banana/generalization-benchmark.json"),
        help="Path to generalization benchmark JSON",
    )
    parser.add_argument(
        "--vocabulary",
        type=Path,
        default=Path("artifacts/training/banana/local/vocabulary.json"),
        help="Path to banana vocabulary artifact",
    )
    parser.add_argument(
        "--report",
        type=Path,
        default=Path("artifacts/training/banana/local/generalization-benchmark.json"),
        help="Path to write benchmark summary JSON",
    )
    parser.add_argument(
        "--min-lane-recall",
        type=float,
        default=0.65,
        help="Minimum concept lane accuracy/recall floor",
    )
    parser.add_argument(
        "--min-context-accuracy",
        type=float,
        default=0.65,
        help="Minimum context-family accuracy floor",
    )
    parser.add_argument(
        "--strict",
        action="store_true",
        help="Return non-zero when any threshold fails",
    )
    return parser.parse_args()


def load_weights(path: Path) -> dict[str, float]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    vocab = payload.get("vocabulary", [])
    if not isinstance(vocab, list):
        raise ValueError(f"{path}: vocabulary must be a list")

    weights: dict[str, float] = {}
    for row in vocab:
        if not isinstance(row, dict):
            continue
        token = str(row.get("token", "")).strip().lower()
        if not token:
            continue
        weights[token] = float(row.get("weight", 0.0))
    return weights


def tokenize(text: str) -> set[str]:
    tokens: set[str] = set()
    for raw in TOKEN_PATTERN.findall(text.lower()):
        token = raw.strip("'")
        if token:
            tokens.add(token)
    return tokens


def predict_label(text: str, weights: dict[str, float]) -> str:
    score = sum(weights.get(token, 0.0) for token in tokenize(text))
    return "banana" if score > 0 else "not-banana"


def safe_divide(a: float, b: float) -> float:
    return 0.0 if b == 0 else a / b


def main() -> int:
    args = parse_args()
    benchmark = json.loads(args.benchmark.read_text(encoding="utf-8"))
    samples = benchmark.get("samples", [])
    if not isinstance(samples, list) or not samples:
        raise ValueError(f"{args.benchmark}: samples must be a non-empty list")

    weights = load_weights(args.vocabulary)

    per_concept_total: Counter[str] = Counter()
    per_concept_correct: Counter[str] = Counter()
    per_context_total: Counter[str] = Counter()
    per_context_correct: Counter[str] = Counter()
    confusion_clusters: defaultdict[str, Counter[str]] = defaultdict(Counter)

    total = 0
    correct = 0

    for item in samples:
        if not isinstance(item, dict):
            continue
        text = str(item.get("text", "")).strip()
        expected = str(item.get("expected_label", "")).strip().lower().replace("_", "-")
        concept = str(item.get("concept", "unknown")).strip() or "unknown"
        context = str(item.get("context_family", "unknown")).strip() or "unknown"
        if not text or expected not in {"banana", "not-banana"}:
            continue

        predicted = predict_label(text, weights)
        matched = predicted == expected

        total += 1
        if matched:
            correct += 1
        per_concept_total[concept] += 1
        per_context_total[context] += 1
        if matched:
            per_concept_correct[concept] += 1
            per_context_correct[context] += 1
        else:
            confusion_clusters[context][f"{expected}->{predicted}"] += 1

    lane_scores = {
        concept: round(safe_divide(per_concept_correct[concept], count), 6)
        for concept, count in sorted(per_concept_total.items())
    }
    context_scores = {
        context: round(safe_divide(per_context_correct[context], count), 6)
        for context, count in sorted(per_context_total.items())
    }

    failing_lanes = {
        k: v for k, v in lane_scores.items() if v < float(args.min_lane_recall)
    }
    failing_contexts = {
        k: v
        for k, v in context_scores.items()
        if v < float(args.min_context_accuracy) and int(per_context_total.get(k, 0)) >= 2
    }

    top_confusions: list[dict[str, Any]] = []
    for context, counter in sorted(confusion_clusters.items()):
        for edge, count in counter.most_common(3):
            top_confusions.append({"context_family": context, "edge": edge, "count": int(count)})

    summary = {
        "schema_version": 1,
        "benchmark_path": str(args.benchmark),
        "vocabulary_path": str(args.vocabulary),
        "sample_count": total,
        "overall_accuracy": round(safe_divide(correct, total), 6),
        "per_concept_recall": lane_scores,
        "per_context_accuracy": context_scores,
        "min_lane_recall": round(float(args.min_lane_recall), 6),
        "min_context_accuracy": round(float(args.min_context_accuracy), 6),
        "failing_lanes": failing_lanes,
        "failing_contexts": failing_contexts,
        "top_confusion_clusters": top_confusions,
    }

    args.report.parent.mkdir(parents=True, exist_ok=True)
    args.report.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))

    if args.strict and (failing_lanes or failing_contexts):
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

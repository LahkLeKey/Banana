#!/usr/bin/env python3
"""Predictive-coding surprise sampler.

Spec ref: FR-003 (feature 050).

Reads the most recent model artifact (``vocabulary.json``) plus a corpus, and
emits per-sample sampling weights = ``0.7 * uniform + 0.3 * normalized_surprise``.

Surprise for sample i is defined as ``1 - sigmoid(|prediction_score|)`` so that
samples the model already classifies confidently get low surprise and samples
near the decision boundary get high surprise. This is a tractable proxy for
free-energy / prediction-error in the Rao-Ballard / Friston sense.
"""

from __future__ import annotations

import argparse
import json
import math
import re
from pathlib import Path

TOKEN_PATTERN = re.compile(r"[a-z0-9']+")


def tokenize(text: str) -> list[str]:
    return [t.strip("'") for t in TOKEN_PATTERN.findall(text.lower()) if t.strip("'")]


def load_weights(vocabulary_path: Path) -> dict[str, float]:
    payload = json.loads(vocabulary_path.read_text(encoding="utf-8"))
    vocab = payload.get("vocabulary", [])
    return {str(entry["token"]): float(entry["weight"]) for entry in vocab}


def load_corpus_texts(corpus_path: Path) -> list[str]:
    payload = json.loads(corpus_path.read_text(encoding="utf-8"))
    raw = payload.get("samples", payload) if isinstance(payload, dict) else payload
    return [str(item.get("text", "")) for item in raw if isinstance(item, dict)]


def sigmoid(x: float) -> float:
    if x >= 0:
        z = math.exp(-x)
        return 1.0 / (1.0 + z)
    z = math.exp(x)
    return z / (1.0 + z)


def compute_surprise(texts: list[str], weights: dict[str, float]) -> list[float]:
    """Return per-text surprise in [0, 1]; uniform 0.5 when vocab is empty."""
    if not weights:
        return [0.5] * len(texts)
    raw: list[float] = []
    for text in texts:
        tokens = set(tokenize(text))
        score = sum(weights.get(t, 0.0) for t in tokens)
        raw.append(1.0 - sigmoid(abs(score)))
    return raw


def normalize(values: list[float]) -> list[float]:
    if not values:
        return []
    total = sum(values)
    if total <= 0:
        n = len(values)
        return [1.0 / n] * n
    return [v / total for v in values]


def blend_with_uniform_floor(surprise_norm: list[float], floor: float = 0.7) -> list[float]:
    """``floor * uniform + (1 - floor) * surprise``; default keeps 70% uniform."""
    if not surprise_norm:
        return []
    n = len(surprise_norm)
    uniform = 1.0 / n
    blend = 1.0 - floor
    return [floor * uniform + blend * s for s in surprise_norm]


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Banana neuro surprise sampler.")
    parser.add_argument("--vocabulary", type=Path, required=True)
    parser.add_argument("--corpus", type=Path, required=True)
    parser.add_argument("--floor", type=float, default=0.7)
    parser.add_argument("--output", type=Path, default=None, help="Write JSON; default stdout.")
    return parser.parse_args(argv)


def build_payload(vocabulary_path: Path, corpus_path: Path, floor: float) -> dict:
    weights = load_weights(vocabulary_path) if vocabulary_path.exists() else {}
    texts = load_corpus_texts(corpus_path)
    surprise_raw = compute_surprise(texts, weights)
    surprise_norm = normalize(surprise_raw)
    blended = blend_with_uniform_floor(surprise_norm, floor=floor)
    # Round for reviewable diffs.
    blended = [round(v, 6) for v in blended]
    return {
        "schema_version": 1,
        "floor": floor,
        "sample_count": len(texts),
        "surprise_weights": blended,
    }


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    payload = build_payload(args.vocabulary, args.corpus, args.floor)
    text = json.dumps(payload, indent=2, sort_keys=True) + "\n"
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(text, encoding="utf-8")
    else:
        print(text, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

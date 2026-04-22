#!/usr/bin/env python3
"""Train the polymorphic not-banana signal vocabulary from a labeled corpus.

The trainer learns which lowercase ASCII tokens most reliably indicate that a
polymorphic /not-banana/junk payload is actually a BANANA. It emits:

  - vocabulary.json  : learned tokens with per-token signal scores and overall
                       evaluation metrics
  - banana_signal_tokens.h : generated C header that can replace the inline
                             vocabulary in
                             src/native/core/domain/banana_not_banana.c
  - metrics.json     : precision/recall/F1/accuracy on a deterministic hold-out
                       split

The implementation is intentionally dependency-free (Python standard library
only) so it runs identically in local and CI environments.
"""

from __future__ import annotations

import argparse
import json
import os
import random
import re
import sys
from collections import Counter
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Iterable

TOKEN_REGEX = re.compile(r"[a-z0-9]+")
MAX_DEPTH = 5
MAX_CHILDREN_PER_NODE = 128
SPLIT_SEED = 1729
DEFAULT_HOLDOUT_FRACTION = 0.3
DEFAULT_VOCAB_SIZE = 24
DEFAULT_MIN_BANANA_OCCURRENCES = 1
DEFAULT_MIN_SIGNAL_SCORE = 0.7

# Structural / polymorphic-schema keys and ids that appear in both classes
# and therefore carry no class signal. They are excluded so the learned
# vocabulary represents real domain words instead of envelope fields.
STRUCTURAL_STOP_WORDS = frozenset(
    {
        "actor",
        "actorid",
        "actorkey",
        "actors",
        "actortype",
        "entity",
        "entityid",
        "entitykey",
        "entities",
        "entitytype",
        "junk",
        "kind",
        "metadata",
        "type",
        "id",
        "key",
    }
)


@dataclass
class Sample:
    label: str
    payload: dict[str, Any]
    tokens: set[str] = field(default_factory=set)


def tokenise_string(value: str) -> Iterable[str]:
    return TOKEN_REGEX.findall(value.lower())


def _add_token(token: str, tokens: set[str]) -> None:
    if token and token not in STRUCTURAL_STOP_WORDS:
        tokens.add(token)


def collect_tokens(value: Any, tokens: set[str], depth: int = 0) -> None:
    if depth > MAX_DEPTH or value is None:
        return

    if isinstance(value, str):
        for token in tokenise_string(value):
            _add_token(token, tokens)
        return

    if isinstance(value, bool):
        _add_token("true" if value else "false", tokens)
        return

    if isinstance(value, (int, float)):
        _add_token(str(value).lower(), tokens)
        return

    if isinstance(value, list):
        for entry in value[:MAX_CHILDREN_PER_NODE]:
            collect_tokens(entry, tokens, depth + 1)
        return

    if isinstance(value, dict):
        for key, entry in list(value.items())[:MAX_CHILDREN_PER_NODE]:
            for token in tokenise_string(str(key)):
                _add_token(token, tokens)
            collect_tokens(entry, tokens, depth + 1)


def load_corpus(path: Path) -> list[Sample]:
    raw = json.loads(path.read_text(encoding="utf-8"))
    samples_raw = raw.get("samples")
    if not isinstance(samples_raw, list) or not samples_raw:
        raise ValueError(f"Corpus at {path} must contain a non-empty 'samples' array.")

    samples: list[Sample] = []
    for index, entry in enumerate(samples_raw):
        if not isinstance(entry, dict):
            raise ValueError(f"Sample #{index} must be an object.")

        label = entry.get("label")
        payload = entry.get("payload")

        if label not in {"BANANA", "NOT_BANANA"}:
            raise ValueError(f"Sample #{index} has invalid label '{label}'.")
        if not isinstance(payload, dict):
            raise ValueError(f"Sample #{index} payload must be an object.")

        tokens: set[str] = set()
        collect_tokens(payload, tokens)
        samples.append(Sample(label=label, payload=payload, tokens=tokens))

    return samples


def deterministic_split(
    samples: list[Sample], holdout_fraction: float
) -> tuple[list[Sample], list[Sample]]:
    if not 0.0 < holdout_fraction < 1.0:
        raise ValueError("holdout_fraction must be between 0 and 1 exclusive.")

    rng = random.Random(SPLIT_SEED)
    indices = list(range(len(samples)))
    rng.shuffle(indices)

    holdout_size = max(1, int(round(len(samples) * holdout_fraction)))
    holdout_indices = set(indices[:holdout_size])

    train = [s for i, s in enumerate(samples) if i not in holdout_indices]
    holdout = [s for i, s in enumerate(samples) if i in holdout_indices]
    return train, holdout


def learn_vocabulary(
    train: list[Sample],
    vocab_size: int,
    min_banana_occurrences: int,
    min_signal_score: float,
) -> list[dict[str, Any]]:
    banana_counts: Counter[str] = Counter()
    not_banana_counts: Counter[str] = Counter()

    for sample in train:
        target = banana_counts if sample.label == "BANANA" else not_banana_counts
        for token in sample.tokens:
            target[token] += 1

    total_banana = sum(1 for s in train if s.label == "BANANA") or 1
    total_not_banana = sum(1 for s in train if s.label == "NOT_BANANA") or 1

    candidates: list[dict[str, Any]] = []
    for token, banana_count in banana_counts.items():
        if banana_count < min_banana_occurrences:
            continue

        not_banana_count = not_banana_counts.get(token, 0)
        banana_rate = banana_count / total_banana
        not_banana_rate = not_banana_count / total_not_banana
        # Laplace-smoothed signal score in [0, 1]; tokens that appear mostly in
        # BANANA samples score higher.
        signal_score = (banana_rate + 1e-3) / (banana_rate + not_banana_rate + 2e-3)

        if signal_score < min_signal_score:
            continue

        candidates.append(
            {
                "token": token,
                "banana_count": banana_count,
                "not_banana_count": not_banana_count,
                "banana_rate": round(banana_rate, 4),
                "not_banana_rate": round(not_banana_rate, 4),
                "signal_score": round(signal_score, 4),
            }
        )

    candidates.sort(
        key=lambda entry: (-entry["signal_score"], -entry["banana_count"], entry["token"])
    )
    return candidates[:vocab_size]


def classify(
    tokens: set[str],
    vocabulary_scores: dict[str, float],
    decision_threshold: float,
) -> str:
    if not tokens:
        return "NOT_BANANA"

    score = sum(vocabulary_scores.get(token, 0.0) for token in tokens)
    return "BANANA" if score >= decision_threshold else "NOT_BANANA"


def evaluate(
    samples: list[Sample],
    vocabulary_scores: dict[str, float],
    decision_threshold: float,
) -> dict[str, float]:
    if not samples:
        return {"precision": 0.0, "recall": 0.0, "f1": 0.0, "accuracy": 0.0, "support": 0}

    true_positive = 0
    false_positive = 0
    false_negative = 0
    true_negative = 0

    for sample in samples:
        prediction = classify(sample.tokens, vocabulary_scores, decision_threshold)
        if sample.label == "BANANA" and prediction == "BANANA":
            true_positive += 1
        elif sample.label == "NOT_BANANA" and prediction == "BANANA":
            false_positive += 1
        elif sample.label == "BANANA" and prediction == "NOT_BANANA":
            false_negative += 1
        else:
            true_negative += 1

    precision_denom = true_positive + false_positive
    recall_denom = true_positive + false_negative
    precision = true_positive / precision_denom if precision_denom else 0.0
    recall = true_positive / recall_denom if recall_denom else 0.0
    f1_denom = precision + recall
    f1 = (2 * precision * recall / f1_denom) if f1_denom else 0.0
    accuracy = (true_positive + true_negative) / len(samples)

    return {
        "precision": round(precision, 4),
        "recall": round(recall, 4),
        "f1": round(f1, 4),
        "accuracy": round(accuracy, 4),
        "support": len(samples),
        "true_positive": true_positive,
        "false_positive": false_positive,
        "false_negative": false_negative,
        "true_negative": true_negative,
    }


def render_c_header(vocabulary: list[dict[str, Any]]) -> str:
    lines = [
        "/* Auto-generated by scripts/train-not-banana-model.py.",
        " * Do not edit by hand. Re-run the trainer to refresh this file.",
        " * Generated from data/not-banana/corpus.json.",
        " */",
        "#ifndef BANANA_NOT_BANANA_GENERATED_VOCABULARY_H",
        "#define BANANA_NOT_BANANA_GENERATED_VOCABULARY_H",
        "",
        "static const char* const k_banana_signal_tokens_generated[] = {",
    ]

    for entry in vocabulary:
        token = entry["token"].replace("\\", "\\\\").replace("\"", "\\\"")
        lines.append(f"    \"{token}\",")

    lines.extend(
        [
            "};",
            "",
            "static const int k_banana_signal_token_count_generated =",
            "    (int)(sizeof(k_banana_signal_tokens_generated) / "
            "sizeof(k_banana_signal_tokens_generated[0]));",
            "",
            "#endif",
            "",
        ]
    )

    return "\n".join(lines)


def write_outputs(
    output_dir: Path,
    vocabulary: list[dict[str, Any]],
    metrics: dict[str, float],
    train_size: int,
    holdout_size: int,
    args: argparse.Namespace,
) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)

    vocabulary_payload = {
        "schema_version": 1,
        "model_id": "not-banana-vocabulary-v1",
        "trained_from": str(args.corpus),
        "train_size": train_size,
        "holdout_size": holdout_size,
        "hyperparameters": {
            "vocab_size": args.vocab_size,
            "min_banana_occurrences": args.min_banana_occurrences,
            "min_signal_score": args.min_signal_score,
            "holdout_fraction": args.holdout_fraction,
            "decision_threshold": args.decision_threshold,
            "split_seed": SPLIT_SEED,
        },
        "vocabulary": vocabulary,
        "metrics": metrics,
    }

    (output_dir / "vocabulary.json").write_text(
        json.dumps(vocabulary_payload, indent=2, sort_keys=False) + "\n",
        encoding="utf-8",
    )

    (output_dir / "metrics.json").write_text(
        json.dumps({"holdout": metrics}, indent=2, sort_keys=False) + "\n",
        encoding="utf-8",
    )

    (output_dir / "banana_signal_tokens.h").write_text(
        render_c_header(vocabulary), encoding="utf-8"
    )


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--corpus",
        type=Path,
        default=Path("data/not-banana/corpus.json"),
        help="Path to labeled corpus JSON.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=Path("artifacts/not-banana-model"),
        help="Output directory for trained artifacts.",
    )
    parser.add_argument(
        "--vocab-size",
        type=int,
        default=DEFAULT_VOCAB_SIZE,
        help="Maximum number of tokens kept in the learned vocabulary.",
    )
    parser.add_argument(
        "--min-banana-occurrences",
        type=int,
        default=DEFAULT_MIN_BANANA_OCCURRENCES,
        help="Minimum number of BANANA samples a token must appear in to be kept.",
    )
    parser.add_argument(
        "--min-signal-score",
        type=float,
        default=DEFAULT_MIN_SIGNAL_SCORE,
        help="Minimum BANANA-vs-NOT_BANANA signal score required to keep a token.",
    )
    parser.add_argument(
        "--holdout-fraction",
        type=float,
        default=DEFAULT_HOLDOUT_FRACTION,
        help="Fraction of samples reserved for evaluation.",
    )
    parser.add_argument(
        "--decision-threshold",
        type=float,
        default=0.5,
        help="Sum of signal_score weights at or above which a sample is "
             "predicted BANANA during evaluation.",
    )
    parser.add_argument(
        "--min-f1",
        type=float,
        default=0.0,
        help="Optional minimum F1 score on the hold-out split. Non-zero "
             "values cause the trainer to exit non-zero when the gate fails.",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)

    samples = load_corpus(args.corpus)
    train, holdout = deterministic_split(samples, args.holdout_fraction)

    vocabulary = learn_vocabulary(
        train,
        vocab_size=args.vocab_size,
        min_banana_occurrences=args.min_banana_occurrences,
        min_signal_score=args.min_signal_score,
    )
    vocabulary_scores = {entry["token"]: entry["signal_score"] for entry in vocabulary}

    metrics = evaluate(holdout, vocabulary_scores, args.decision_threshold)

    write_outputs(args.output, vocabulary, metrics, len(train), len(holdout), args)

    print(
        "trained vocabulary tokens="
        f"{len(vocabulary)} train={len(train)} holdout={len(holdout)} "
        f"f1={metrics['f1']} precision={metrics['precision']} "
        f"recall={metrics['recall']} accuracy={metrics['accuracy']}"
    )

    summary_path = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary_path:
        with open(summary_path, "a", encoding="utf-8") as handle:
            handle.write("## Not-Banana Trainer\n\n")
            handle.write(f"- vocabulary size: {len(vocabulary)}\n")
            handle.write(f"- train samples: {len(train)}\n")
            handle.write(f"- holdout samples: {len(holdout)}\n")
            handle.write(f"- precision: {metrics['precision']}\n")
            handle.write(f"- recall: {metrics['recall']}\n")
            handle.write(f"- f1: {metrics['f1']}\n")
            handle.write(f"- accuracy: {metrics['accuracy']}\n")

    if args.min_f1 > 0.0 and metrics["f1"] < args.min_f1:
        print(
            f"::error::F1 {metrics['f1']} below required gate {args.min_f1}",
            file=sys.stderr,
        )
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

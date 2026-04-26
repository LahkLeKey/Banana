#!/usr/bin/env python3
"""Deterministic ripeness trainer used by local and CI workflows."""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import random
import re
from collections import Counter
from dataclasses import dataclass
from pathlib import Path
from statistics import mean

TOKEN_PATTERN = re.compile(r"[a-z0-9']+")
PROFILE_DEFAULTS = {
    "ci": {"sessions": 4, "vocab_size": 24},
    "local": {"sessions": 12, "vocab_size": 32},
    "overnight": {"sessions": 32, "vocab_size": 48},
}
DETERMINISTIC_GENERATED_AT = "1970-01-01T00:00:00Z"
LABELS = ["unripe", "ripe", "overripe", "spoiled"]


@dataclass(frozen=True)
class Sample:
    sample_id: str
    text: str
    label: str


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Train ripeness multi-class vocabulary model.")
    parser.add_argument("--corpus", required=True, help="Path to corpus JSON file.")
    parser.add_argument("--output", required=True, help="Output directory for artifacts.")
    parser.add_argument("--vocab-size", type=int, default=24, help="Maximum number of learned tokens.")
    parser.add_argument(
        "--training-profile",
        choices=["ci", "local", "overnight", "auto"],
        default="ci",
        help="Resource profile for session defaults.",
    )
    parser.add_argument(
        "--session-mode",
        choices=["single", "incremental"],
        default="incremental",
        help="Single runs one session; incremental evaluates multiple sessions.",
    )
    parser.add_argument(
        "--max-sessions",
        type=int,
        default=0,
        help="Maximum evaluated sessions. 0 uses profile default.",
    )
    parser.add_argument("--min-signal-score", type=float, default=0.7, help="Minimum signal score gate.")
    parser.add_argument("--min-f1", type=float, default=0.7, help="Minimum macro-F1 hold-out gate.")
    parser.add_argument("--min-token-length", type=int, default=3, help="Minimum token length.")
    parser.add_argument(
        "--allow-numeric-tokens",
        action="store_true",
        help="Allow numeric-only tokens in learned vocabulary.",
    )
    return parser.parse_args()


def normalize_label(label: str) -> str:
    cleaned = label.strip().lower().replace("_", "-")
    if cleaned in LABELS:
        return cleaned
    raise ValueError(f"Unsupported label '{label}'. Expected one of: {', '.join(LABELS)}.")


def load_corpus(path: Path) -> tuple[list[Sample], list[str]]:
    if not path.exists():
        raise FileNotFoundError(f"Corpus file not found: {path}")

    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise ValueError("Corpus JSON must be an object.")

    raw_samples = payload.get("samples", [])
    if not isinstance(raw_samples, list):
        raise ValueError("Corpus JSON must include a 'samples' array.")

    split_ids: list[str] = []
    splits = payload.get("splits")
    if isinstance(splits, dict):
        eval_ids = splits.get("eval_ids", [])
        if isinstance(eval_ids, list):
            split_ids = [str(item) for item in eval_ids]

    samples: list[Sample] = []
    for index, item in enumerate(raw_samples):
        if not isinstance(item, dict):
            raise ValueError(f"Corpus entry #{index + 1} must be an object.")
        sample_id = str(item.get("id", "")).strip()
        text = str(item.get("text", "")).strip()
        if not sample_id:
            raise ValueError(f"Corpus entry #{index + 1} has empty id.")
        if not text:
            raise ValueError(f"Corpus entry #{index + 1} has empty text.")
        label = normalize_label(str(item.get("label", "")))
        samples.append(Sample(sample_id=sample_id, text=text, label=label))

    if len(samples) < 8:
        raise ValueError("Corpus must provide at least 8 samples to evaluate hold-out metrics.")

    per_label = Counter(sample.label for sample in samples)
    for label in LABELS:
        if per_label[label] < 8:
            raise ValueError(f"Corpus must provide at least 8 samples for label '{label}'.")

    return samples, split_ids


def resolve_profile(profile: str) -> str:
    if profile != "auto":
        return profile
    if os.environ.get("GITHUB_ACTIONS", "").strip().lower() == "true":
        return "ci"
    cpu_count = os.cpu_count() or 1
    if cpu_count >= 16:
        return "overnight"
    return "local"


def resolve_sessions(profile: str, session_mode: str, max_sessions: int) -> int:
    if session_mode == "single":
        return 1
    default_sessions = PROFILE_DEFAULTS[profile]["sessions"]
    return max_sessions if max_sessions > 0 else default_sessions


def resolve_vocab_size(profile: str, requested_vocab_size: int) -> int:
    if requested_vocab_size > 0:
        return requested_vocab_size
    return PROFILE_DEFAULTS[profile]["vocab_size"]


def tokenize(text: str, min_token_length: int, allow_numeric_tokens: bool) -> list[str]:
    tokens = []
    for raw in TOKEN_PATTERN.findall(text.lower()):
        token = raw.strip("'")
        if not token:
            continue
        if len(token) < min_token_length:
            continue
        if not allow_numeric_tokens and token.isdigit():
            continue
        tokens.append(token)
    return tokens


def split_train_holdout(samples: list[Sample], seed: int, eval_ids: set[str]) -> tuple[list[Sample], list[Sample]]:
    if eval_ids:
        holdout = [sample for sample in samples if sample.sample_id in eval_ids]
        if not holdout:
            raise ValueError("splits.eval_ids was provided but no matching sample ids were found.")
        train = [sample for sample in samples if sample.sample_id not in eval_ids]
        if not train:
            raise ValueError("splits.eval_ids consumed the full corpus; training set is empty.")
        present = {sample.label for sample in holdout}
        for label in LABELS:
            if label not in present:
                raise ValueError(f"Holdout set is missing label '{label}'.")
        return train, holdout

    grouped: dict[str, list[Sample]] = {label: [] for label in LABELS}
    for sample in samples:
        grouped[sample.label].append(sample)

    holdout: list[Sample] = []
    train: list[Sample] = []
    for offset, label in enumerate(LABELS):
        label_samples = grouped[label][:]
        random.Random(seed + offset).shuffle(label_samples)
        holdout_count = max(1, len(label_samples) // 4)
        holdout.extend(label_samples[:holdout_count])
        train.extend(label_samples[holdout_count:])

    return train, holdout


def learn_vocabulary(
    train_samples: list[Sample],
    vocab_size: int,
    min_token_length: int,
    allow_numeric_tokens: bool,
) -> tuple[list[dict], dict[str, dict[str, float]], float]:
    token_label_counts: dict[str, Counter[str]] = {}
    for sample in train_samples:
        unique_tokens = set(tokenize(sample.text, min_token_length, allow_numeric_tokens))
        for token in unique_tokens:
            counts = token_label_counts.setdefault(token, Counter())
            counts[sample.label] += 1

    scored = []
    for token in sorted(token_label_counts):
        counts = token_label_counts[token]
        support = sum(counts.values())
        if support == 0:
            continue
        probs = {label: counts[label] / support for label in LABELS}
        top_label = max(LABELS, key=lambda label: (probs[label], label))
        top_prob = probs[top_label]
        signal = top_prob - (1.0 / len(LABELS))
        weights = {label: round(probs[label], 6) for label in LABELS}
        scored.append(
            {
                "token": token,
                "support": support,
                "top_label": top_label,
                "top_probability": round(top_prob, 6),
                "signal": round(signal, 6),
                "weights": weights,
                "centered_weights": {
                    label: round(probs[label] - (1.0 / len(LABELS)), 6) for label in LABELS
                },
                "counts": {label: int(counts[label]) for label in LABELS},
            }
        )

    scored.sort(
        key=lambda row: (
            -float(row["signal"]),
            -int(row["support"]),
            str(row["top_label"]),
            str(row["token"]),
        )
    )

    # Keep a minimum per-label token quota so every class contributes signal.
    per_label_quota = max(1, min(6, vocab_size // len(LABELS)))
    selected: list[dict] = []
    selected_tokens: set[str] = set()
    for label in LABELS:
        label_rows = [row for row in scored if str(row["top_label"]) == label]
        for row in label_rows[:per_label_quota]:
            token = str(row["token"])
            if token in selected_tokens:
                continue
            selected.append(row)
            selected_tokens.add(token)

    if len(selected) < vocab_size:
        for row in scored:
            token = str(row["token"])
            if token in selected_tokens:
                continue
            selected.append(row)
            selected_tokens.add(token)
            if len(selected) >= vocab_size:
                break

    weights = {
        str(item["token"]): {label: float(item["centered_weights"][label]) for label in LABELS}
        for item in selected
    }
    signal_score = mean(float(item["signal"]) for item in selected) if selected else 0.0
    return selected, weights, round(signal_score, 6)


def predict_label(
    text: str,
    token_weights: dict[str, dict[str, float]],
    min_token_length: int,
    allow_numeric_tokens: bool,
) -> tuple[str, dict[str, float]]:
    unique_tokens = set(tokenize(text, min_token_length, allow_numeric_tokens))
    scores = {label: 0.0 for label in LABELS}
    for token in unique_tokens:
        weights = token_weights.get(token)
        if not weights:
            continue
        for label in LABELS:
            scores[label] += float(weights.get(label, 0.0))

    predicted = max(LABELS, key=lambda label: (scores[label], label))
    return predicted, scores


def safe_divide(numerator: float, denominator: float) -> float:
    if denominator == 0:
        return 0.0
    return numerator / denominator


def evaluate(
    holdout_samples: list[Sample],
    token_weights: dict[str, dict[str, float]],
    min_token_length: int,
    allow_numeric_tokens: bool,
) -> dict[str, object]:
    confusion = {actual: {pred: 0 for pred in LABELS} for actual in LABELS}
    for sample in holdout_samples:
        predicted, _ = predict_label(sample.text, token_weights, min_token_length, allow_numeric_tokens)
        confusion[sample.label][predicted] += 1

    per_label_metrics: dict[str, dict[str, float]] = {}
    f1_values = []
    total = 0
    correct = 0

    for label in LABELS:
        tp = confusion[label][label]
        fp = sum(confusion[actual][label] for actual in LABELS if actual != label)
        fn = sum(confusion[label][pred] for pred in LABELS if pred != label)
        precision = safe_divide(tp, tp + fp)
        recall = safe_divide(tp, tp + fn)
        f1 = safe_divide(2 * precision * recall, precision + recall)
        per_label_metrics[label] = {
            "precision": round(precision, 6),
            "recall": round(recall, 6),
            "f1": round(f1, 6),
        }
        f1_values.append(f1)

        total += sum(confusion[label].values())
        correct += tp

    macro_f1 = safe_divide(sum(f1_values), len(LABELS))
    accuracy = safe_divide(correct, total)

    return {
        "macro_f1": round(macro_f1, 6),
        "holdout_accuracy": round(accuracy, 6),
        "per_label": per_label_metrics,
        "confusion_matrix": confusion,
        "holdout_size": total,
    }


def write_label_header(path: Path) -> None:
    label_lines = "\n".join(f'    "{label}",' for label in LABELS)
    header = f"""/* Auto-generated by scripts/train-ripeness-model.py */
#ifndef BANANA_RIPENESS_LABELS_GENERATED_H
#define BANANA_RIPENESS_LABELS_GENERATED_H

static const char* const k_banana_ripeness_labels_generated[] = {{
{label_lines}
}};

static const unsigned long k_banana_ripeness_labels_generated_count =
    sizeof(k_banana_ripeness_labels_generated) / sizeof(k_banana_ripeness_labels_generated[0]);

#endif /* BANANA_RIPENESS_LABELS_GENERATED_H */
"""
    path.write_text(header, encoding="utf-8")


def stable_run_fingerprint(args: argparse.Namespace, corpus_path: Path, sample_count: int) -> str:
    payload = {
        "corpus": str(corpus_path),
        "sample_count": sample_count,
        "training_profile": args.training_profile,
        "session_mode": args.session_mode,
        "max_sessions": args.max_sessions,
        "vocab_size": args.vocab_size,
        "min_signal_score": args.min_signal_score,
        "min_f1": args.min_f1,
        "min_token_length": args.min_token_length,
        "allow_numeric_tokens": bool(args.allow_numeric_tokens),
    }
    encoded = json.dumps(payload, sort_keys=True, separators=(",", ":")).encode("utf-8")
    return hashlib.sha256(encoded).hexdigest()


def main() -> int:
    args = parse_args()

    corpus_path = Path(args.corpus)
    output_dir = Path(args.output)
    output_dir.mkdir(parents=True, exist_ok=True)

    samples, eval_ids_raw = load_corpus(corpus_path)
    eval_ids = set(eval_ids_raw)

    profile = resolve_profile(args.training_profile)
    sessions_to_run = resolve_sessions(profile, args.session_mode, args.max_sessions)
    vocab_size = resolve_vocab_size(profile, args.vocab_size)

    sessions: list[dict[str, object]] = []
    for session_index in range(sessions_to_run):
        seed = 20260426 + session_index
        train_samples, holdout_samples = split_train_holdout(samples, seed, eval_ids)
        vocabulary, token_weights, signal_score = learn_vocabulary(
            train_samples=train_samples,
            vocab_size=vocab_size,
            min_token_length=args.min_token_length,
            allow_numeric_tokens=args.allow_numeric_tokens,
        )
        metrics = evaluate(
            holdout_samples=holdout_samples,
            token_weights=token_weights,
            min_token_length=args.min_token_length,
            allow_numeric_tokens=args.allow_numeric_tokens,
        )
        session = {
            "session_index": session_index + 1,
            "seed": seed,
            "training_size": len(train_samples),
            "holdout_size": len(holdout_samples),
            "vocabulary_size": len(vocabulary),
            "signal_score": signal_score,
            "metrics": metrics,
            "vocabulary": vocabulary,
        }
        sessions.append(session)

    selected = max(
        sessions,
        key=lambda item: (
            float(item["metrics"]["macro_f1"]),
            float(item["signal_score"]),
            float(item["metrics"]["holdout_accuracy"]),
            -int(item["session_index"]),
        ),
    )

    selected_metrics = dict(selected["metrics"])
    selected_metrics.update(
        {
            "signal_score": round(float(selected["signal_score"]), 6),
            "min_signal_score": round(float(args.min_signal_score), 6),
            "min_f1": round(float(args.min_f1), 6),
            "meets_thresholds": (
                float(selected["signal_score"]) >= float(args.min_signal_score)
                and float(selected["metrics"]["macro_f1"]) >= float(args.min_f1)
            ),
            "training_profile": profile,
            "session_mode": args.session_mode,
            "evaluated_sessions": sessions_to_run,
            "selected_session": int(selected["session_index"]),
            "label_set": LABELS,
            "f1_type": "macro",
        }
    )

    run_fingerprint = stable_run_fingerprint(args, corpus_path, len(samples))

    vocabulary_payload = {
        "schema_version": 1,
        "generated_at_utc": DETERMINISTIC_GENERATED_AT,
        "run_fingerprint": run_fingerprint,
        "corpus_path": str(corpus_path),
        "corpus_size": len(samples),
        "training_profile": profile,
        "session_mode": args.session_mode,
        "vocab_size": vocab_size,
        "labels": LABELS,
        "vocabulary": list(selected["vocabulary"]),
        "metrics": selected_metrics,
    }

    metrics_payload = {
        "schema_version": 1,
        "generated_at_utc": DETERMINISTIC_GENERATED_AT,
        "run_fingerprint": run_fingerprint,
        "metrics": selected_metrics,
    }

    sessions_payload = {
        "schema_version": 1,
        "generated_at_utc": DETERMINISTIC_GENERATED_AT,
        "run_fingerprint": run_fingerprint,
        "sessions": sessions,
    }

    (output_dir / "vocabulary.json").write_text(json.dumps(vocabulary_payload, indent=2) + "\n", encoding="utf-8")
    (output_dir / "metrics.json").write_text(json.dumps(metrics_payload, indent=2) + "\n", encoding="utf-8")
    (output_dir / "sessions.json").write_text(json.dumps(sessions_payload, indent=2) + "\n", encoding="utf-8")
    write_label_header(output_dir / "banana_ripeness_labels.h")

    print(json.dumps({"output": str(output_dir), "metrics": selected_metrics, "run_fingerprint": run_fingerprint}, indent=2))

    if not selected_metrics["meets_thresholds"]:
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

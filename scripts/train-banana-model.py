#!/usr/bin/env python3
"""Deterministic banana trainer used by local and CI workflows."""

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
from typing import Iterable

from training_session_store import append_training_session_record, load_training_session_history, summarize_replay_drift

TOKEN_PATTERN = re.compile(r"[a-z0-9']+")
PROFILE_DEFAULTS = {
    "ci": {"sessions": 4, "vocab_size": 24},
    "local": {"sessions": 12, "vocab_size": 32},
    "overnight": {"sessions": 32, "vocab_size": 48},
}
DETERMINISTIC_GENERATED_AT = "1970-01-01T00:00:00Z"


@dataclass(frozen=True)
class Sample:
    text: str
    label: str


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Train banana vs not-banana vocabulary model.")
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
    parser.add_argument("--min-f1", type=float, default=0.7, help="Minimum hold-out F1 gate.")
    parser.add_argument("--min-token-length", type=int, default=3, help="Minimum token length.")
    parser.add_argument(
        "--allow-numeric-tokens",
        action="store_true",
        help="Allow numeric-only tokens in learned vocabulary.",
    )
    return parser.parse_args()


def normalize_label(label: str) -> str:
    cleaned = label.strip().lower().replace("_", "-")
    if cleaned == "banana":
        return "banana"
    if cleaned in {"not-banana", "notbanana", "not banana"}:
        return "not-banana"
    raise ValueError(f"Unsupported label '{label}'. Expected banana or not-banana.")


def load_corpus(path: Path) -> list[Sample]:
    if not path.exists():
        raise FileNotFoundError(f"Corpus file not found: {path}")

    payload = json.loads(path.read_text(encoding="utf-8"))
    if isinstance(payload, dict):
        raw_samples = payload.get("samples", [])
    elif isinstance(payload, list):
        raw_samples = payload
    else:
        raise ValueError("Corpus JSON must be an array or an object with a 'samples' array.")

    samples: list[Sample] = []
    for index, item in enumerate(raw_samples):
        if not isinstance(item, dict):
            raise ValueError(f"Corpus entry #{index + 1} must be an object.")
        text = str(item.get("text", "")).strip()
        if not text:
            raise ValueError(f"Corpus entry #{index + 1} has empty text.")
        label = normalize_label(str(item.get("label", "")))
        samples.append(Sample(text=text, label=label))

    if len(samples) < 6:
        raise ValueError("Corpus must provide at least 6 samples to evaluate hold-out metrics.")
    return samples


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


def split_train_holdout(samples: list[Sample], seed: int) -> tuple[list[Sample], list[Sample]]:
    ordered = samples[:]
    random.Random(seed).shuffle(ordered)
    holdout_size = max(1, len(ordered) // 4)
    holdout = ordered[:holdout_size]
    train = ordered[holdout_size:]
    if not train:
        train = holdout
        holdout = ordered[-1:]
    return train, holdout


def learn_vocabulary(
    train_samples: list[Sample],
    vocab_size: int,
    min_token_length: int,
    allow_numeric_tokens: bool,
) -> tuple[list[dict[str, float | int | str]], dict[str, float], float]:
    banana_counts: Counter[str] = Counter()
    not_banana_counts: Counter[str] = Counter()

    for sample in train_samples:
        unique_tokens = set(tokenize(sample.text, min_token_length, allow_numeric_tokens))
        for token in unique_tokens:
            if sample.label == "banana":
                banana_counts[token] += 1
            else:
                not_banana_counts[token] += 1

    scored: list[dict[str, float | int | str]] = []
    all_tokens = set(banana_counts) | set(not_banana_counts)
    for token in all_tokens:
        banana_hits = banana_counts[token]
        not_hits = not_banana_counts[token]
        support = banana_hits + not_hits
        if support == 0:
            continue
        signed_weight = (banana_hits - not_hits) / support
        signal = abs(signed_weight)
        scored.append(
            {
                "token": token,
                "banana_count": banana_hits,
                "not_banana_count": not_hits,
                "support": support,
                "weight": round(signed_weight, 6),
                "signal": round(signal, 6),
            }
        )

    scored.sort(key=lambda row: (-abs(float(row["weight"])), -int(row["support"]), str(row["token"])))
    selected = scored[:vocab_size]
    weights = {str(item["token"]): float(item["weight"]) for item in selected}
    signal_score = mean(abs(value) for value in weights.values()) if weights else 0.0
    return selected, weights, round(signal_score, 6)


def predict_label(
    text: str,
    weights: dict[str, float],
    min_token_length: int,
    allow_numeric_tokens: bool,
) -> tuple[str, float]:
    unique_tokens = set(tokenize(text, min_token_length, allow_numeric_tokens))
    score = sum(weights[token] for token in unique_tokens if token in weights)
    # Prefer "not-banana" for neutral score to reduce false positives in sparse-token cases.
    label = "banana" if score > 0 else "not-banana"
    return label, score


def safe_divide(numerator: float, denominator: float) -> float:
    if denominator == 0:
        return 0.0
    return numerator / denominator


def evaluate(
    holdout_samples: list[Sample],
    weights: dict[str, float],
    min_token_length: int,
    allow_numeric_tokens: bool,
) -> dict[str, float]:
    tp = fp = fn = tn = 0
    for sample in holdout_samples:
        predicted, _ = predict_label(sample.text, weights, min_token_length, allow_numeric_tokens)
        actual_not = sample.label == "not-banana"
        predicted_not = predicted == "not-banana"
        if predicted_not and actual_not:
            tp += 1
        elif predicted_not and not actual_not:
            fp += 1
        elif not predicted_not and actual_not:
            fn += 1
        else:
            tn += 1

    precision = safe_divide(tp, tp + fp)
    recall = safe_divide(tp, tp + fn)
    f1 = safe_divide(2 * precision * recall, precision + recall)
    accuracy = safe_divide(tp + tn, tp + fp + fn + tn)

    return {
        "holdout_precision": round(precision, 6),
        "holdout_recall": round(recall, 6),
        "holdout_f1": round(f1, 6),
        "holdout_accuracy": round(accuracy, 6),
        "tp": float(tp),
        "fp": float(fp),
        "fn": float(fn),
        "tn": float(tn),
    }


def write_header(path: Path, tokens: Iterable[str]) -> None:
    token_lines = "\n".join(f'    "{token}",' for token in tokens)
    header = f"""/* Auto-generated by scripts/train-banana-model.py */
#ifndef BANANA_SIGNAL_TOKENS_GENERATED_H
#define BANANA_SIGNAL_TOKENS_GENERATED_H

static const char* const k_banana_signal_tokens_generated[] = {{
{token_lines}
}};

static const unsigned long k_banana_signal_tokens_generated_count =
    sizeof(k_banana_signal_tokens_generated) / sizeof(k_banana_signal_tokens_generated[0]);

#endif /* BANANA_SIGNAL_TOKENS_GENERATED_H */
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

    samples = load_corpus(corpus_path)

    profile = resolve_profile(args.training_profile)
    sessions_to_run = resolve_sessions(profile, args.session_mode, args.max_sessions)
    vocab_size = resolve_vocab_size(profile, args.vocab_size)
    history_before = load_training_session_history(lane="banana")

    sessions: list[dict[str, object]] = []
    for session_index in range(sessions_to_run):
        seed = 20260426 + session_index
        train_samples, holdout_samples = split_train_holdout(samples, seed)
        vocabulary, weights, signal_score = learn_vocabulary(
            train_samples=train_samples,
            vocab_size=vocab_size,
            min_token_length=args.min_token_length,
            allow_numeric_tokens=args.allow_numeric_tokens,
        )
        metrics = evaluate(
            holdout_samples=holdout_samples,
            weights=weights,
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
            float(item["metrics"]["holdout_f1"]),
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
                and float(selected["metrics"]["holdout_f1"]) >= float(args.min_f1)
            ),
            "training_profile": profile,
            "session_mode": args.session_mode,
            "evaluated_sessions": sessions_to_run,
            "selected_session": int(selected["session_index"]),
        }
    )

    selected_vocabulary = list(selected["vocabulary"])
    selected_tokens = [str(entry["token"]) for entry in selected_vocabulary]

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
        "vocabulary": selected_vocabulary,
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
    write_header(output_dir / "banana_signal_tokens.h", selected_tokens)

    session_write = append_training_session_record(
        lane="banana",
        corpus_path=corpus_path,
        output_dir=output_dir,
        run_fingerprint=run_fingerprint,
        generated_at_utc=DETERMINISTIC_GENERATED_AT,
        training_profile=profile,
        session_mode=args.session_mode,
        max_sessions=args.max_sessions,
        selected_session=int(selected_metrics["selected_session"]),
        thresholds={
            "min_signal_score": float(args.min_signal_score),
            "min_f1": float(args.min_f1),
        },
        metrics=selected_metrics,
    )
    drift_summary = summarize_replay_drift(
        lane="banana",
        corpus_path=corpus_path,
        history_records=history_before,
        run_fingerprint=run_fingerprint,
        current_metrics=selected_metrics,
    )

    print(
        json.dumps(
            {
                "output": str(output_dir),
                "metrics": selected_metrics,
                "run_fingerprint": run_fingerprint,
                "persisted_session": session_write,
                "history_drift": drift_summary,
            },
            indent=2,
        )
    )

    if not selected_metrics["meets_thresholds"]:
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

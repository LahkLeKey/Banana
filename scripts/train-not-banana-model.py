#!/usr/bin/env python3
"""Train the polymorphic not-banana signal vocabulary from a labeled corpus.

The trainer learns which lowercase ASCII tokens most reliably indicate that a
polymorphic /not-banana/junk payload is actually a BANANA. It can run either a
single baseline session or an incremental resource-aware sweep (CI/local/
overnight profiles), and emits:

    - vocabulary.json: learned tokens with per-token signal scores and evaluation
        metrics for the selected session.
    - banana_signal_tokens.h: generated C header that can replace the inline
        vocabulary in src/native/core/domain/banana_not_banana.c.
    - metrics.json: precision/recall/F1/accuracy for the selected hold-out run.
    - sessions.json: per-session hyperparameters and hold-out metrics.

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
DEFAULT_MIN_TOKEN_LENGTH = 3
DEFAULT_ALLOW_NUMERIC_TOKENS = False
DEFAULT_TRAINING_PROFILE = "auto"
DEFAULT_SESSION_MODE = "incremental"

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


@dataclass(frozen=True)
class SessionConfig:
    vocab_size: int
    min_banana_occurrences: int
    min_signal_score: float
    holdout_fraction: float
    decision_threshold: float


def detect_cpu_count() -> int:
    return max(1, os.cpu_count() or 1)


def clamp_int(value: int, min_value: int, max_value: int) -> int:
    return max(min_value, min(max_value, value))


def unique_ints(values: Iterable[int], min_value: int, max_value: int) -> list[int]:
    seen: set[int] = set()
    output: list[int] = []

    for value in values:
        clamped = clamp_int(int(value), min_value, max_value)
        if clamped not in seen:
            seen.add(clamped)
            output.append(clamped)

    return output


def unique_floats(values: Iterable[float], min_value: float, max_value: float) -> list[float]:
    seen: set[float] = set()
    output: list[float] = []

    for value in values:
        clamped = round(max(min_value, min(max_value, float(value))), 4)
        if clamped not in seen:
            seen.add(clamped)
            output.append(clamped)

    return output


def resolve_training_profile(profile: str) -> str:
    if profile != "auto":
        return profile

    env_profile = os.environ.get("BANANA_TRAIN_PROFILE", "").strip().lower()
    if env_profile in {"ci", "local", "overnight"}:
        return env_profile

    if os.environ.get("GITHUB_ACTIONS", "").strip().lower() == "true":
        return "ci"

    cpu_count = detect_cpu_count()
    if cpu_count >= 12:
        return "overnight"

    return "local"


def resolve_max_sessions(profile: str, requested_max_sessions: int) -> int:
    if requested_max_sessions > 0:
        return requested_max_sessions

    cpu_count = detect_cpu_count()
    if profile == "ci":
        return min(8, max(3, cpu_count + 1))
    if profile == "overnight":
        return min(72, max(16, cpu_count * 6))

    return min(24, max(6, cpu_count * 3))


def build_session_plan(
    args: argparse.Namespace,
    training_profile: str,
    max_sessions: int,
    session_mode: str,
) -> list[SessionConfig]:
    baseline = SessionConfig(
        vocab_size=args.vocab_size,
        min_banana_occurrences=args.min_banana_occurrences,
        min_signal_score=args.min_signal_score,
        holdout_fraction=args.holdout_fraction,
        decision_threshold=args.decision_threshold,
    )

    if session_mode == "single":
        return [baseline]

    if training_profile == "ci":
        vocab_values = unique_ints(
            [baseline.vocab_size, baseline.vocab_size + 4, baseline.vocab_size + 8],
            min_value=4,
            max_value=256,
        )
        signal_values = unique_floats(
            [
                baseline.min_signal_score,
                baseline.min_signal_score - 0.05,
                baseline.min_signal_score + 0.05,
            ],
            min_value=0.0,
            max_value=1.0,
        )
        occurrence_values = unique_ints(
            [baseline.min_banana_occurrences, baseline.min_banana_occurrences + 1],
            min_value=1,
            max_value=32,
        )
        decision_values = unique_floats(
            [
                baseline.decision_threshold,
                baseline.decision_threshold - 0.05,
                baseline.decision_threshold + 0.05,
            ],
            min_value=0.01,
            max_value=0.99,
        )
        holdout_values = unique_floats([baseline.holdout_fraction], 0.05, 0.95)
    elif training_profile == "overnight":
        vocab_values = unique_ints(
            [
                baseline.vocab_size,
                baseline.vocab_size + 4,
                baseline.vocab_size + 8,
                baseline.vocab_size + 12,
                baseline.vocab_size + 16,
                baseline.vocab_size + 24,
            ],
            min_value=4,
            max_value=512,
        )
        signal_values = unique_floats(
            [
                baseline.min_signal_score,
                baseline.min_signal_score - 0.1,
                baseline.min_signal_score - 0.05,
                baseline.min_signal_score + 0.05,
                baseline.min_signal_score + 0.1,
            ],
            min_value=0.0,
            max_value=1.0,
        )
        occurrence_values = unique_ints(
            [
                baseline.min_banana_occurrences,
                baseline.min_banana_occurrences + 1,
                baseline.min_banana_occurrences + 2,
            ],
            min_value=1,
            max_value=32,
        )
        decision_values = unique_floats(
            [
                baseline.decision_threshold,
                baseline.decision_threshold - 0.1,
                baseline.decision_threshold - 0.05,
                baseline.decision_threshold + 0.05,
                baseline.decision_threshold + 0.1,
            ],
            min_value=0.01,
            max_value=0.99,
        )
        holdout_values = unique_floats(
            [baseline.holdout_fraction, 0.25, 0.3, 0.35],
            min_value=0.05,
            max_value=0.95,
        )
    else:
        vocab_values = unique_ints(
            [baseline.vocab_size, baseline.vocab_size + 4, baseline.vocab_size + 8, baseline.vocab_size + 12],
            min_value=4,
            max_value=512,
        )
        signal_values = unique_floats(
            [
                baseline.min_signal_score,
                baseline.min_signal_score - 0.05,
                baseline.min_signal_score + 0.05,
            ],
            min_value=0.0,
            max_value=1.0,
        )
        occurrence_values = unique_ints(
            [baseline.min_banana_occurrences, baseline.min_banana_occurrences + 1, baseline.min_banana_occurrences + 2],
            min_value=1,
            max_value=32,
        )
        decision_values = unique_floats(
            [
                baseline.decision_threshold,
                baseline.decision_threshold - 0.05,
                baseline.decision_threshold + 0.05,
            ],
            min_value=0.01,
            max_value=0.99,
        )
        holdout_values = unique_floats(
            [baseline.holdout_fraction, 0.25, 0.3],
            min_value=0.05,
            max_value=0.95,
        )

    sessions: list[SessionConfig] = []
    seen: set[tuple[int, int, float, float, float]] = set()

    def add_session(candidate: SessionConfig) -> None:
        if len(sessions) >= max_sessions:
            return

        key = (
            candidate.vocab_size,
            candidate.min_banana_occurrences,
            candidate.min_signal_score,
            candidate.holdout_fraction,
            candidate.decision_threshold,
        )
        if key in seen:
            return

        seen.add(key)
        sessions.append(candidate)

    add_session(baseline)

    for value in vocab_values:
        add_session(
            SessionConfig(
                vocab_size=value,
                min_banana_occurrences=baseline.min_banana_occurrences,
                min_signal_score=baseline.min_signal_score,
                holdout_fraction=baseline.holdout_fraction,
                decision_threshold=baseline.decision_threshold,
            )
        )
    for value in signal_values:
        add_session(
            SessionConfig(
                vocab_size=baseline.vocab_size,
                min_banana_occurrences=baseline.min_banana_occurrences,
                min_signal_score=value,
                holdout_fraction=baseline.holdout_fraction,
                decision_threshold=baseline.decision_threshold,
            )
        )
    for value in occurrence_values:
        add_session(
            SessionConfig(
                vocab_size=baseline.vocab_size,
                min_banana_occurrences=value,
                min_signal_score=baseline.min_signal_score,
                holdout_fraction=baseline.holdout_fraction,
                decision_threshold=baseline.decision_threshold,
            )
        )
    for value in decision_values:
        add_session(
            SessionConfig(
                vocab_size=baseline.vocab_size,
                min_banana_occurrences=baseline.min_banana_occurrences,
                min_signal_score=baseline.min_signal_score,
                holdout_fraction=baseline.holdout_fraction,
                decision_threshold=value,
            )
        )
    for value in holdout_values:
        add_session(
            SessionConfig(
                vocab_size=baseline.vocab_size,
                min_banana_occurrences=baseline.min_banana_occurrences,
                min_signal_score=baseline.min_signal_score,
                holdout_fraction=value,
                decision_threshold=baseline.decision_threshold,
            )
        )

    for vocab_size in vocab_values:
        for min_signal_score in signal_values:
            add_session(
                SessionConfig(
                    vocab_size=vocab_size,
                    min_banana_occurrences=baseline.min_banana_occurrences,
                    min_signal_score=min_signal_score,
                    holdout_fraction=baseline.holdout_fraction,
                    decision_threshold=baseline.decision_threshold,
                )
            )
    for decision_threshold in decision_values:
        for min_signal_score in signal_values:
            add_session(
                SessionConfig(
                    vocab_size=baseline.vocab_size,
                    min_banana_occurrences=baseline.min_banana_occurrences,
                    min_signal_score=min_signal_score,
                    holdout_fraction=baseline.holdout_fraction,
                    decision_threshold=decision_threshold,
                )
            )
    for holdout_fraction in holdout_values:
        for vocab_size in vocab_values:
            add_session(
                SessionConfig(
                    vocab_size=vocab_size,
                    min_banana_occurrences=baseline.min_banana_occurrences,
                    min_signal_score=baseline.min_signal_score,
                    holdout_fraction=holdout_fraction,
                    decision_threshold=baseline.decision_threshold,
                )
            )

    return sessions[:max_sessions]


def session_score_key(session_result: dict[str, Any]) -> tuple[float, float, float, float, int]:
    metrics = session_result["metrics"]
    params = session_result["hyperparameters"]
    return (
        float(metrics["f1"]),
        float(metrics["accuracy"]),
        float(metrics["precision"]),
        float(metrics["recall"]),
        -int(params["vocab_size"]),
    )


def tokenise_string(value: str) -> Iterable[str]:
    return TOKEN_REGEX.findall(value.lower())


def token_has_ascii_alpha(token: str) -> bool:
    for char in token:
        if "a" <= char <= "z":
            return True
    return False


def token_passes_quality_filters(
    token: str,
    min_token_length: int,
    allow_numeric_tokens: bool,
) -> bool:
    if len(token) < min_token_length:
        return False

    if not allow_numeric_tokens and not token_has_ascii_alpha(token):
        return False

    return True


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
    min_token_length: int,
    allow_numeric_tokens: bool,
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
        if not token_passes_quality_filters(token, min_token_length, allow_numeric_tokens):
            continue

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
    training_profile: str,
    session_mode: str,
    session_results: list[dict[str, Any]],
    selected_session: dict[str, Any],
    resource_context: dict[str, Any],
) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)

    selected_hyperparameters = selected_session["hyperparameters"]

    vocabulary_payload = {
        "schema_version": 1,
        "model_id": "not-banana-vocabulary-v1",
        "trained_from": str(args.corpus),
        "training_profile": training_profile,
        "session_mode": session_mode,
        "resource_context": resource_context,
        "session_count": len(session_results),
        "selected_session_index": selected_session["session_index"],
        "train_size": train_size,
        "holdout_size": holdout_size,
        "hyperparameters": {
            "vocab_size": selected_hyperparameters["vocab_size"],
            "min_banana_occurrences": selected_hyperparameters[
                "min_banana_occurrences"
            ],
            "min_signal_score": selected_hyperparameters["min_signal_score"],
            "min_token_length": selected_hyperparameters["min_token_length"],
            "allow_numeric_tokens": selected_hyperparameters["allow_numeric_tokens"],
            "holdout_fraction": selected_hyperparameters["holdout_fraction"],
            "decision_threshold": selected_hyperparameters["decision_threshold"],
            "split_seed": SPLIT_SEED,
        },
        "requested_hyperparameters": {
            "vocab_size": args.vocab_size,
            "min_banana_occurrences": args.min_banana_occurrences,
            "min_signal_score": args.min_signal_score,
            "min_token_length": args.min_token_length,
            "allow_numeric_tokens": args.allow_numeric_tokens,
            "holdout_fraction": args.holdout_fraction,
            "decision_threshold": args.decision_threshold,
            "training_profile": args.training_profile,
            "session_mode": args.session_mode,
            "max_sessions": args.max_sessions,
            "split_seed": SPLIT_SEED,
        },
        "vocabulary": vocabulary,
        "metrics": metrics,
        "sessions": session_results,
    }

    (output_dir / "vocabulary.json").write_text(
        json.dumps(vocabulary_payload, indent=2, sort_keys=False) + "\n",
        encoding="utf-8",
    )

    (output_dir / "metrics.json").write_text(
        json.dumps(
            {
                "training_profile": training_profile,
                "session_mode": session_mode,
                "resource_context": resource_context,
                "session_count": len(session_results),
                "selected_session_index": selected_session["session_index"],
                "holdout": metrics,
                "sessions": session_results,
            },
            indent=2,
            sort_keys=False,
        )
        + "\n",
        encoding="utf-8",
    )

    (output_dir / "sessions.json").write_text(
        json.dumps(
            {
                "training_profile": training_profile,
                "session_mode": session_mode,
                "resource_context": resource_context,
                "session_count": len(session_results),
                "selected_session_index": selected_session["session_index"],
                "sessions": session_results,
            },
            indent=2,
            sort_keys=False,
        )
        + "\n",
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
        "--min-token-length",
        type=int,
        default=DEFAULT_MIN_TOKEN_LENGTH,
        help="Minimum token length required before a token is eligible for the vocabulary.",
    )
    parser.add_argument(
        "--allow-numeric-tokens",
        action=argparse.BooleanOptionalAction,
        default=DEFAULT_ALLOW_NUMERIC_TOKENS,
        help="Allow tokens that contain no alphabetic characters (for example pure numbers).",
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
    parser.add_argument(
        "--training-profile",
        choices=["auto", "ci", "local", "overnight"],
        default=DEFAULT_TRAINING_PROFILE,
        help="Resource profile used to size the incremental training sweep.",
    )
    parser.add_argument(
        "--session-mode",
        choices=["single", "incremental"],
        default=DEFAULT_SESSION_MODE,
        help="Run a single baseline session or an incremental parameter sweep.",
    )
    parser.add_argument(
        "--max-sessions",
        type=int,
        default=0,
        help="Upper bound on sessions to evaluate. 0 selects a profile-based default.",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)

    if args.max_sessions < 0:
        raise ValueError("max-sessions must be zero or greater.")
    if args.min_token_length < 1:
        raise ValueError("min-token-length must be at least 1.")

    samples = load_corpus(args.corpus)
    training_profile = resolve_training_profile(args.training_profile)
    max_sessions = resolve_max_sessions(training_profile, args.max_sessions)
    resource_context = {
        "cpu_count": detect_cpu_count(),
        "github_actions": os.environ.get("GITHUB_ACTIONS", "").strip().lower()
        == "true",
    }
    session_plan = build_session_plan(
        args=args,
        training_profile=training_profile,
        max_sessions=max_sessions,
        session_mode=args.session_mode,
    )

    selected_session_result: dict[str, Any] | None = None
    selected_vocabulary: list[dict[str, Any]] = []
    session_results: list[dict[str, Any]] = []

    for session_index, session in enumerate(session_plan, start=1):
        train, holdout = deterministic_split(samples, session.holdout_fraction)
        vocabulary = learn_vocabulary(
            train,
            vocab_size=session.vocab_size,
            min_banana_occurrences=session.min_banana_occurrences,
            min_signal_score=session.min_signal_score,
            min_token_length=args.min_token_length,
            allow_numeric_tokens=args.allow_numeric_tokens,
        )
        vocabulary_scores = {
            entry["token"]: entry["signal_score"] for entry in vocabulary
        }
        metrics = evaluate(holdout, vocabulary_scores, session.decision_threshold)

        session_result = {
            "session_index": session_index,
            "train_size": len(train),
            "holdout_size": len(holdout),
            "token_count": len(vocabulary),
            "hyperparameters": {
                "vocab_size": session.vocab_size,
                "min_banana_occurrences": session.min_banana_occurrences,
                "min_signal_score": session.min_signal_score,
                "min_token_length": args.min_token_length,
                "allow_numeric_tokens": args.allow_numeric_tokens,
                "holdout_fraction": session.holdout_fraction,
                "decision_threshold": session.decision_threshold,
                "split_seed": SPLIT_SEED,
            },
            "metrics": metrics,
        }
        session_results.append(session_result)

        if selected_session_result is None:
            selected_session_result = session_result
            selected_vocabulary = vocabulary
            continue

        if session_score_key(session_result) > session_score_key(selected_session_result):
            selected_session_result = session_result
            selected_vocabulary = vocabulary

    if selected_session_result is None:
        raise RuntimeError("Trainer did not produce any training sessions.")

    metrics = selected_session_result["metrics"]
    train_size = int(selected_session_result["train_size"])
    holdout_size = int(selected_session_result["holdout_size"])

    write_outputs(
        output_dir=args.output,
        vocabulary=selected_vocabulary,
        metrics=metrics,
        train_size=train_size,
        holdout_size=holdout_size,
        args=args,
        training_profile=training_profile,
        session_mode=args.session_mode,
        session_results=session_results,
        selected_session=selected_session_result,
        resource_context=resource_context,
    )

    print(
        "trained vocabulary tokens="
        f"{len(selected_vocabulary)} train={train_size} holdout={holdout_size} "
        f"profile={training_profile} sessions={len(session_results)} "
        f"selected_session={selected_session_result['session_index']} "
        f"f1={metrics['f1']} precision={metrics['precision']} "
        f"recall={metrics['recall']} accuracy={metrics['accuracy']}"
    )

    summary_path = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary_path:
        with open(summary_path, "a", encoding="utf-8") as handle:
            handle.write("## Not-Banana Trainer\n\n")
            handle.write(f"- profile: {training_profile}\n")
            handle.write(f"- session mode: {args.session_mode}\n")
            handle.write(f"- sessions evaluated: {len(session_results)}\n")
            handle.write(
                f"- selected session: {selected_session_result['session_index']}\n"
            )
            handle.write(f"- vocabulary size: {len(selected_vocabulary)}\n")
            handle.write(f"- train samples: {train_size}\n")
            handle.write(f"- holdout samples: {holdout_size}\n")
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

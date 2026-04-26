#!/usr/bin/env python3
"""Validate not-banana training artifacts consumed by the TypeScript runtime."""

from __future__ import annotations

import argparse
import json
import re
from pathlib import Path
from typing import Any

EXPECTED_SCHEMA_VERSION = 1
HEADER_PATTERN = re.compile(
    r"k_banana_signal_tokens_generated\[\]\s*=\s*\{(.*?)\};",
    re.DOTALL,
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Validate not-banana artifact schema/version and runtime contract."
    )
    parser.add_argument(
        "--model-dir",
        required=True,
        help="Directory containing vocabulary.json, metrics.json, sessions.json, and banana_signal_tokens.h",
    )
    parser.add_argument(
        "--header",
        default="banana_signal_tokens.h",
        help="Header file name relative to --model-dir.",
    )
    return parser.parse_args()


def load_json(path: Path, label: str, errors: list[str]) -> dict[str, Any] | None:
    if not path.exists():
        errors.append(f"{label}_missing:{path}")
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except Exception as exc:  # pragma: no cover - defensive parsing
        errors.append(f"{label}_invalid_json:{path}:{exc}")
        return None

    if not isinstance(payload, dict):
        errors.append(f"{label}_not_object:{path}")
        return None
    return payload


def validate_schema_version(payload: dict[str, Any], label: str, errors: list[str]) -> None:
    version = payload.get("schema_version")
    if version != EXPECTED_SCHEMA_VERSION:
        errors.append(
            f"{label}_unsupported_schema_version:expected={EXPECTED_SCHEMA_VERSION},actual={version}"
        )


def require_dict(payload: dict[str, Any], key: str, label: str, errors: list[str]) -> dict[str, Any] | None:
    value = payload.get(key)
    if not isinstance(value, dict):
        errors.append(f"{label}_{key}_missing_or_not_object")
        return None
    return value


def require_list(payload: dict[str, Any], key: str, label: str, errors: list[str]) -> list[Any] | None:
    value = payload.get(key)
    if not isinstance(value, list):
        errors.append(f"{label}_{key}_missing_or_not_array")
        return None
    return value


def validate_vocabulary(vocabulary_payload: dict[str, Any], errors: list[str]) -> set[str]:
    validate_schema_version(vocabulary_payload, "vocabulary", errors)

    generated = vocabulary_payload.get("generated_at_utc")
    if not isinstance(generated, str) or not generated:
        errors.append("vocabulary_generated_at_utc_missing")

    vocab_size = vocabulary_payload.get("vocab_size")
    if not isinstance(vocab_size, int) or vocab_size <= 0:
        errors.append(f"vocabulary_vocab_size_invalid:{vocab_size}")

    entries = require_list(vocabulary_payload, "vocabulary", "vocabulary", errors)
    if entries is None:
        return set()

    tokens: list[str] = []
    for index, entry in enumerate(entries):
        if not isinstance(entry, dict):
            errors.append(f"vocabulary_entry_not_object:index={index}")
            continue

        token = entry.get("token")
        weight = entry.get("weight")
        signal = entry.get("signal")

        if not isinstance(token, str) or not token.strip():
            errors.append(f"vocabulary_token_invalid:index={index}")
            continue

        if not isinstance(weight, (int, float)):
            errors.append(f"vocabulary_weight_invalid:index={index}")
            continue

        if not isinstance(signal, (int, float)):
            errors.append(f"vocabulary_signal_invalid:index={index}")
            continue

        tokens.append(token)

    if isinstance(vocab_size, int) and vocab_size != len(entries):
        errors.append(
            f"vocabulary_vocab_size_mismatch:declared={vocab_size},actual={len(entries)}"
        )

    unique_tokens = set(tokens)
    if len(unique_tokens) != len(tokens):
        errors.append("vocabulary_duplicate_tokens")

    if not unique_tokens:
        errors.append("vocabulary_empty")

    metrics = require_dict(vocabulary_payload, "metrics", "vocabulary", errors)
    if metrics is not None:
        for key in ("min_signal_score", "min_f1", "holdout_f1", "holdout_accuracy"):
            value = metrics.get(key)
            if not isinstance(value, (int, float)):
                errors.append(f"vocabulary_metrics_{key}_invalid")

    return unique_tokens


def validate_metrics(
    metrics_payload: dict[str, Any],
    vocabulary_metrics: dict[str, Any] | None,
    errors: list[str],
) -> None:
    validate_schema_version(metrics_payload, "metrics", errors)

    metrics = require_dict(metrics_payload, "metrics", "metrics", errors)
    if metrics is None:
        return

    for key in ("min_signal_score", "min_f1", "holdout_f1", "holdout_accuracy", "meets_thresholds"):
        if key not in metrics:
            errors.append(f"metrics_missing_{key}")

    if vocabulary_metrics is not None:
        for key in (
            "min_signal_score",
            "min_f1",
            "holdout_f1",
            "holdout_accuracy",
            "selected_session",
            "training_profile",
            "session_mode",
        ):
            if key in metrics and key in vocabulary_metrics and metrics[key] != vocabulary_metrics[key]:
                errors.append(f"metrics_mismatch_{key}")


def validate_sessions(sessions_payload: dict[str, Any], errors: list[str]) -> None:
    validate_schema_version(sessions_payload, "sessions", errors)
    sessions = require_list(sessions_payload, "sessions", "sessions", errors)
    if sessions is None:
        return

    if not sessions:
        errors.append("sessions_empty")
        return

    for index, session in enumerate(sessions):
        if not isinstance(session, dict):
            errors.append(f"sessions_entry_not_object:index={index}")
            continue

        if not isinstance(session.get("session_index"), int):
            errors.append(f"sessions_session_index_invalid:index={index}")

        metrics = session.get("metrics")
        if not isinstance(metrics, dict):
            errors.append(f"sessions_metrics_missing:index={index}")


def validate_header(path: Path, expected_tokens: set[str], errors: list[str]) -> None:
    if not path.exists():
        errors.append(f"header_missing:{path}")
        return

    source = path.read_text(encoding="utf-8")
    match = HEADER_PATTERN.search(source)
    if not match:
        errors.append(f"header_token_array_missing:{path}")
        return

    tokens = set(re.findall(r'"([^"]+)"', match.group(1)))
    if tokens != expected_tokens:
        missing = sorted(expected_tokens - tokens)
        extra = sorted(tokens - expected_tokens)
        if missing:
            errors.append(f"header_tokens_missing:{','.join(missing)}")
        if extra:
            errors.append(f"header_tokens_extra:{','.join(extra)}")


def main() -> int:
    args = parse_args()
    model_dir = Path(args.model_dir)
    errors: list[str] = []

    vocabulary_path = model_dir / "vocabulary.json"
    metrics_path = model_dir / "metrics.json"
    sessions_path = model_dir / "sessions.json"
    header_path = model_dir / args.header

    vocabulary_payload = load_json(vocabulary_path, "vocabulary", errors)
    metrics_payload = load_json(metrics_path, "metrics", errors)
    sessions_payload = load_json(sessions_path, "sessions", errors)

    expected_tokens: set[str] = set()

    if vocabulary_payload is not None:
        expected_tokens = validate_vocabulary(vocabulary_payload, errors)

    vocabulary_metrics = None
    if vocabulary_payload is not None:
        metrics = vocabulary_payload.get("metrics")
        if isinstance(metrics, dict):
            vocabulary_metrics = metrics

    if metrics_payload is not None:
        validate_metrics(metrics_payload, vocabulary_metrics, errors)

    if sessions_payload is not None:
        validate_sessions(sessions_payload, errors)

    if expected_tokens:
        validate_header(header_path, expected_tokens, errors)

    if errors:
        print(
            json.dumps(
                {
                    "status": "failed",
                    "schema_version": EXPECTED_SCHEMA_VERSION,
                    "model_dir": str(model_dir),
                    "errors": errors,
                },
                indent=2,
            )
        )
        return 2

    summary = {
        "status": "ok",
        "schema_version": EXPECTED_SCHEMA_VERSION,
        "model_dir": str(model_dir),
        "token_count": len(expected_tokens),
        "files": {
            "vocabulary": str(vocabulary_path),
            "metrics": str(metrics_path),
            "sessions": str(sessions_path),
            "header": str(header_path),
        },
    }
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

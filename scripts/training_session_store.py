#!/usr/bin/env python3
"""Shared helpers for persisting deterministic training session records."""

from __future__ import annotations

import hashlib
import json
from pathlib import Path
from typing import Any


def _sha256_bytes(raw: bytes) -> str:
    return hashlib.sha256(raw).hexdigest()


def compute_corpus_id(corpus_path: Path) -> str:
    payload = corpus_path.read_bytes()
    digest = _sha256_bytes(payload)
    return f"{corpus_path.as_posix()}:{digest}"


def _relative_or_posix(path: Path, repo_root: Path) -> str:
    try:
        return path.resolve().relative_to(repo_root.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def _next_sequence(history_file: Path) -> int:
    if not history_file.exists():
        return 1
    count = 0
    for raw_line in history_file.read_text(encoding="utf-8").splitlines():
        if raw_line.strip():
            count += 1
    return count + 1


def append_training_session_record(
    *,
    lane: str,
    corpus_path: Path,
    output_dir: Path,
    run_fingerprint: str,
    generated_at_utc: str,
    training_profile: str,
    session_mode: str,
    max_sessions: int,
    selected_session: int,
    thresholds: dict[str, float],
    metrics: dict[str, Any],
) -> dict[str, str]:
    repo_root = Path.cwd()
    history_dir = repo_root / "data" / lane / "sessions"
    history_dir.mkdir(parents=True, exist_ok=True)
    history_file = history_dir / "history.jsonl"

    sequence = _next_sequence(history_file)
    session_id = f"{lane}-{run_fingerprint[:12]}-{sequence:06d}"

    artifact_paths = {
        "output_dir": _relative_or_posix(output_dir, repo_root),
        "vocabulary": _relative_or_posix(output_dir / "vocabulary.json", repo_root),
        "metrics": _relative_or_posix(output_dir / "metrics.json", repo_root),
        "sessions": _relative_or_posix(output_dir / "sessions.json", repo_root),
    }
    header_path = output_dir / "banana_signal_tokens.h"
    if not header_path.exists():
        header_path = output_dir / "banana_ripeness_labels.h"
    artifact_paths["header"] = _relative_or_posix(header_path, repo_root)

    record = {
        "schema_version": 1,
        "session_id": session_id,
        "lane": lane,
        "corpus_id": compute_corpus_id(corpus_path),
        "training_profile": training_profile,
        "session_mode": session_mode,
        "max_sessions": int(max_sessions),
        "selected_session": int(selected_session),
        "thresholds": {
            "min_signal_score": float(thresholds["min_signal_score"]),
            "min_f1": float(thresholds["min_f1"]),
        },
        "metrics": metrics,
        "run_fingerprint": run_fingerprint,
        "generated_at_utc": generated_at_utc,
        "artifact_paths": artifact_paths,
    }

    with history_file.open("a", encoding="utf-8") as handle:
        handle.write(json.dumps(record, sort_keys=True) + "\n")

    return {
        "session_id": session_id,
        "history_file": _relative_or_posix(history_file, repo_root),
    }


def load_training_session_history(*, lane: str) -> list[dict[str, Any]]:
    history_file = Path.cwd() / "data" / lane / "sessions" / "history.jsonl"
    if not history_file.exists():
        return []
    records: list[dict[str, Any]] = []
    for raw_line in history_file.read_text(encoding="utf-8").splitlines():
        if not raw_line.strip():
            continue
        payload = json.loads(raw_line)
        if isinstance(payload, dict):
            records.append(payload)
    return records


def summarize_replay_drift(
    *,
    lane: str,
    corpus_path: Path,
    history_records: list[dict[str, Any]],
    run_fingerprint: str,
    current_metrics: dict[str, Any],
) -> dict[str, Any]:
    if not history_records:
        return {"status": "no-history", "lane": lane}

    corpus_id = compute_corpus_id(corpus_path)
    previous: dict[str, Any] | None = None
    for record in reversed(history_records):
        if str(record.get("corpus_id", "")) == corpus_id:
            previous = record
            break

    if previous is None:
        return {
            "status": "no-matching-corpus",
            "lane": lane,
            "history_count": len(history_records),
        }

    previous_metrics = previous.get("metrics", {})
    deltas: dict[str, float] = {}
    if isinstance(previous_metrics, dict):
        for key, value in current_metrics.items():
            previous_value = previous_metrics.get(key)
            if isinstance(value, (int, float)) and isinstance(
                previous_value, (int, float)
            ):
                deltas[key] = round(float(value) - float(previous_value), 6)

    return {
        "status": "compared",
        "lane": lane,
        "history_count": len(history_records),
        "previous_session_id": previous.get("session_id"),
        "previous_run_fingerprint": previous.get("run_fingerprint"),
        "current_run_fingerprint": run_fingerprint,
        "metric_deltas": deltas,
    }

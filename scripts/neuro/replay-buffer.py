#!/usr/bin/env python3
"""Episodic replay buffer (reservoir-sampled, JSONL on disk).

Spec refs: FR-001, FR-011, SC-005 (feature 050).

Operations:
    append     Append one or more session records to the buffer (reservoir cap).
    sample     Deterministically sample N records from the buffer.
    prune      Drop records older than --max-age-days (default keeps all).

Records are JSON objects with at minimum a ``session_id`` and ``observed_at_utc``
field. Other fields are passed through opaquely.
"""

from __future__ import annotations

import argparse
import json
import random
import sys
from pathlib import Path
from typing import Iterable

DEFAULT_CAP = 2048


def load_buffer(path: Path) -> list[dict]:
    if not path.exists():
        return []
    records: list[dict] = []
    for line_no, raw in enumerate(path.read_text(encoding="utf-8").splitlines(), start=1):
        line = raw.strip()
        if not line:
            continue
        try:
            obj = json.loads(line)
        except json.JSONDecodeError as exc:
            raise ValueError(f"{path}:{line_no} invalid JSON: {exc}") from exc
        if not isinstance(obj, dict):
            raise ValueError(f"{path}:{line_no} expected JSON object")
        records.append(obj)
    return records


def write_buffer(path: Path, records: Iterable[dict]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    serialized = [
        json.dumps(rec, sort_keys=True, separators=(",", ":")) for rec in records
    ]
    text = "\n".join(serialized) + ("\n" if serialized else "")
    # Force LF endings on all platforms so artifact diffs stay reviewable.
    path.write_bytes(text.encode("utf-8"))


def reservoir_append(
    existing: list[dict],
    incoming: list[dict],
    cap: int,
    seed: int,
) -> list[dict]:
    """Algorithm R reservoir sampling.

    Preserves deterministic ordering by sorting the final retained set by
    ``session_id`` (string compare) so that disk writes are reviewable diffs.
    """
    if cap <= 0:
        return []
    rng = random.Random(seed)
    pool: list[dict] = list(existing)
    seen = len(pool)
    for rec in incoming:
        seen += 1
        if len(pool) < cap:
            pool.append(rec)
            continue
        # Random index in [0, seen). If < cap, evict.
        idx = rng.randrange(seen)
        if idx < cap:
            pool[idx] = rec
    pool.sort(key=lambda r: str(r.get("session_id", "")))
    return pool


def deterministic_sample(records: list[dict], n: int, seed: int) -> list[dict]:
    if n <= 0 or not records:
        return []
    if n >= len(records):
        return list(records)
    rng = random.Random(seed)
    indices = sorted(rng.sample(range(len(records)), n))
    return [records[i] for i in indices]


def prune_by_age(records: list[dict], max_age_days: int, now_iso: str) -> list[dict]:
    if max_age_days <= 0:
        return list(records)
    # ISO8601 lexicographic compare is valid for UTC ``Z`` timestamps.
    from datetime import datetime, timedelta

    def _parse(ts: str) -> datetime:
        return datetime.fromisoformat(ts.replace("Z", "+00:00"))

    cutoff = _parse(now_iso) - timedelta(days=max_age_days)
    kept: list[dict] = []
    for rec in records:
        observed = rec.get("observed_at_utc")
        if not isinstance(observed, str):
            kept.append(rec)
            continue
        try:
            if _parse(observed) >= cutoff:
                kept.append(rec)
        except ValueError:
            kept.append(rec)
    return kept


def _read_incoming(stdin_payload: str | None, file_path: Path | None) -> list[dict]:
    if stdin_payload is not None:
        text = stdin_payload
    elif file_path is not None:
        text = file_path.read_text(encoding="utf-8")
    else:
        return []
    text = text.strip()
    if not text:
        return []
    try:
        payload = json.loads(text)
    except json.JSONDecodeError:
        # Treat as JSONL fallback.
        out: list[dict] = []
        for line in text.splitlines():
            line = line.strip()
            if line:
                out.append(json.loads(line))
        return out
    if isinstance(payload, dict):
        return [payload]
    if isinstance(payload, list):
        return [p for p in payload if isinstance(p, dict)]
    raise ValueError("incoming records must be an object, list, or JSONL")


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Banana neuro replay buffer.")
    parser.add_argument("--buffer", required=True, help="Path to JSONL buffer file.")
    parser.add_argument(
        "--op",
        choices=["append", "sample", "prune"],
        required=True,
    )
    parser.add_argument("--cap", type=int, default=DEFAULT_CAP)
    parser.add_argument("--seed", type=int, default=20260501)
    parser.add_argument("--n", type=int, default=0, help="Sample size for op=sample.")
    parser.add_argument("--max-age-days", type=int, default=0)
    parser.add_argument("--now", default="1970-01-01T00:00:00Z")
    parser.add_argument(
        "--records-file",
        type=Path,
        default=None,
        help="JSON or JSONL file containing records to append. If omitted, stdin is read.",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    buffer_path = Path(args.buffer)
    existing = load_buffer(buffer_path)

    if args.op == "append":
        stdin_payload = None
        if args.records_file is None and not sys.stdin.isatty():
            stdin_payload = sys.stdin.read()
        incoming = _read_incoming(stdin_payload, args.records_file)
        retained = reservoir_append(existing, incoming, args.cap, args.seed)
        write_buffer(buffer_path, retained)
        print(json.dumps({"op": "append", "size": len(retained), "cap": args.cap}))
        return 0

    if args.op == "sample":
        sampled = deterministic_sample(existing, args.n, args.seed)
        sys.stdout.write(
            "\n".join(json.dumps(r, sort_keys=True, separators=(",", ":")) for r in sampled)
        )
        if sampled:
            sys.stdout.write("\n")
        return 0

    # prune
    kept = prune_by_age(existing, args.max_age_days, args.now)
    write_buffer(buffer_path, kept)
    print(json.dumps({"op": "prune", "size": len(kept)}))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

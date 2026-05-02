#!/usr/bin/env python3
"""Sleep-style consolidation pass: writes a Fisher-information snapshot.

Spec refs: FR-002, US2 acceptance #1 (feature 050).

Given a model artifact (``vocabulary.json``) and an optional replay buffer,
emits ``fisher.json`` (per-token diagonal Fisher proxy) and a
``consolidation-report.json`` summary. Cold-start safe: missing inputs produce
empty (but well-formed) outputs rather than failing.

The Fisher proxy here uses ``weight^2 * support_norm`` per token, which is the
diagonal of the empirical Fisher for a logistic decision rule when each token
contributes additively. This is intentionally lightweight; it gives us the
EWC penalty surface (``F_i * (theta_i - theta_i*)^2``) without any heavy ML
dependency.
"""

from __future__ import annotations

import argparse
import json
from datetime import datetime, timezone
from pathlib import Path

DETERMINISTIC_GENERATED_AT = "1970-01-01T00:00:00Z"


def utc_date(now_iso: str | None = None) -> str:
    if now_iso:
        return now_iso[:10]
    return datetime.now(timezone.utc).strftime("%Y-%m-%d")


def load_vocabulary(path: Path) -> list[dict]:
    if not path.exists():
        return []
    payload = json.loads(path.read_text(encoding="utf-8"))
    return list(payload.get("vocabulary", []))


def load_buffer_size(path: Path) -> int:
    if not path.exists():
        return 0
    return sum(1 for line in path.read_text(encoding="utf-8").splitlines() if line.strip())


def compute_fisher(vocabulary: list[dict]) -> dict[str, float]:
    if not vocabulary:
        return {}
    total_support = sum(int(entry.get("support", 0)) for entry in vocabulary) or 1
    fisher: dict[str, float] = {}
    for entry in vocabulary:
        token = str(entry.get("token", ""))
        if not token:
            continue
        weight = float(entry.get("weight", 0.0))
        support = int(entry.get("support", 0))
        fisher[token] = round((weight * weight) * (support / total_support), 6)
    return fisher


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Banana neuro consolidation pass.")
    parser.add_argument("--model", required=True, help="Model name, e.g. not-banana.")
    parser.add_argument("--vocabulary", type=Path, required=True)
    parser.add_argument("--replay-buffer", type=Path, default=None)
    parser.add_argument("--output-root", type=Path, required=True,
                        help="Root for artifacts/training/<model>/consolidation/<date>/")
    parser.add_argument("--phase", choices=["nrem", "rem"], default="nrem")
    parser.add_argument("--now", default=None, help="Override UTC date (YYYY-MM-DD or ISO).")
    parser.add_argument("--deterministic-timestamp", action="store_true",
                        help="Emit the deterministic 1970 timestamp instead of wall clock.")
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    vocab = load_vocabulary(args.vocabulary)
    fisher = compute_fisher(vocab)
    buffer_size = load_buffer_size(args.replay_buffer) if args.replay_buffer else 0

    out_dir: Path = args.output_root / utc_date(args.now)
    out_dir.mkdir(parents=True, exist_ok=True)

    generated_at = (
        DETERMINISTIC_GENERATED_AT
        if args.deterministic_timestamp
        else datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    )

    fisher_payload = {
        "schema_version": 1,
        "generated_at_utc": generated_at,
        "model": args.model,
        "phase": args.phase,
        "vocabulary_size": len(vocab),
        "fisher": fisher,
    }

    report_payload = {
        "schema_version": 1,
        "generated_at_utc": generated_at,
        "model": args.model,
        "phase": args.phase,
        "replay_buffer_size": buffer_size,
        "vocabulary_size": len(vocab),
        "fisher_token_count": len(fisher),
        "fisher_sum": round(sum(fisher.values()), 6),
        "cold_start": not vocab,
    }

    (out_dir / "fisher.json").write_text(
        json.dumps(fisher_payload, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    (out_dir / "consolidation-report.json").write_text(
        json.dumps(report_payload, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )

    print(json.dumps({"output_dir": str(out_dir), **report_payload}, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

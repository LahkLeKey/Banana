#!/usr/bin/env python3
"""Dopamine-style reward-modulated learning-rate scalar.

Spec ref: FR-004, US3 acceptance #2 (feature 050).

Returns a scalar in ``[0.25, 4.0]`` derived from the rolling delta of validation
accuracy across the most recent N sessions:

    delta = mean(accuracy[-window:]) - mean(accuracy[-2*window:-window])

The scalar maps positive deltas to >1 (more learning) and negative deltas to <1
(damp learning) via ``2 ** (k * delta)`` clamped to the bounds. With the default
``k = 8.0``, a +0.1 delta -> ~2.0x, and a -0.1 delta -> ~0.5x.
"""

from __future__ import annotations

import argparse
import json
import math
from pathlib import Path

LO = 0.25
HI = 4.0


def clamp(value: float, lo: float = LO, hi: float = HI) -> float:
    return max(lo, min(hi, value))


def rolling_delta(accuracies: list[float], window: int) -> float:
    if window <= 0 or len(accuracies) < 2:
        return 0.0
    if len(accuracies) < window * 2:
        # Cold-start: compare second half vs first half if we have at least 2.
        mid = len(accuracies) // 2
        if mid == 0:
            return 0.0
        recent = accuracies[mid:]
        prior = accuracies[:mid]
    else:
        recent = accuracies[-window:]
        prior = accuracies[-2 * window : -window]
    return (sum(recent) / len(recent)) - (sum(prior) / len(prior))


def scalar_from_delta(delta: float, k: float = 8.0) -> float:
    return clamp(2.0 ** (k * delta))


def load_accuracies(history_path: Path | None) -> list[float]:
    if history_path is None or not history_path.exists():
        return []
    payload = json.loads(history_path.read_text(encoding="utf-8"))
    if isinstance(payload, dict):
        records = payload.get("sessions", payload.get("records", []))
    else:
        records = payload
    out: list[float] = []
    for rec in records:
        if not isinstance(rec, dict):
            continue
        metrics = rec.get("metrics", rec)
        acc = metrics.get("holdout_accuracy") if isinstance(metrics, dict) else None
        if isinstance(acc, (int, float)):
            out.append(float(acc))
    return out


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Banana neuro reward modulator.")
    parser.add_argument("--history", type=Path, default=None)
    parser.add_argument("--window", type=int, default=3)
    parser.add_argument("--k", type=float, default=8.0)
    parser.add_argument("--accuracies", type=str, default=None,
                        help="Comma-separated list overriding --history.")
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    if args.accuracies:
        accuracies = [float(x) for x in args.accuracies.split(",") if x.strip()]
    else:
        accuracies = load_accuracies(args.history)
    delta = rolling_delta(accuracies, args.window)
    scalar = scalar_from_delta(delta, args.k)
    payload = {
        "schema_version": 1,
        "window": args.window,
        "k": args.k,
        "samples": len(accuracies),
        "delta": round(delta, 6),
        "lr_scalar": round(scalar, 6),
        "bounds": {"lo": LO, "hi": HI},
    }
    print(json.dumps(payload, indent=2, sort_keys=True))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

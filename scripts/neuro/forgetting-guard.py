#!/usr/bin/env python3
"""Anti-forgetting guard (feature 050 T016 — Wave A stub, Wave C completes).

Wave A purpose: provide a referenced file so workflow pre-flight passes and
trainer / consolidation pipelines can call it idempotently. Wave C will add
anchor-validation comparison logic and enforce the 1.0pp regression bound.

Current behavior:
    - Reads ``--metrics`` JSON if present and prints a status payload.
    - Always exits 0 (informational) so this stub never blocks CI.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Banana neuro forgetting guard (stub).")
    parser.add_argument("--model", required=True)
    parser.add_argument("--metrics", type=Path, required=True)
    parser.add_argument("--anchor", type=Path, default=None,
                        help="Anchor metrics for comparison (Wave C).")
    parser.add_argument("--max-regression-pp", type=float, default=1.0)
    parser.add_argument("--enforce", action="store_true",
                        help="Wave C: when set, exit non-zero on regression.")
    return parser.parse_args(argv)


def _load_accuracy(path: Path) -> float | None:
    if not path.exists():
        return None
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        return None
    metrics = payload.get("metrics", payload) if isinstance(payload, dict) else None
    if isinstance(metrics, dict):
        acc = metrics.get("holdout_accuracy")
        if isinstance(acc, (int, float)):
            return float(acc)
    return None


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    current = _load_accuracy(args.metrics)
    anchor = _load_accuracy(args.anchor) if args.anchor else None
    regression_pp = None
    if current is not None and anchor is not None:
        regression_pp = round((anchor - current) * 100.0, 4)
    payload = {
        "schema_version": 1,
        "model": args.model,
        "current_holdout_accuracy": current,
        "anchor_holdout_accuracy": anchor,
        "regression_pp": regression_pp,
        "max_regression_pp": args.max_regression_pp,
        "status": "informational-stub-wave-a",
    }
    print(json.dumps(payload, indent=2, sort_keys=True))
    if args.enforce and regression_pp is not None and regression_pp > args.max_regression_pp:
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

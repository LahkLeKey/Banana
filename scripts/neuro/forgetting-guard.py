#!/usr/bin/env python3
"""Anti-forgetting guard (feature 050 T016).

Compares current ``holdout_accuracy`` against an anchor metrics snapshot and
fails (exit 2) when regression exceeds ``--max-regression-pp`` percentage
points. Cold-start safe: when ``--anchor`` is missing or unreadable, the
check is skipped and the run is treated as informational (exit 0).

Wired into both ``orchestrate-neuro-git-event-training.yml`` and
``orchestrate-neuro-consolidation.yml`` as the final gating step.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Banana neuro forgetting guard.")
    parser.add_argument("--model", required=True)
    parser.add_argument("--metrics", type=Path, required=True)
    parser.add_argument("--anchor", type=Path, default=None,
                        help="Anchor metrics for comparison (e.g. data/<model>/anchor-metrics.json).")
    parser.add_argument("--max-regression-pp", type=float, default=1.0)
    parser.add_argument("--enforce", action="store_true",
                        help="Exit non-zero on regression beyond --max-regression-pp.")
    return parser.parse_args(argv)


def _load_accuracy(path: Path | None) -> float | None:
    if path is None or not path.exists():
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


def evaluate(
    *,
    current: float | None,
    anchor: float | None,
    max_regression_pp: float,
) -> tuple[float | None, str]:
    if current is None or anchor is None:
        return None, "informational"
    regression_pp = round((anchor - current) * 100.0, 4)
    if regression_pp > max_regression_pp:
        return regression_pp, "regression"
    return regression_pp, "ok"


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    current = _load_accuracy(args.metrics)
    anchor = _load_accuracy(args.anchor)
    regression_pp, status = evaluate(
        current=current, anchor=anchor, max_regression_pp=args.max_regression_pp
    )
    payload = {
        "schema_version": 1,
        "model": args.model,
        "current_holdout_accuracy": current,
        "anchor_holdout_accuracy": anchor,
        "regression_pp": regression_pp,
        "max_regression_pp": args.max_regression_pp,
        "status": status,
        "enforced": bool(args.enforce),
    }
    print(json.dumps(payload, indent=2, sort_keys=True))
    if args.enforce and status == "regression":
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

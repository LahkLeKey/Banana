#!/usr/bin/env python3
"""249-T003 — Cross-lane suspicion guard for training metrics.

Reads artifacts/training/{banana,not-banana,ripeness}/local/metrics.json and
flags suspicious patterns:
  - holdout_accuracy == 1.0 with holdout_size below minimum (trivially easy eval)
  - Diagonal-only confusion matrix with tiny per-cell support (multiclass lanes)

Exit codes:
  0  All lanes pass suspicion checks.
  1  One or more lanes are flagged as suspicious (prints remediation guidance).
"""

from __future__ import annotations

import json
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]

LANES = {
    "banana": {
        "path": REPO_ROOT / "artifacts" / "training" / "banana" / "local" / "metrics.json",
        "type": "binary",
        "min_honest_holdout": 8,
    },
    "not-banana": {
        "path": REPO_ROOT / "artifacts" / "training" / "not-banana" / "local" / "metrics.json",
        "type": "binary",
        "min_honest_holdout": 8,
    },
    "ripeness": {
        "path": REPO_ROOT / "artifacts" / "training" / "ripeness" / "local" / "metrics.json",
        "type": "multiclass",
        "min_honest_holdout": 12,
    },
}


def _is_diagonal_only(confusion: dict) -> bool:
    """Return True if every off-diagonal cell in the confusion matrix is zero."""
    for actual, pred_counts in confusion.items():
        for predicted, count in pred_counts.items():
            if actual != predicted and count > 0:
                return False
    return True


def _max_diagonal_cell(confusion: dict) -> int:
    return max(
        (v for row in confusion.values() for k, v in row.items() if k == list(row.keys())[0]),
        default=0,
    )


def _diagonal_support(confusion: dict) -> list[int]:
    supports = []
    for label, pred_counts in confusion.items():
        supports.append(pred_counts.get(label, 0))
    return supports


def check_lane(lane_name: str, lane_cfg: dict) -> list[str]:
    flags: list[str] = []
    metrics_path: Path = lane_cfg["path"]
    lane_type: str = lane_cfg["type"]
    min_holdout: int = lane_cfg["min_honest_holdout"]

    if not metrics_path.exists():
        flags.append(
            f"[{lane_name}] metrics.json not found at {metrics_path}. "
            "Run the trainer to generate artifacts before checking."
        )
        return flags

    payload = json.loads(metrics_path.read_text(encoding="utf-8"))
    metrics = payload.get("metrics", payload)

    accuracy = float(metrics.get("holdout_accuracy", 0.0))

    # Binary lanes store tp/fp/fn/tn; multiclass stores holdout_size directly.
    holdout_size = metrics.get("holdout_size")
    if holdout_size is None:
        holdout_size = int(
            metrics.get("tp", 0)
            + metrics.get("fp", 0)
            + metrics.get("fn", 0)
            + metrics.get("tn", 0)
        )
    else:
        holdout_size = int(holdout_size)

    # Rule 1: perfect accuracy with tiny holdout
    if accuracy >= 1.0 and holdout_size < min_holdout:
        flags.append(
            f"[{lane_name}] SUSPICIOUS: holdout_accuracy=1.0 but holdout_size={holdout_size} "
            f"< minimum honest holdout {min_holdout} for {lane_type} lane. "
            f"Expand eval split with hard-boundary samples "
            f"(target holdout_size >= {min_holdout})."
        )

    # Rule 2: diagonal-only confusion matrix with very small per-cell support
    confusion = metrics.get("confusion_matrix")
    if confusion and isinstance(confusion, dict) and _is_diagonal_only(confusion):
        support_per_label = _diagonal_support(confusion)
        if all(s <= 2 for s in support_per_label):
            flags.append(
                f"[{lane_name}] SUSPICIOUS: confusion matrix is perfectly diagonal "
                f"but each cell has support <= 2 ({support_per_label}). "
                "The eval set is trivially small — add boundary samples so each label "
                "has >= 3 eval examples including at least one hard-boundary case."
            )

    return flags


def main() -> int:
    all_flags: list[str] = []

    for lane_name, lane_cfg in LANES.items():
        lane_flags = check_lane(lane_name, lane_cfg)
        all_flags.extend(lane_flags)

    if all_flags:
        print("check-training-metrics-suspicion: FAIL")
        print()
        for flag in all_flags:
            print(f"  {flag}")
        print()
        print("Summary: suspicious metrics detected across the classifier lanes above.")
        print("Fix the eval split before treating training results as trustworthy.")
        return 1

    print("check-training-metrics-suspicion: PASS  (all lanes clear)")
    return 0


if __name__ == "__main__":
    sys.exit(main())

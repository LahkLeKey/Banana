"""249-T007 — Fixture tests for scripts/check-training-metrics-suspicion.py"""

from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "check-training-metrics-suspicion.py"

spec = importlib.util.spec_from_file_location("ctms", SCRIPT)
assert spec and spec.loader
ctms = importlib.util.module_from_spec(spec)
sys.modules["ctms"] = ctms
spec.loader.exec_module(ctms)

LABELS = ["unripe", "ripe", "overripe", "spoiled"]


def _make_metrics(
    tmp_path: Path,
    lane: str,
    accuracy: float,
    holdout_size: int,
    confusion: dict | None = None,
    include_confusion: bool = True,
) -> Path:
    if confusion is None and include_confusion:
        # Build a diagonal confusion matrix with per-cell support > 2
        per = max(3, holdout_size // len(LABELS))
        confusion = {la: {lb: (per if la == lb else 0) for lb in LABELS} for la in LABELS}
    payload: dict = {
        "schema_version": 1,
        "metrics": {
            "holdout_accuracy": accuracy,
            "holdout_size": holdout_size,
        },
    }
    if confusion is not None:
        payload["metrics"]["confusion_matrix"] = confusion
    lane_dir = tmp_path / lane / "local"
    lane_dir.mkdir(parents=True, exist_ok=True)
    p = lane_dir / "metrics.json"
    p.write_text(json.dumps(payload), encoding="utf-8")
    return tmp_path


def _args(tmp_root: Path) -> list[str]:
    """Return argv that points the suspicion guard at tmp_root."""
    return ["--artifacts-root", str(tmp_root)]


# ---------------------------------------------------------------------------
# Passing scenarios
# ---------------------------------------------------------------------------

def test_honest_metrics_pass(tmp_path: Path) -> None:
    """All lanes with honest accuracy and sufficient holdout_size should pass."""
    root = tmp_path
    _make_metrics(root, "banana", accuracy=0.9, holdout_size=10)
    _make_metrics(root, "not-banana", accuracy=1.0, holdout_size=12)
    # not-banana at 1.0 is fine if holdout_size >= 8
    _make_metrics(root, "ripeness", accuracy=0.875, holdout_size=12)
    assert ctms.main(_args(root)) == 0


def test_perfect_accuracy_with_large_holdout_passes(tmp_path: Path) -> None:
    root = tmp_path
    _make_metrics(root, "banana", accuracy=1.0, holdout_size=10)
    _make_metrics(root, "not-banana", accuracy=1.0, holdout_size=9)
    _make_metrics(root, "ripeness", accuracy=1.0, holdout_size=16)
    assert ctms.main(_args(root)) == 0


# ---------------------------------------------------------------------------
# Failing scenarios
# ---------------------------------------------------------------------------

def test_binary_lane_tiny_holdout_flags(tmp_path: Path) -> None:
    """banana at 1.0 with holdout_size=4 should be flagged."""
    root = tmp_path
    _make_metrics(root, "banana", accuracy=1.0, holdout_size=4)
    _make_metrics(root, "not-banana", accuracy=0.9, holdout_size=10)
    _make_metrics(root, "ripeness", accuracy=0.875, holdout_size=12)
    assert ctms.main(_args(root)) == 1


def test_multiclass_lane_tiny_holdout_flags(tmp_path: Path) -> None:
    """ripeness at 1.0 with holdout_size=8 (< 12) should be flagged."""
    root = tmp_path
    _make_metrics(root, "banana", accuracy=0.9, holdout_size=10)
    _make_metrics(root, "not-banana", accuracy=0.9, holdout_size=10)
    _make_metrics(root, "ripeness", accuracy=1.0, holdout_size=8)
    assert ctms.main(_args(root)) == 1


def test_diagonal_only_with_trivial_support_flags(tmp_path: Path) -> None:
    """Perfect diagonal with each cell = 1 is trivially suspicious."""
    trivial_confusion = {la: {lb: (1 if la == lb else 0) for lb in LABELS} for la in LABELS}
    root = tmp_path
    _make_metrics(root, "banana", accuracy=0.9, holdout_size=10)
    _make_metrics(root, "not-banana", accuracy=0.9, holdout_size=10)
    _make_metrics(root, "ripeness", accuracy=1.0, holdout_size=4, confusion=trivial_confusion)
    assert ctms.main(_args(root)) == 1


def test_missing_metrics_file_flags(tmp_path: Path) -> None:
    """Missing metrics.json should produce a FAIL flag."""
    root = tmp_path
    _make_metrics(root, "not-banana", accuracy=0.9, holdout_size=10)
    _make_metrics(root, "ripeness", accuracy=0.875, holdout_size=12)
    # banana metrics intentionally NOT created
    assert ctms.main(_args(root)) == 1

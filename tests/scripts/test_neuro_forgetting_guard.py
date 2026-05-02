"""Tests for scripts/neuro/forgetting-guard.py (feature 050 T016)."""
from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "neuro" / "forgetting-guard.py"

_spec = importlib.util.spec_from_file_location("neuro_forgetting_guard", SCRIPT)
assert _spec and _spec.loader
mod = importlib.util.module_from_spec(_spec)
sys.modules["neuro_forgetting_guard"] = mod
_spec.loader.exec_module(mod)


def _write_metrics(path: Path, accuracy: float) -> None:
    path.write_text(json.dumps({"metrics": {"holdout_accuracy": accuracy}}), encoding="utf-8")


def test_evaluate_ok_when_within_threshold():
    pp, status = mod.evaluate(current=0.85, anchor=0.86, max_regression_pp=1.0)
    assert status == "ok"
    assert pp == 1.0


def test_evaluate_flags_regression_beyond_threshold():
    pp, status = mod.evaluate(current=0.80, anchor=0.85, max_regression_pp=1.0)
    assert status == "regression"
    assert pp == 5.0


def test_evaluate_informational_when_anchor_missing():
    pp, status = mod.evaluate(current=0.9, anchor=None, max_regression_pp=1.0)
    assert status == "informational"
    assert pp is None


def test_main_enforce_exits_two_on_regression(tmp_path: Path, capsys):
    metrics = tmp_path / "metrics.json"
    anchor = tmp_path / "anchor.json"
    _write_metrics(metrics, 0.70)
    _write_metrics(anchor, 0.85)
    rc = mod.main([
        "--model", "not-banana",
        "--metrics", str(metrics),
        "--anchor", str(anchor),
        "--max-regression-pp", "1.0",
        "--enforce",
    ])
    assert rc == 2
    out = json.loads(capsys.readouterr().out)
    assert out["status"] == "regression"
    assert out["enforced"] is True


def test_main_enforce_passes_when_within_bound(tmp_path: Path, capsys):
    metrics = tmp_path / "metrics.json"
    anchor = tmp_path / "anchor.json"
    _write_metrics(metrics, 0.849)
    _write_metrics(anchor, 0.85)
    rc = mod.main([
        "--model", "banana",
        "--metrics", str(metrics),
        "--anchor", str(anchor),
        "--max-regression-pp", "1.0",
        "--enforce",
    ])
    assert rc == 0
    out = json.loads(capsys.readouterr().out)
    assert out["status"] == "ok"


def test_main_missing_anchor_returns_zero_even_with_enforce(tmp_path: Path, capsys):
    metrics = tmp_path / "metrics.json"
    _write_metrics(metrics, 0.5)
    rc = mod.main([
        "--model", "ripeness",
        "--metrics", str(metrics),
        "--anchor", str(tmp_path / "missing.json"),
        "--enforce",
    ])
    assert rc == 0
    out = json.loads(capsys.readouterr().out)
    assert out["status"] == "informational"

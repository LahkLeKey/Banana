"""Tests for scripts/neuro/reward-modulator.py (feature 050 T004)."""

from __future__ import annotations

import importlib.util
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "neuro" / "reward-modulator.py"

_spec = importlib.util.spec_from_file_location("neuro_reward_modulator", SCRIPT)
assert _spec and _spec.loader
mod = importlib.util.module_from_spec(_spec)
sys.modules["neuro_reward_modulator"] = mod
_spec.loader.exec_module(mod)


def test_scalar_clamped_to_lower_bound():
    # Very negative delta -> would map below 0.25.
    assert mod.scalar_from_delta(-1.0, k=8.0) == 0.25


def test_scalar_clamped_to_upper_bound():
    assert mod.scalar_from_delta(1.0, k=8.0) == 4.0


def test_zero_delta_returns_one():
    assert mod.scalar_from_delta(0.0, k=8.0) == 1.0


def test_positive_delta_increases_scalar():
    assert mod.scalar_from_delta(0.05, k=8.0) > 1.0


def test_negative_delta_decreases_scalar():
    assert mod.scalar_from_delta(-0.05, k=8.0) < 1.0


def test_rolling_delta_cold_start_returns_zero_for_singleton():
    assert mod.rolling_delta([0.7], window=3) == 0.0


def test_rolling_delta_short_history_uses_halves():
    # 2 samples: mid=1, recent=[0.8], prior=[0.6] -> +0.2
    assert abs(mod.rolling_delta([0.6, 0.8], window=3) - 0.2) < 1e-9


def test_rolling_delta_window_average():
    # window=2 over [0.5,0.5,0.7,0.9] -> recent=(0.7+0.9)/2=0.8, prior=0.5 -> +0.3
    assert abs(mod.rolling_delta([0.5, 0.5, 0.7, 0.9], window=2) - 0.3) < 1e-9


def test_clamp_bounds():
    assert mod.clamp(0.1) == 0.25
    assert mod.clamp(10.0) == 4.0
    assert mod.clamp(1.5) == 1.5


def test_load_accuracies_handles_missing(tmp_path: Path):
    assert mod.load_accuracies(tmp_path / "missing.json") == []
    assert mod.load_accuracies(None) == []

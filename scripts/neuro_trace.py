#!/usr/bin/env python3
"""Shared neuro-trace helper for trainers (feature 050 T006-T008).

Trainers call ``write_neuro_trace`` after they finish writing their normal
artifacts. When ``profile == "off"`` (default), this is a no-op so existing
deterministic outputs are unaffected.
"""

from __future__ import annotations

import importlib.util
import json
from pathlib import Path
from typing import Any

REPO_ROOT = Path(__file__).resolve().parents[1]
NEURO_DIR = REPO_ROOT / "scripts" / "neuro"

# Defer module imports until first call so this file stays cheap to import.
_reward_mod = None


def _load_reward_module():
    global _reward_mod
    if _reward_mod is not None:
        return _reward_mod
    spec = importlib.util.spec_from_file_location(
        "_neuro_reward_modulator", NEURO_DIR / "reward-modulator.py"
    )
    if not spec or not spec.loader:
        return None
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    _reward_mod = mod
    return mod


def write_neuro_trace(
    *,
    profile: str,
    output_dir: Path,
    model: str,
    history_accuracies: list[float],
    selected_metrics: dict[str, Any],
    run_fingerprint: str,
    deterministic_timestamp: str,
) -> dict[str, Any] | None:
    """Write ``neuro-trace.json`` to ``output_dir`` when the profile is enabled.

    Returns the trace payload (or ``None`` when disabled) so the trainer can
    include it in stdout JSON.
    """
    if profile == "off":
        return None

    reward = _load_reward_module()
    if reward is None:
        # Tools missing — emit a minimal trace rather than failing.
        scalar = 1.0
        delta = 0.0
        window = 0
    else:
        window = 3 if profile == "ci" else 5
        delta = reward.rolling_delta(history_accuracies, window)
        scalar = reward.scalar_from_delta(delta)

    payload = {
        "schema_version": 1,
        "generated_at_utc": deterministic_timestamp,
        "model": model,
        "neuro_profile": profile,
        "run_fingerprint": run_fingerprint,
        "reward": {
            "window": window,
            "delta": round(delta, 6),
            "lr_scalar": round(scalar, 6),
            "samples": len(history_accuracies),
            "bounds": {"lo": 0.25, "hi": 4.0},
        },
        "surprise": {
            "floor": 0.7,
            "blend": 0.3,
            "note": "Sampling weights are observed only; trainer outputs unchanged in v1.",
        },
        "replay": {
            "cap": 2048,
            "note": "Buffer admin handled out-of-band by scripts/neuro/replay-buffer.py.",
        },
        "selected_holdout_accuracy": float(
            selected_metrics.get("holdout_accuracy", 0.0)
        ),
    }

    output_dir = Path(output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)
    text = json.dumps(payload, indent=2, sort_keys=True) + "\n"
    (output_dir / "neuro-trace.json").write_bytes(text.encode("utf-8"))
    return payload

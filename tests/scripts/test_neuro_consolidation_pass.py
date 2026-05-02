"""Smoke tests for scripts/neuro/consolidation-pass.py (feature 050 T005)."""

from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "neuro" / "consolidation-pass.py"

_spec = importlib.util.spec_from_file_location("neuro_consolidation_pass", SCRIPT)
assert _spec and _spec.loader
mod = importlib.util.module_from_spec(_spec)
sys.modules["neuro_consolidation_pass"] = mod
_spec.loader.exec_module(mod)


def test_compute_fisher_empty_returns_empty():
    assert mod.compute_fisher([]) == {}


def test_compute_fisher_weights_squared_and_support_normalized():
    vocab = [
        {"token": "banana", "weight": 0.5, "support": 8},
        {"token": "yellow", "weight": -0.5, "support": 8},
    ]
    fisher = mod.compute_fisher(vocab)
    # Both tokens equal support fraction 0.5; weight^2 = 0.25 -> 0.125 each.
    assert abs(fisher["banana"] - 0.125) < 1e-6
    assert abs(fisher["yellow"] - 0.125) < 1e-6


def test_cold_start_writes_well_formed_outputs(tmp_path: Path):
    out_root = tmp_path / "consolidation"
    rc = mod.main(
        [
            "--model",
            "not-banana",
            "--vocabulary",
            str(tmp_path / "missing-vocab.json"),
            "--output-root",
            str(out_root),
            "--phase",
            "nrem",
            "--now",
            "2026-05-01",
            "--deterministic-timestamp",
        ]
    )
    assert rc == 0
    fisher_file = out_root / "2026-05-01" / "fisher.json"
    report_file = out_root / "2026-05-01" / "consolidation-report.json"
    assert fisher_file.exists()
    assert report_file.exists()
    fisher_payload = json.loads(fisher_file.read_text(encoding="utf-8"))
    report_payload = json.loads(report_file.read_text(encoding="utf-8"))
    assert fisher_payload["fisher"] == {}
    assert report_payload["cold_start"] is True
    assert report_payload["vocabulary_size"] == 0


def test_full_run_writes_fisher(tmp_path: Path):
    vocab_path = tmp_path / "vocab.json"
    vocab_path.write_text(
        json.dumps(
            {
                "vocabulary": [
                    {"token": "banana", "weight": 0.8, "support": 4},
                    {"token": "yellow", "weight": 0.5, "support": 3},
                ]
            }
        ),
        encoding="utf-8",
    )
    buffer_path = tmp_path / "buf.jsonl"
    buffer_path.write_text(
        '{"session_id":"s1"}\n{"session_id":"s2"}\n', encoding="utf-8"
    )
    out_root = tmp_path / "consolidation"
    rc = mod.main(
        [
            "--model",
            "banana",
            "--vocabulary",
            str(vocab_path),
            "--replay-buffer",
            str(buffer_path),
            "--output-root",
            str(out_root),
            "--phase",
            "rem",
            "--now",
            "2026-05-01",
            "--deterministic-timestamp",
        ]
    )
    assert rc == 0
    report = json.loads(
        (out_root / "2026-05-01" / "consolidation-report.json").read_text(
            encoding="utf-8"
        )
    )
    assert report["replay_buffer_size"] == 2
    assert report["vocabulary_size"] == 2
    assert report["phase"] == "rem"
    assert report["cold_start"] is False

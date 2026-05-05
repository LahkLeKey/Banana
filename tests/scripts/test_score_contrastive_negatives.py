"""Tests for scripts/score-contrastive-negatives.py"""

from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "score-contrastive-negatives.py"

spec = importlib.util.spec_from_file_location("scn", SCRIPT)
assert spec and spec.loader
scn = importlib.util.module_from_spec(spec)
sys.modules["scn"] = scn
spec.loader.exec_module(scn)


def test_contrastive_precision_pass(tmp_path: Path) -> None:
    dataset = {
        "samples": [
            {"id": "hn1", "text": "banana flavored candy", "category": "boundary", "expected_label": "not-banana"},
            {"id": "hn2", "text": "metal keyboard", "category": "technology", "expected_label": "not-banana"},
        ]
    }
    vocab = {
        "vocabulary": [
            {"token": "banana", "weight": 1.0},
            {"token": "flavored", "weight": -1.2},
            {"token": "candy", "weight": -0.8},
            {"token": "metal", "weight": -0.7},
            {"token": "keyboard", "weight": -0.7},
        ]
    }
    dp = tmp_path / "dataset.json"
    vp = tmp_path / "vocabulary.json"
    rp = tmp_path / "report.json"
    dp.write_text(json.dumps(dataset), encoding="utf-8")
    vp.write_text(json.dumps(vocab), encoding="utf-8")

    argv_backup = sys.argv[:]
    try:
        sys.argv = [
            "score-contrastive-negatives.py",
            "--dataset",
            str(dp),
            "--vocabulary",
            str(vp),
            "--report",
            str(rp),
            "--strict",
            "--min-contrastive-precision",
            "0.5",
        ]
        assert scn.main() == 0
    finally:
        sys.argv = argv_backup


def test_contrastive_precision_fail(tmp_path: Path) -> None:
    dataset = {
        "samples": [
            {"id": "hn1", "text": "banana candy", "category": "boundary", "expected_label": "not-banana"}
        ]
    }
    vocab = {"vocabulary": [{"token": "banana", "weight": 1.0}]}
    dp = tmp_path / "dataset.json"
    vp = tmp_path / "vocabulary.json"
    rp = tmp_path / "report.json"
    dp.write_text(json.dumps(dataset), encoding="utf-8")
    vp.write_text(json.dumps(vocab), encoding="utf-8")

    argv_backup = sys.argv[:]
    try:
        sys.argv = [
            "score-contrastive-negatives.py",
            "--dataset",
            str(dp),
            "--vocabulary",
            str(vp),
            "--report",
            str(rp),
            "--strict",
            "--min-contrastive-precision",
            "0.8",
        ]
        assert scn.main() == 1
    finally:
        sys.argv = argv_backup

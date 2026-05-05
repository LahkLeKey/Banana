"""Tests for scripts/score-banana-generalization-benchmark.py"""

from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "score-banana-generalization-benchmark.py"

spec = importlib.util.spec_from_file_location("sbgb", SCRIPT)
assert spec and spec.loader
sbgb = importlib.util.module_from_spec(spec)
sys.modules["sbgb"] = sbgb
spec.loader.exec_module(sbgb)


def test_benchmark_pass(tmp_path: Path) -> None:
    benchmark = {
        "samples": [
            {"text": "banana fruit", "expected_label": "banana", "concept": "fruit-identity", "context_family": "definition"},
            {"text": "metal keyboard", "expected_label": "not-banana", "concept": "fruit-identity", "context_family": "technology"},
        ]
    }
    vocabulary = {
        "vocabulary": [
            {"token": "banana", "weight": 1.0},
            {"token": "fruit", "weight": 0.5},
            {"token": "keyboard", "weight": -0.8},
            {"token": "metal", "weight": -0.4},
        ]
    }

    bench_path = tmp_path / "benchmark.json"
    vocab_path = tmp_path / "vocabulary.json"
    report_path = tmp_path / "report.json"
    bench_path.write_text(json.dumps(benchmark), encoding="utf-8")
    vocab_path.write_text(json.dumps(vocabulary), encoding="utf-8")

    rc = sbgb.main.__wrapped__() if hasattr(sbgb.main, "__wrapped__") else None
    del rc

    argv_backup = sys.argv[:]
    try:
        sys.argv = [
            "score-banana-generalization-benchmark.py",
            "--benchmark",
            str(bench_path),
            "--vocabulary",
            str(vocab_path),
            "--report",
            str(report_path),
            "--strict",
            "--min-lane-recall",
            "0.5",
            "--min-context-accuracy",
            "0.5",
        ]
        assert sbgb.main() == 0
    finally:
        sys.argv = argv_backup

    report = json.loads(report_path.read_text(encoding="utf-8"))
    assert report["overall_accuracy"] == 1.0
    assert report["failing_lanes"] == {}


def test_benchmark_strict_fail(tmp_path: Path) -> None:
    benchmark = {
        "samples": [
            {"text": "banana fruit", "expected_label": "banana", "concept": "fruit-identity", "context_family": "definition"},
            {"text": "banana candy", "expected_label": "not-banana", "concept": "fruit-identity", "context_family": "boundary"},
        ]
    }
    vocabulary = {"vocabulary": [{"token": "banana", "weight": 1.0}]}

    bench_path = tmp_path / "benchmark.json"
    vocab_path = tmp_path / "vocabulary.json"
    report_path = tmp_path / "report.json"
    bench_path.write_text(json.dumps(benchmark), encoding="utf-8")
    vocab_path.write_text(json.dumps(vocabulary), encoding="utf-8")

    argv_backup = sys.argv[:]
    try:
        sys.argv = [
            "score-banana-generalization-benchmark.py",
            "--benchmark",
            str(bench_path),
            "--vocabulary",
            str(vocab_path),
            "--report",
            str(report_path),
            "--strict",
            "--min-context-accuracy",
            "0.8",
        ]
        assert sbgb.main() == 1
    finally:
        sys.argv = argv_backup

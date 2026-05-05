"""Tests for scripts/score-banana-explanations.py"""

from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "score-banana-explanations.py"

spec = importlib.util.spec_from_file_location("sbe", SCRIPT)
assert spec and spec.loader
sbe = importlib.util.module_from_spec(spec)
sys.modules["sbe"] = sbe
spec.loader.exec_module(sbe)


def test_explanation_quality_pass(tmp_path: Path) -> None:
    rubric = {
        "required_facts": [
            {"id": "fruit", "required_terms": ["fruit"]},
            {"id": "peel", "required_terms": ["peel"]},
        ],
        "min_words": 3,
        "max_words": 40,
        "strict_required_facts": 2,
    }
    prompts = {
        "items": [
            {
                "id": "exp-1",
                "candidate_explanation": "Banana is a fruit with a peel that people eat.",
            }
        ]
    }
    rp = tmp_path / "rubric.json"
    pp = tmp_path / "prompts.json"
    out = tmp_path / "report.json"
    rp.write_text(json.dumps(rubric), encoding="utf-8")
    pp.write_text(json.dumps(prompts), encoding="utf-8")

    argv_backup = sys.argv[:]
    try:
        sys.argv = [
            "score-banana-explanations.py",
            "--rubric",
            str(rp),
            "--prompts",
            str(pp),
            "--report",
            str(out),
            "--strict",
        ]
        assert sbe.main() == 0
    finally:
        sys.argv = argv_backup

    report = json.loads(out.read_text(encoding="utf-8"))
    assert report["strict_failures"] == 0


def test_explanation_quality_fail_on_missing_facts(tmp_path: Path) -> None:
    rubric = {
        "required_facts": [
            {"id": "fruit", "required_terms": ["fruit"]},
            {"id": "potassium", "required_terms": ["potassium"]},
        ],
        "min_words": 3,
        "max_words": 20,
        "strict_required_facts": 2,
    }
    prompts = {"items": [{"id": "exp-2", "candidate_explanation": "Banana is a fruit."}]}
    rp = tmp_path / "rubric.json"
    pp = tmp_path / "prompts.json"
    out = tmp_path / "report.json"
    rp.write_text(json.dumps(rubric), encoding="utf-8")
    pp.write_text(json.dumps(prompts), encoding="utf-8")

    argv_backup = sys.argv[:]
    try:
        sys.argv = [
            "score-banana-explanations.py",
            "--rubric",
            str(rp),
            "--prompts",
            str(pp),
            "--report",
            str(out),
            "--strict",
        ]
        assert sbe.main() == 1
    finally:
        sys.argv = argv_backup

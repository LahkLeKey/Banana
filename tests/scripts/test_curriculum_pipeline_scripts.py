"""Tests for curriculum gap/candidate/ingest scripts."""

from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]


def _load(name: str, script_path: Path):
    spec = importlib.util.spec_from_file_location(name, script_path)
    assert spec and spec.loader
    mod = importlib.util.module_from_spec(spec)
    sys.modules[name] = mod
    spec.loader.exec_module(mod)
    return mod


mine = _load("mine_gaps", REPO_ROOT / "scripts" / "mine-banana-knowledge-gaps.py")
gen = _load("gen_curr", REPO_ROOT / "scripts" / "generate-banana-curriculum-candidates.py")
ingest = _load("ingest_curr", REPO_ROOT / "scripts" / "ingest-banana-curriculum-staging.py")


def test_curriculum_pipeline(tmp_path: Path) -> None:
    ontology = {
        "min_samples_per_concept": 2,
        "concepts": [
            {"id": "fruit-identity"},
            {"id": "nutrition-potassium"},
        ],
    }
    coverage = {"counts": {"fruit-identity": 3, "nutrition-potassium": 0}}
    generalization = {"per_concept_recall": {"fruit-identity": 0.9, "nutrition-potassium": 0.2}}
    staging = {"max_auto_accept_per_run": 2, "entries": []}
    corpus = {
        "schema_version": 1,
        "description": "fixture",
        "samples": [
            {"id": "b1", "label": "banana", "text": "banana fruit"},
            {"id": "n1", "label": "not-banana", "text": "metal keyboard"},
        ],
    }

    ontology_p = tmp_path / "ontology.json"
    coverage_p = tmp_path / "coverage.json"
    gen_p = tmp_path / "generalization.json"
    gaps_p = tmp_path / "gaps.json"
    staging_p = tmp_path / "staging.json"
    corpus_p = tmp_path / "corpus.json"
    report_p = tmp_path / "ingest-report.json"

    ontology_p.write_text(json.dumps(ontology), encoding="utf-8")
    coverage_p.write_text(json.dumps(coverage), encoding="utf-8")
    gen_p.write_text(json.dumps(generalization), encoding="utf-8")
    staging_p.write_text(json.dumps(staging), encoding="utf-8")
    corpus_p.write_text(json.dumps(corpus), encoding="utf-8")

    argv_backup = sys.argv[:]
    try:
        sys.argv = [
            "mine-banana-knowledge-gaps.py",
            "--ontology",
            str(ontology_p),
            "--coverage-report",
            str(coverage_p),
            "--generalization-report",
            str(gen_p),
            "--output",
            str(gaps_p),
        ]
        assert mine.main() == 0

        sys.argv = [
            "generate-banana-curriculum-candidates.py",
            "--gaps",
            str(gaps_p),
            "--staging",
            str(staging_p),
            "--max-new",
            "2",
        ]
        assert gen.main() == 0

        sys.argv = [
            "ingest-banana-curriculum-staging.py",
            "--corpus",
            str(corpus_p),
            "--staging",
            str(staging_p),
            "--max-accept",
            "2",
            "--report",
            str(report_p),
        ]
        assert ingest.main() == 0
    finally:
        sys.argv = argv_backup

    report = json.loads(report_p.read_text(encoding="utf-8"))
    assert report["accepted_count"] >= 1

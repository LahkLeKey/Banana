"""247-T008 — Fixture tests for scripts/check-ripeness-eval-split.py"""

from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "check-ripeness-eval-split.py"

spec = importlib.util.spec_from_file_location("cres", SCRIPT)
assert spec and spec.loader
cres = importlib.util.module_from_spec(spec)
sys.modules["cres"] = cres
spec.loader.exec_module(cres)

LABELS = ["unripe", "ripe", "overripe", "spoiled"]


def _make_corpus(tmp_path: Path, eval_ids: list[str], samples: list[dict]) -> Path:
    payload = {
        "schema_version": 1,
        "labels": LABELS,
        "splits": {"eval_ids": eval_ids},
        "samples": samples,
    }
    p = tmp_path / "corpus.json"
    p.write_text(json.dumps(payload), encoding="utf-8")
    return p


def _boundary_sample(sid: str, label: str) -> dict:
    return {"id": sid, "label": label, "text": f"boundary text for {label}", "source": "hard-boundary-v2"}


def _seed_sample(sid: str, label: str) -> dict:
    return {"id": sid, "label": label, "text": f"seed text for {label}", "source": "seed"}


def _passing_corpus(tmp_path: Path) -> Path:
    """Build a minimal corpus that satisfies all FR-247 constraints."""
    samples = []
    eval_ids = []
    for label in LABELS:
        for i in range(1, 10):
            sid = f"{label}-{i:03d}"
            if i <= 2:
                s = _boundary_sample(sid, label)
                eval_ids.append(sid)
            elif i == 3:
                s = _seed_sample(sid, label)
                eval_ids.append(sid)
            else:
                s = _seed_sample(sid, label)
            samples.append(s)
    return _make_corpus(tmp_path, eval_ids, samples)


def test_passing_corpus_exits_zero(tmp_path: Path) -> None:
    p = _passing_corpus(tmp_path)
    assert cres.main(["--corpus", str(p)]) == 0


def test_too_few_eval_ids_exits_one(tmp_path: Path) -> None:
    """Only 8 eval_ids — below MIN_EVAL_IDS=12."""
    samples = []
    eval_ids = []
    for label in LABELS:
        for i in range(1, 10):
            sid = f"{label}-{i:03d}"
            s = _boundary_sample(sid, label) if i <= 2 else _seed_sample(sid, label)
            samples.append(s)
            if i <= 2:
                eval_ids.append(sid)  # only 2 per label = 8 total
    p = _make_corpus(tmp_path, eval_ids, samples)
    assert cres.main(["--corpus", str(p)]) == 1


def test_missing_label_in_eval_exits_one(tmp_path: Path) -> None:
    """overripe has zero eval entries — violates per-label >= 3."""
    samples = []
    eval_ids = []
    for label in LABELS:
        for i in range(1, 13):
            sid = f"{label}-{i:03d}"
            s = _boundary_sample(sid, label) if i <= 2 else _seed_sample(sid, label)
            samples.append(s)
            if label != "overripe" and i <= 3:
                eval_ids.append(sid)
    p = _make_corpus(tmp_path, eval_ids, samples)
    assert cres.main(["--corpus", str(p)]) == 1


def test_low_boundary_ratio_exits_one(tmp_path: Path) -> None:
    """All eval_ids are seed samples — boundary_ratio = 0 < 0.5."""
    samples = []
    eval_ids = []
    for label in LABELS:
        for i in range(1, 14):
            sid = f"{label}-{i:03d}"
            s = _seed_sample(sid, label)
            samples.append(s)
            if i <= 3:
                eval_ids.append(sid)  # 3 per label = 12 total, but all seed
    p = _make_corpus(tmp_path, eval_ids, samples)
    assert cres.main(["--corpus", str(p)]) == 1


def test_missing_corpus_exits_one(tmp_path: Path) -> None:
    assert cres.main(["--corpus", str(tmp_path / "nonexistent.json")]) == 1

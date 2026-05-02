"""Tests for scripts/neuro/surprise-sampler.py (feature 050 T003)."""
from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "neuro" / "surprise-sampler.py"

_spec = importlib.util.spec_from_file_location("neuro_surprise_sampler", SCRIPT)
assert _spec and _spec.loader
mod = importlib.util.module_from_spec(_spec)
sys.modules["neuro_surprise_sampler"] = mod
_spec.loader.exec_module(mod)


def test_floor_blends_to_seventy_thirty():
    surprise = [0.5, 0.5, 0.5, 0.5]  # uniform after normalize: 0.25 each
    norm = mod.normalize(surprise)
    blended = mod.blend_with_uniform_floor(norm, floor=0.7)
    # Each element = 0.7 * 0.25 + 0.3 * 0.25 = 0.25
    for v in blended:
        assert abs(v - 0.25) < 1e-9


def test_floor_dominates_extreme_surprise():
    n = 4
    norm = [1.0, 0.0, 0.0, 0.0]
    blended = mod.blend_with_uniform_floor(norm, floor=0.7)
    uniform = 0.7 / n  # 0.175
    # Lowest weight stays >= floor*uniform; never zero.
    for v in blended[1:]:
        assert v >= uniform - 1e-9
    # Highest weight is floor*uniform + 0.3*1 = 0.475
    assert abs(blended[0] - (uniform + 0.3)) < 1e-9


def test_rank_ordering_preserved_high_surprise_first():
    # Boundary samples (low |score|) should rank above confident ones.
    weights = {"banana": 1.0, "yellow": 1.0}
    texts = [
        "banana yellow banana yellow",  # confident -> low surprise
        "neutral pink table",            # boundary -> high surprise
    ]
    raw = mod.compute_surprise(texts, weights)
    assert raw[1] > raw[0]


def test_empty_vocab_uniform():
    raw = mod.compute_surprise(["a", "b", "c"], {})
    assert raw == [0.5, 0.5, 0.5]


def test_normalize_zero_total_returns_uniform():
    out = mod.normalize([0.0, 0.0, 0.0])
    for v in out:
        assert abs(v - 1.0 / 3) < 1e-9


def test_build_payload_writes_expected_shape(tmp_path: Path):
    vocab = tmp_path / "vocab.json"
    vocab.write_text(json.dumps({
        "vocabulary": [
            {"token": "banana", "weight": 0.8, "support": 4},
            {"token": "yellow", "weight": 0.5, "support": 3},
        ],
    }), encoding="utf-8")
    corpus = tmp_path / "corpus.json"
    corpus.write_text(json.dumps({"samples": [
        {"text": "banana", "label": "banana"},
        {"text": "table", "label": "not-banana"},
        {"text": "yellow banana", "label": "banana"},
    ]}), encoding="utf-8")
    payload = mod.build_payload(vocab, corpus, floor=0.7)
    assert payload["sample_count"] == 3
    assert len(payload["surprise_weights"]) == 3
    # Probabilities sum to ~1.0.
    assert abs(sum(payload["surprise_weights"]) - 1.0) < 1e-3

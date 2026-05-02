"""Tests for scripts/neuro/replay-buffer.py (feature 050 T002)."""
from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "neuro" / "replay-buffer.py"

_spec = importlib.util.spec_from_file_location("neuro_replay_buffer", SCRIPT)
assert _spec and _spec.loader
mod = importlib.util.module_from_spec(_spec)
sys.modules["neuro_replay_buffer"] = mod
_spec.loader.exec_module(mod)


def _records(n: int) -> list[dict]:
    return [{"session_id": f"s{i:04d}", "observed_at_utc": f"2026-04-{(i % 28) + 1:02d}T00:00:00Z"} for i in range(n)]


def test_reservoir_append_under_cap_keeps_all_sorted():
    out = mod.reservoir_append([], _records(10), cap=2048, seed=1)
    assert len(out) == 10
    ids = [r["session_id"] for r in out]
    assert ids == sorted(ids)


def test_reservoir_append_caps_at_size():
    incoming = _records(5000)
    out = mod.reservoir_append([], incoming, cap=2048, seed=42)
    assert len(out) == 2048
    # Deterministic with same seed.
    out2 = mod.reservoir_append([], incoming, cap=2048, seed=42)
    assert [r["session_id"] for r in out] == [r["session_id"] for r in out2]


def test_reservoir_append_different_seeds_diverge():
    incoming = _records(3000)
    a = mod.reservoir_append([], incoming, cap=512, seed=1)
    b = mod.reservoir_append([], incoming, cap=512, seed=2)
    assert a != b


def test_deterministic_sample_stable():
    recs = _records(100)
    a = mod.deterministic_sample(recs, n=10, seed=7)
    b = mod.deterministic_sample(recs, n=10, seed=7)
    assert a == b
    assert len(a) == 10


def test_deterministic_sample_n_ge_len_returns_copy():
    recs = _records(3)
    out = mod.deterministic_sample(recs, n=10, seed=1)
    assert out == recs
    assert out is not recs


def test_prune_by_age_drops_old_records():
    recs = [
        {"session_id": "old", "observed_at_utc": "2024-01-01T00:00:00Z"},
        {"session_id": "new", "observed_at_utc": "2026-04-30T00:00:00Z"},
    ]
    kept = mod.prune_by_age(recs, max_age_days=30, now_iso="2026-05-01T00:00:00Z")
    assert [r["session_id"] for r in kept] == ["new"]


def test_prune_by_age_zero_keeps_all():
    recs = _records(5)
    assert mod.prune_by_age(recs, max_age_days=0, now_iso="2026-05-01T00:00:00Z") == recs


def test_load_and_write_roundtrip(tmp_path: Path):
    path = tmp_path / "buf.jsonl"
    mod.write_buffer(path, _records(3))
    assert mod.load_buffer(path) == _records(3)


def test_write_buffer_emits_lf_only(tmp_path: Path):
    path = tmp_path / "buf.jsonl"
    mod.write_buffer(path, _records(2))
    raw = path.read_bytes()
    assert b"\r\n" not in raw
    assert raw.endswith(b"\n")


def test_cli_append_then_sample(tmp_path: Path, capsys):
    path = tmp_path / "buf.jsonl"
    payload = tmp_path / "in.json"
    payload.write_text(json.dumps(_records(5)), encoding="utf-8")
    rc = mod.main(["--buffer", str(path), "--op", "append", "--records-file", str(payload)])
    assert rc == 0
    capsys.readouterr()
    rc = mod.main(["--buffer", str(path), "--op", "sample", "--n", "2", "--seed", "3"])
    assert rc == 0
    out = capsys.readouterr().out.strip().splitlines()
    assert len(out) == 2
    for line in out:
        json.loads(line)

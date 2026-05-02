"""Tests for scripts/neuro/scope-models-from-event.py (feature 050 T013)."""

from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path
from unittest import mock


REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "neuro" / "scope-models-from-event.py"

_spec = importlib.util.spec_from_file_location("neuro_scope", SCRIPT)
assert _spec and _spec.loader
mod = importlib.util.module_from_spec(_spec)
sys.modules["neuro_scope"] = mod
_spec.loader.exec_module(mod)


def test_scope_from_paths_only_banana():
    assert mod.scope_from_paths(["data/banana/corpus.json"]) == ["banana"]


def test_scope_from_paths_multi():
    paths = ["data/banana/corpus.json", "scripts/train-ripeness-model.py"]
    assert mod.scope_from_paths(paths) == ["banana", "ripeness"]


def test_scope_from_paths_shared_returns_all():
    assert mod.scope_from_paths(["scripts/neuro/replay-buffer.py"]) == list(
        mod.ALL_MODELS
    )
    assert mod.scope_from_paths(["scripts/neuro_trace.py"]) == list(mod.ALL_MODELS)


def test_scope_from_paths_unrelated_returns_empty():
    assert mod.scope_from_paths(["docs/release-notes/000-foo.md"]) == []


def test_scope_from_labels_specific_model():
    assert mod.scope_from_labels(json.dumps(["train:not-banana", "automation"])) == [
        "not-banana"
    ]


def test_scope_from_labels_all():
    assert mod.scope_from_labels(json.dumps(["train:all"])) == list(mod.ALL_MODELS)


def test_scope_from_labels_invalid_json_returns_empty():
    assert mod.scope_from_labels("not-json") == []


def test_main_dispatch_all(capsys, monkeypatch):
    monkeypatch.setenv("BANANA_NEURO_EVENT_NAME", "workflow_dispatch")
    monkeypatch.setenv("BANANA_NEURO_DISPATCH_MODEL", "all")
    monkeypatch.setenv("BANANA_NEURO_PR_LABELS", "")
    rc = mod.main()
    assert rc == 0
    out = capsys.readouterr().out.strip().splitlines()
    assert any(line.startswith("models=") for line in out)
    models_line = next(line for line in out if line.startswith("models="))
    assert json.loads(models_line.split("=", 1)[1]) == list(mod.ALL_MODELS)
    assert "run=true" in out


def test_main_push_with_no_changes(capsys, monkeypatch):
    monkeypatch.setenv("BANANA_NEURO_EVENT_NAME", "push")
    monkeypatch.setenv("BANANA_NEURO_DISPATCH_MODEL", "")
    monkeypatch.setenv("BANANA_NEURO_PR_LABELS", "")
    with mock.patch.object(mod, "changed_paths", return_value=["docs/foo.md"]):
        rc = mod.main()
    assert rc == 0
    out = capsys.readouterr().out
    assert "models=[]" in out
    assert "run=false" in out


def test_main_pr_target_label_overrides_paths(capsys, monkeypatch):
    monkeypatch.setenv("BANANA_NEURO_EVENT_NAME", "pull_request_target")
    monkeypatch.setenv("BANANA_NEURO_DISPATCH_MODEL", "")
    monkeypatch.setenv("BANANA_NEURO_PR_LABELS", json.dumps(["train:banana"]))
    with mock.patch.object(mod, "changed_paths", return_value=["docs/foo.md"]):
        rc = mod.main()
    assert rc == 0
    out = capsys.readouterr().out
    assert 'models=["banana"]' in out
    assert "run=true" in out

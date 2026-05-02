#!/usr/bin/env python3
"""Scope which models to retrain from a git event (feature 050 T013).

Reads:
    BANANA_NEURO_EVENT_NAME    workflow_dispatch | push | pull_request_target
    BANANA_NEURO_DISPATCH_MODEL  optional override: all | banana | not-banana | ripeness
    BANANA_NEURO_PR_LABELS      JSON array of PR labels (for pull_request_target)

Emits, on stdout (suitable for ``>> $GITHUB_OUTPUT``):
    models=<json array>
    run=<true|false>

When triggered by ``push`` (or ``pull_request_target`` without explicit
'train:<model>' labels), the union of changed-path scopes is used. If the
union is empty, ``run=false`` short-circuits the matrix.
"""

from __future__ import annotations

import json
import os
import subprocess
import sys
from pathlib import Path

ALL_MODELS = ("banana", "not-banana", "ripeness")

PATH_SCOPES = {
    "banana": ("data/banana/", "scripts/train-banana-model.py"),
    "not-banana": ("data/not-banana/", "scripts/train-not-banana-model.py"),
    "ripeness": ("data/ripeness/", "scripts/train-ripeness-model.py"),
}
SHARED_PATHS = ("scripts/neuro/", "scripts/neuro_trace.py")


def changed_paths() -> list[str]:
    try:
        out = subprocess.check_output(
            ["git", "diff", "--name-only", "HEAD~1", "HEAD"], text=True
        )
    except subprocess.CalledProcessError:
        return []
    return [line.strip() for line in out.splitlines() if line.strip()]


def scope_from_paths(paths: list[str]) -> list[str]:
    selected: set[str] = set()
    for path in paths:
        if any(path.startswith(p) for p in SHARED_PATHS):
            return list(ALL_MODELS)
        for model, prefixes in PATH_SCOPES.items():
            if any(path.startswith(p) for p in prefixes):
                selected.add(model)
    return [m for m in ALL_MODELS if m in selected]


def scope_from_labels(labels_json: str) -> list[str]:
    try:
        labels = json.loads(labels_json) if labels_json else []
    except json.JSONDecodeError:
        labels = []
    selected: set[str] = set()
    for label in labels:
        if not isinstance(label, str):
            continue
        if label == "train:all":
            return list(ALL_MODELS)
        if label.startswith("train:"):
            model = label.removeprefix("train:")
            if model in ALL_MODELS:
                selected.add(model)
    return [m for m in ALL_MODELS if m in selected]


def main() -> int:
    event = os.environ.get("BANANA_NEURO_EVENT_NAME", "").strip()
    dispatch_model = os.environ.get("BANANA_NEURO_DISPATCH_MODEL", "").strip()
    labels_json = os.environ.get("BANANA_NEURO_PR_LABELS", "").strip()

    if event == "workflow_dispatch":
        if dispatch_model in {"", "all"}:
            models = list(ALL_MODELS)
        elif dispatch_model in ALL_MODELS:
            models = [dispatch_model]
        else:
            models = []
    elif event == "pull_request_target":
        models = scope_from_labels(labels_json) or scope_from_paths(changed_paths())
    else:
        models = scope_from_paths(changed_paths())

    payload = json.dumps(models)
    sys.stdout.write(f"models={payload}\n")
    sys.stdout.write(f"run={'true' if models else 'false'}\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

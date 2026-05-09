"""Regression tests for retired self-training contract guards (spec 003 T013)."""

from __future__ import annotations

import importlib.util
import os
import subprocess
import sys
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
VALIDATOR_SCRIPT = REPO_ROOT / ".specify" / "scripts" / "validate-ai-contracts.py"
ORCHESTRATE_SDLC_SCRIPT = REPO_ROOT / "scripts" / "workflow-orchestrate-sdlc.sh"

_spec = importlib.util.spec_from_file_location(
    "validate_ai_contracts", VALIDATOR_SCRIPT
)
assert _spec and _spec.loader
validator = importlib.util.module_from_spec(_spec)
sys.modules["validate_ai_contracts"] = validator
_spec.loader.exec_module(validator)


def _run_sdlc_with_catalog(path: str) -> subprocess.CompletedProcess[str]:
    command = (
        "BANANA_SDLC_INCREMENT_CATALOG_PATH='"
        + path
        + "' bash scripts/workflow-orchestrate-sdlc.sh spec-drain"
    )
    return subprocess.run(
        ["bash", "-lc", command],
        cwd=REPO_ROOT,
        check=False,
        text=True,
        capture_output=True,
        env={**os.environ},
    )


def test_retired_self_training_fragments_flagged_in_active_targets() -> None:
    target_texts = {
        "scripts/workflow-orchestrate-feedback-loop.sh": (
            "uses docs/automation/agent-pulse/autonomous-self-training-default-increments.json"
        )
    }

    issues = validator.evaluate_retired_self_training_contract_violations(target_texts)

    assert len(issues) == 1
    assert "RETIRED_SELF_TRAINING_CONTRACT" in issues[0]
    assert "autonomous-self-training-default-increments.json" in issues[0]


def test_retired_self_training_fragments_not_flagged_when_absent() -> None:
    target_texts = {
        "scripts/workflow-orchestrate-sdlc.sh": (
            "uses docs/automation/agent-pulse/sdlc-default-increments.json"
        )
    }

    issues = validator.evaluate_retired_self_training_contract_violations(target_texts)

    assert issues == []


def test_sdlc_script_rejects_retired_catalog_path() -> None:
    result = _run_sdlc_with_catalog(
        "docs/automation/agent-pulse/autonomous-self-training-default-increments.json"
    )

    assert result.returncode == 1
    combined = (result.stdout or "") + (result.stderr or "")
    assert "retired self-training catalog path is not allowed" in combined


def test_sdlc_script_accepts_non_retired_catalog_path() -> None:
    # /dev/null is a non-retired path that keeps this test focused on guard behavior
    # without depending on full orchestration prerequisites.
    result = _run_sdlc_with_catalog("/dev/null")

    assert "retired self-training catalog path is not allowed" not in (
        (result.stdout or "") + (result.stderr or "")
    )

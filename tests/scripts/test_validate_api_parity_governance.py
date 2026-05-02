"""Tests for scripts/validate-api-parity-governance.sh."""

from __future__ import annotations

import json
import subprocess
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "validate-api-parity-governance.sh"
INVENTORY = (
    REPO_ROOT
    / ".specify"
    / "specs"
    / "047-api-parity-governance"
    / "artifacts"
    / "overlapping-routes.inventory.json"
)
DRIFT = (
    REPO_ROOT
    / ".specify"
    / "specs"
    / "047-api-parity-governance"
    / "artifacts"
    / "parity-drift-report.json"
)
GATE = (
    REPO_ROOT
    / ".specify"
    / "specs"
    / "047-api-parity-governance"
    / "artifacts"
    / "parity-gate-result.json"
)
EXCEPTIONS = (
    REPO_ROOT
    / ".specify"
    / "specs"
    / "047-api-parity-governance"
    / "artifacts"
    / "parity-exceptions.json"
)


def _run(*args: str) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        ["bash", str(SCRIPT), *args],
        cwd=REPO_ROOT,
        check=False,
        text=True,
        capture_output=True,
    )


def test_inventory_mode_generates_overlap_artifact() -> None:
    result = _run("--inventory-only")
    assert result.returncode == 0, result.stderr
    assert INVENTORY.exists()

    payload = json.loads(INVENTORY.read_text(encoding="utf-8"))
    routes = payload.get("routes", [])
    assert isinstance(routes, list)
    assert routes, "expected at least one overlap route"

    keys = {route.get("route_key") for route in routes}
    assert "GET /health" in keys
    assert "POST /ripeness/predict" in keys


def test_non_strict_mode_writes_drift_report() -> None:
    result = _run()
    assert result.returncode == 0, result.stderr
    assert DRIFT.exists()
    assert GATE.exists()

    payload = json.loads(DRIFT.read_text(encoding="utf-8"))
    assert "summary" in payload
    assert "findings" in payload
    assert payload["summary"]["total_findings"] >= 0

    gate_payload = json.loads(GATE.read_text(encoding="utf-8"))
    assert gate_payload["decision"] in {"PASS", "FAIL"}


def test_strict_mode_fails_when_unresolved_missing_routes_exist() -> None:
    original_exceptions = (
        EXCEPTIONS.read_text(encoding="utf-8") if EXCEPTIONS.exists() else None
    )
    EXCEPTIONS.write_text(
        json.dumps(
            {"updated_at_utc": "2026-04-26T00:00:00Z", "exceptions": []}, indent=2
        )
        + "\n",
        encoding="utf-8",
    )

    try:
        result = _run("--strict")
        # With no active exceptions and known cross-surface drift, strict mode must fail.
        assert result.returncode == 1

        gate_payload = json.loads(GATE.read_text(encoding="utf-8"))
        assert gate_payload["decision"] == "FAIL"
    finally:
        if original_exceptions is None:
            EXCEPTIONS.unlink(missing_ok=True)
        else:
            EXCEPTIONS.write_text(original_exceptions, encoding="utf-8")

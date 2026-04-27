"""Tests for scripts/validate-api-parity-governance.sh."""
from __future__ import annotations

import json
import subprocess
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "validate-api-parity-governance.sh"
INVENTORY = REPO_ROOT / ".specify" / "specs" / "047-api-parity-governance" / "artifacts" / "overlapping-routes.inventory.json"
DRIFT = REPO_ROOT / ".specify" / "specs" / "047-api-parity-governance" / "artifacts" / "parity-drift-report.json"


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

    payload = json.loads(DRIFT.read_text(encoding="utf-8"))
    assert "summary" in payload
    assert "findings" in payload
    assert payload["summary"]["total_findings"] >= 0


def test_strict_mode_fails_when_unresolved_missing_routes_exist() -> None:
    result = _run("--strict")
    # Current repo intentionally has non-overlapping routes across ASP.NET/Fastify.
    assert result.returncode == 1

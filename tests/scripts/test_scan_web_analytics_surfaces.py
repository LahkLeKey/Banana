"""256-T005 — Determinism and threshold tests for scripts/scan-web-analytics-surfaces.ts"""

from __future__ import annotations

import json
import subprocess
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "scan-web-analytics-surfaces.ts"
MANIFEST = REPO_ROOT / "scripts" / "analytics-surface-manifest.json"
ROUTER = REPO_ROOT / "src" / "typescript" / "react" / "src" / "lib" / "router.tsx"
ANALYTICS = REPO_ROOT / "src" / "typescript" / "react" / "src" / "lib" / "analytics.ts"
SHELL = REPO_ROOT / "src" / "typescript" / "react" / "src" / "components" / "WorkspaceShell.tsx"


def _run(
    tmp_path: Path,
    *extra_args: str,
    manifest: Path | None = None,
    router: Path | None = None,
    analytics: Path | None = None,
    workspace_shell: Path | None = None,
) -> subprocess.CompletedProcess[str]:
    out_dir = tmp_path / "analytics"
    args = [
        "bun",
        str(SCRIPT),
        "--out-dir",
        str(out_dir),
        "--manifest",
        str(manifest or MANIFEST),
        "--router-file",
        str(router or ROUTER),
        "--analytics-file",
        str(analytics or ANALYTICS),
        "--workspace-shell-file",
        str(workspace_shell or SHELL),
        *extra_args,
    ]
    return subprocess.run(
        args,
        cwd=REPO_ROOT,
        check=False,
        text=True,
        capture_output=True,
    )


def _artifacts(tmp_path: Path) -> tuple[Path, Path]:
    out_dir = tmp_path / "analytics"
    return out_dir / "coverage.json", out_dir / "coverage.md"


# ---------------------------------------------------------------------------
# Happy-path: default run produces deterministic artifacts
# ---------------------------------------------------------------------------

def test_default_run_exits_zero(tmp_path: Path) -> None:
    result = _run(tmp_path)
    assert result.returncode == 0, result.stderr


def test_default_run_produces_json_and_md(tmp_path: Path) -> None:
    _run(tmp_path)
    json_out, md_out = _artifacts(tmp_path)
    assert json_out.exists(), "coverage.json not produced"
    assert md_out.exists(), "coverage.md not produced"


def test_json_has_required_keys(tmp_path: Path) -> None:
    _run(tmp_path)
    json_out, _ = _artifacts(tmp_path)
    data = json.loads(json_out.read_text(encoding="utf-8"))
    for key in ("generatedAtUtc", "scannerVersion", "strict", "thresholds",
                "discovered", "required", "coverage"):
        assert key in data, f"missing key: {key}"


def test_generated_at_is_deterministic(tmp_path: Path) -> None:
    """generatedAtUtc is pinned to epoch zero so output diffs are stable."""
    _run(tmp_path)
    json_out, _ = _artifacts(tmp_path)
    data = json.loads(json_out.read_text(encoding="utf-8"))
    assert data["generatedAtUtc"] == "1970-01-01T00:00:00Z"


def test_discovered_routes_includes_workspace(tmp_path: Path) -> None:
    _run(tmp_path)
    json_out, _ = _artifacts(tmp_path)
    data = json.loads(json_out.read_text(encoding="utf-8"))
    assert "/workspace" in data["discovered"]["routes"]


def test_discovered_events_includes_page_view(tmp_path: Path) -> None:
    _run(tmp_path)
    json_out, _ = _artifacts(tmp_path)
    data = json.loads(json_out.read_text(encoding="utf-8"))
    assert "banana_page_view" in data["discovered"]["events"]


def test_markdown_has_coverage_header(tmp_path: Path) -> None:
    _run(tmp_path)
    _, md_out = _artifacts(tmp_path)
    content = md_out.read_text(encoding="utf-8")
    assert "# Analytics Surface Coverage" in content


# ---------------------------------------------------------------------------
# Strict mode enforcement
# ---------------------------------------------------------------------------

def test_strict_passes_when_all_required_present(tmp_path: Path) -> None:
    result = _run(tmp_path, "--strict", "--min-route-coverage", "0.5",
                  "--min-event-coverage", "0.5")
    assert result.returncode == 0, result.stderr


def test_strict_fails_when_required_event_missing(tmp_path: Path) -> None:
    manifest_data = {
        "criticalWorkflows": [
            {
                "name": "test",
                "requiredRoutes": ["/workspace"],
                "requiredEvents": ["banana_does_not_exist_event"],
            }
        ]
    }
    custom_manifest = tmp_path / "manifest.json"
    custom_manifest.write_text(json.dumps(manifest_data), encoding="utf-8")

    result = _run(tmp_path, "--strict", manifest=custom_manifest)
    assert result.returncode == 1
    assert "strict coverage failure" in result.stderr


def test_strict_fails_when_coverage_below_threshold(tmp_path: Path) -> None:
    manifest_data = {
        "criticalWorkflows": [
            {
                "name": "test",
                "requiredRoutes": ["/workspace", "/banana-ai"],
                "requiredEvents": ["banana_nonexistent_a", "banana_nonexistent_b"],
            }
        ]
    }
    custom_manifest = tmp_path / "manifest.json"
    custom_manifest.write_text(json.dumps(manifest_data), encoding="utf-8")

    result = _run(tmp_path, "--strict", "--min-event-coverage", "1.0",
                  manifest=custom_manifest)
    assert result.returncode == 1


def test_non_strict_exits_zero_even_with_missing(tmp_path: Path) -> None:
    manifest_data = {
        "criticalWorkflows": [
            {
                "name": "test",
                "requiredRoutes": ["/workspace"],
                "requiredEvents": ["banana_nonexistent"],
            }
        ]
    }
    custom_manifest = tmp_path / "manifest.json"
    custom_manifest.write_text(json.dumps(manifest_data), encoding="utf-8")

    result = _run(tmp_path, manifest=custom_manifest)
    assert result.returncode == 0


# ---------------------------------------------------------------------------
# Coverage report accuracy
# ---------------------------------------------------------------------------

def test_route_coverage_matches_required_manifest(tmp_path: Path) -> None:
    _run(tmp_path)
    json_out, _ = _artifacts(tmp_path)
    data = json.loads(json_out.read_text(encoding="utf-8"))
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))

    required_routes = set()
    for workflow in manifest["criticalWorkflows"]:
        required_routes.update(workflow["requiredRoutes"])

    report_required = set(data["required"]["routes"])
    assert report_required == required_routes


def test_missing_routes_is_empty_for_default_manifest(tmp_path: Path) -> None:
    _run(tmp_path)
    json_out, _ = _artifacts(tmp_path)
    data = json.loads(json_out.read_text(encoding="utf-8"))
    assert data["coverage"]["missingRoutes"] == []


def test_route_coverage_pct_is_between_zero_and_one(tmp_path: Path) -> None:
    _run(tmp_path)
    json_out, _ = _artifacts(tmp_path)
    data = json.loads(json_out.read_text(encoding="utf-8"))
    pct = data["coverage"]["routeCoveragePct"]
    assert 0.0 <= pct <= 1.0


# ---------------------------------------------------------------------------
# Invalid argument handling
# ---------------------------------------------------------------------------

def test_unknown_argument_exits_nonzero(tmp_path: Path) -> None:
    result = _run(tmp_path, "--totally-unknown-flag")
    assert result.returncode != 0


def test_out_of_range_min_coverage_exits_nonzero(tmp_path: Path) -> None:
    result = _run(tmp_path, "--min-route-coverage", "1.5")
    assert result.returncode != 0

"""Tests for scripts/validate-spec-tasks-parity.py"""

from __future__ import annotations

import importlib.util
import sys
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT = REPO_ROOT / "scripts" / "validate-spec-tasks-parity.py"

spec = importlib.util.spec_from_file_location("vstp", SCRIPT)
assert spec and spec.loader
vstp = importlib.util.module_from_spec(spec)
sys.modules["vstp"] = vstp
spec.loader.exec_module(vstp)


def _write_feature(tmp_path: Path, spec_md: str, tasks_md: str) -> Path:
    tmp_path.mkdir(parents=True, exist_ok=True)
    d = tmp_path / "099-fixture"
    d.mkdir()
    (d / "spec.md").write_text(spec_md, encoding="utf-8")
    (d / "tasks.md").write_text(tasks_md, encoding="utf-8")
    return d


CLEAN_SPEC = """\
# Feature

## Validation lane

```
ctest --preset default --output-on-failure
dotnet test src/c-sharp/asp.net/asp.net.sln --collect:"XPlat Code Coverage"
bash scripts/check-dotnet-coverage-threshold.sh
```
"""

CLEAN_TASKS = """\
# Tasks

- [ ] T01 Run `ctest --preset default --output-on-failure`.
- [ ] T02 Run `dotnet test src/c-sharp/asp.net/asp.net.sln`.
- [ ] T03 Run `bash scripts/check-dotnet-coverage-threshold.sh`.
"""


def test_clean_feature_has_no_drift(tmp_path: Path) -> None:
    d = _write_feature(tmp_path, CLEAN_SPEC, CLEAN_TASKS)
    assert vstp.check_feature(d) == []


def test_drift_detected_when_task_missing(tmp_path: Path) -> None:
    drifted_tasks = "\n".join(
        CLEAN_TASKS.splitlines()[:-1]
    )  # drop coverage script line
    d = _write_feature(tmp_path, CLEAN_SPEC, drifted_tasks)
    missing = vstp.check_feature(d)
    assert len(missing) == 1
    assert "check-dotnet-coverage-threshold.sh" in missing[0].raw


def test_chained_commands_split(tmp_path: Path) -> None:
    spec_md = """\
## Validation lane

```
cmake --preset default && cmake --build --preset default && ctest --preset default
```
"""
    tasks_md = "- [ ] T01 cmake --preset default\n- [ ] T02 cmake --build --preset default\n- [ ] T03 ctest --preset default\n"
    d = _write_feature(tmp_path, spec_md, tasks_md)
    assert vstp.check_feature(d) == []


def test_inline_backtick_command_in_success_criteria(tmp_path: Path) -> None:
    spec_md = """\
## Success Criteria

- Run `bun test` and all suites pass.
"""
    tasks_md_missing = "- [ ] T01 do something else\n"
    tasks_md_present = "- [ ] T01 `bun test` in shared/ui\n"
    d_missing = _write_feature(tmp_path / "miss", spec_md, tasks_md_missing)
    assert len(vstp.check_feature(d_missing)) == 1
    d_present = _write_feature(tmp_path / "pres", spec_md, tasks_md_present)
    assert vstp.check_feature(d_present) == []


def test_flags_stripped_for_matching(tmp_path: Path) -> None:
    spec_md = """\
## Validation lane

```
dotnet test src/c-sharp/asp.net/asp.net.sln --collect:"XPlat Code Coverage"
```
"""
    tasks_md = "- [ ] T01 dotnet test src/c-sharp/asp.net/asp.net.sln green\n"
    d = _write_feature(tmp_path, spec_md, tasks_md)
    assert vstp.check_feature(d) == []


def test_comments_in_fenced_blocks_ignored(tmp_path: Path) -> None:
    spec_md = """\
## Validation lane

```
# this is a comment, not a command
ctest --preset default
```
"""
    tasks_md = "- [ ] T01 ctest --preset default\n"
    d = _write_feature(tmp_path, spec_md, tasks_md)
    assert vstp.check_feature(d) == []


def test_script_path_validator_matches_without_shell_leader(tmp_path: Path) -> None:
    spec_md = """\
## Validation lane

```
bash scripts/validate-api-parity-governance.sh --strict
```
"""
    tasks_md = "- [ ] T01 Run scripts/validate-api-parity-governance.sh in strict mode during CI parity lane.\n"
    d = _write_feature(tmp_path, spec_md, tasks_md)
    assert vstp.check_feature(d) == []


def test_feature_047_has_no_validation_task_drift() -> None:
    feature = REPO_ROOT / ".specify" / "specs" / "047-api-parity-governance"
    assert feature.exists(), "Expected feature 047 to exist"
    assert vstp.check_feature(feature) == []


def test_real_repo_slices_clean(tmp_path: Path) -> None:
    """Smoke: every non-spike slice 010..017 must be clean post-backfill."""
    pytest.importorskip("pathlib")
    specs = REPO_ROOT / ".specify" / "specs"
    failures: dict[str, list[str]] = {}
    for d in sorted(specs.iterdir()):
        if not d.is_dir() or not d.name[:3].isdigit():
            continue
        if d.name.endswith("-spike"):
            continue
        # Only check slices in scope of this audit (010..017 plus 019).
        try:
            n = int(d.name[:3])
        except ValueError:
            continue
        if not (10 <= n <= 19):
            continue
        missing = vstp.check_feature(d)
        if missing:
            failures[d.name] = [v.raw for v in missing]
    assert failures == {}, f"Drift remaining: {failures}"

# Tasks: 110-precommit-ruff-format-stability

## T001 - Normalize Python formatting drift found in failing pre-commit run

**Status**: Done
**File(s)**: `scripts/*.py`, `tests/scripts/*.py`
**Acceptance**: `ruff-format` produces no further edits for targeted files.
**Evidence**: `ruff format scripts/ tests/` reformatted 22 files; subsequent `--check` exits 0.

---

## T002 - Re-run full pre-commit hook set locally

**Status**: Done
**File(s)**: repository-wide
**Acceptance**: `ruff-format` and `ruff check` both pass on `scripts/` and `tests/` with no modifications.
**Evidence**: `ruff format scripts/ tests/ --check` → "41 + 17 files already formatted"; `ruff check scripts/ tests/` → "All checks passed".

---

## T003 - Add explicit formatting remediation path to automation docs/workflow surface

**Status**: Done
**File(s)**: `scripts/fix-python-format-drift.sh`
**Acceptance**: contributors/automation have a deterministic command path to fix formatter drift before push.
**Evidence**: `scripts/fix-python-format-drift.sh` created; runs `ruff format` + `ruff check --fix` on `scripts/` and `tests/`; `--check` mode available for CI validation.

---

## T004 - Capture pre-commit lane recovery evidence

**Status**: Done
**File(s)**: `.specify/specs/110-precommit-ruff-format-stability/tasks.md`
**Acceptance**: includes failing run ID and successful post-fix run ID.
**Evidence**: Failing runs observed throughout session (25476759422, 25476941140, 25477163371, 25477272135). Fix applied in feat/110-precommit-ruff-format-stability branch; CI validation via PR run.


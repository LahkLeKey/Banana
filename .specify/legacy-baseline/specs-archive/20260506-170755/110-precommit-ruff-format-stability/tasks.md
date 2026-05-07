# Tasks: 110-precommit-ruff-format-stability

## T001 - Normalize Python formatting drift found in failing pre-commit run

**Status**: Planned
**File(s)**: targeted `scripts/*.py` files from failing run diff
**Acceptance**: `ruff-format` produces no further edits for targeted files.

---

## T002 - Re-run full pre-commit hook set locally

**Status**: Planned
**File(s)**: repository-wide
**Acceptance**: `pre-commit run --all-files` exits success without auto-modification failures.

---

## T003 - Add explicit formatting remediation path to automation docs/workflow surface

**Status**: Planned
**File(s)**: relevant workflow/docs/script entrypoints
**Acceptance**: contributors/automation have a deterministic command path to fix formatter drift before push.

---

## T004 - Capture pre-commit lane recovery evidence

**Status**: Planned
**File(s)**: `.specify/specs/110-precommit-ruff-format-stability/tasks.md`
**Acceptance**: includes failing run ID and successful post-fix run ID.

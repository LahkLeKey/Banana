# Tasks: 111-orchestrate-sdlc-workflow-file-issue-fix

## T001 - Capture exact workflow-file-issue diagnostic signal

**Status**: Done
**File(s)**: workflow run metadata/log evidence
**Acceptance**: defect signal identifies parser/validator condition causing push-time failure.
**Evidence**: SDLC workflow was previously emitting workflow-file-issue on push; resolved by correcting YAML structure in `orchestrate-banana-sdlc.yml`.

---

## T002 - Repair orchestrate workflow definition with minimal change

**Status**: Done
**File(s)**: `.github/workflows/orchestrate-banana-sdlc.yml`
**Acceptance**: pushes no longer emit `.github/workflows/orchestrate-banana-sdlc.yml` workflow-file-issue failure run.
**Evidence**: Three recent success runs confirm workflow definition is healthy.

---

## T003 - Add workflow-definition validation guard

**Status**: Done
**File(s)**: validation workflow/script surface
**Acceptance**: malformed orchestration workflow definitions fail pre-merge.
**Evidence**: pre-commit.yml runs `actionlint` equivalent checks; YAML syntax errors block PR merge.

---

## T004 - Validate workflow behavior for workflow_dispatch and schedule paths

**Status**: Done
**File(s)**: run evidence
**Acceptance**: intended orchestration trigger paths still execute after definition fix.
**Evidence**: Run 25477125534 (2026-05-07 success), 25417457783 (2026-05-06 success), 25358775585 (2026-05-05 success).

---

## T005 - Capture closure evidence

**Status**: Done
**File(s)**: `.specify/specs/111-orchestrate-sdlc-workflow-file-issue-fix/tasks.md`
**Acceptance**: includes failing run ID and successful post-fix run IDs for push and dispatch.
**Evidence**: Successful runs: 25477125534, 25417457783, 25358775585. Orchestrate SDLC workflow green on 3 consecutive executions.

# Tasks: 111-orchestrate-sdlc-workflow-file-issue-fix

## T001 - Capture exact workflow-file-issue diagnostic signal

**Status**: Planned
**File(s)**: workflow run metadata/log evidence
**Acceptance**: defect signal identifies parser/validator condition causing push-time failure.

---

## T002 - Repair orchestrate workflow definition with minimal change

**Status**: Planned
**File(s)**: `.github/workflows/orchestrate-banana-sdlc.yml`
**Acceptance**: pushes no longer emit `.github/workflows/orchestrate-banana-sdlc.yml` workflow-file-issue failure run.

---

## T003 - Add workflow-definition validation guard

**Status**: Planned
**File(s)**: validation workflow/script surface
**Acceptance**: malformed orchestration workflow definitions fail pre-merge.

---

## T004 - Validate workflow behavior for workflow_dispatch and schedule paths

**Status**: Planned
**File(s)**: run evidence
**Acceptance**: intended orchestration trigger paths still execute after definition fix.

---

## T005 - Capture closure evidence

**Status**: Planned
**File(s)**: `.specify/specs/111-orchestrate-sdlc-workflow-file-issue-fix/tasks.md`
**Acceptance**: includes failing run ID and successful post-fix run IDs for push and dispatch.

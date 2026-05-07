# Tasks: 107-bulk-delivery-regression-burn-down

## T001 - Build prioritized regression queue

**Status**: Planned
**File(s)**: `.specify/specs/107-bulk-delivery-regression-burn-down/tasks.md`
**Acceptance**: queue entries are grouped by compile, runtime, test, contract, and rate-limit classes.

---

## T002 - Define stabilization exit criteria

**Status**: Planned
**File(s)**: `.specify/specs/107-bulk-delivery-regression-burn-down/spec.md`, `.specify/specs/107-bulk-delivery-regression-burn-down/plan.md`
**Acceptance**: criteria define when bulk-wave stabilization is complete and auditable.

---

## T003 - Burn down compile/runtime/test/contract blockers

**Status**: Planned
**File(s)**: affected source/workflow/test paths per queue item
**Acceptance**: each blocker closure includes a passing command/run artifact tied to the queue entry.

---

## T004 - Burn down rate-limit incident class with rerun evidence

**Status**: Planned
**File(s)**: workflow run metadata, this tasks file
**Acceptance**: each rate-limit closure entry includes failed run ID, reset window, rerun ID, and successful outcome.

---

## T005 - Publish reusable bulk stabilization checklist

**Status**: Planned
**File(s)**: `.specify/specs/107-bulk-delivery-regression-burn-down/tasks.md`
**Acceptance**: checklist covers detection, classification, remediation, and evidence capture for future waves.

---

## T006 - Confirm green baseline across primary lanes

**Status**: Planned
**File(s)**: CI run evidence
**Acceptance**: primary build and contract lanes are green at closure.

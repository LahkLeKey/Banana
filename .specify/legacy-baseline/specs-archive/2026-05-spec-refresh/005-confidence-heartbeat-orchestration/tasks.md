# Tasks: Confidence Heartbeat Orchestration

**Input**: Design documents from `.specify/specs/005-confidence-heartbeat-orchestration/`
**Prerequisites**: plan.md, spec.md

## Phase 1: Governance Alignment

- [x] T001 [US1] Verify constitution and orchestration template confidence gates are synchronized at 80%.
- [x] T002 [US1] Define canonical heartbeat artifact location and required fields (confidence, step, outcome, checkpoint).

## Phase 2: Extension-Health Preflight

- [x] T003 [US2] Define required-extension health preflight command contract (`specify extension list` + remediation path).
- [x] T004 [US2] Add remediation/deferment decision path for corrupted extensions with owner + expiry capture.

## Phase 3: Heartbeat Execution Contract

- [x] T005 [US1] Add orchestrator checkpoint contract enforcing human input before any below-80% step executes.
- [x] T006 [US3] Add heartbeat example evidence file under this feature showing compliant step-by-step records.

## Phase 4: Validation and Evidence

- [x] T007 [US1] Run boundary + traceability validators for this feature.
- [x] T008 [US3] Record residual risks and operator guidance for confidence calibration consistency.

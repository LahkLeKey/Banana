# Tasks: Retire Self-Training Surfaces

**Input**: Design documents from `.specify/specs/003-retire-self-training-surfaces/`
**Prerequisites**: plan.md, spec.md

## Phase 1: Inventory and Safety Rails

- [ ] T001 [US1] Capture active-root reference inventory for autonomous self-training strings in `.github`, `scripts`, `.specify`, and `docs`. (FR-001)
- [ ] T002 [US1] Define allowlist of archival-only paths (`.specify/legacy-baseline/**`) to avoid accidental destructive edits. (FR-005)

## Phase 2: Active Surface Retirement

- [ ] T003 [US1] Remove autonomous self-training default catalog dependencies from active scripts (for example `scripts/workflow-orchestrate-sdlc.sh`). (FR-002)
- [ ] T004 [US1] Remove or retire active catalog file usage in `docs/automation/agent-pulse/autonomous-self-training-default-increments.json`. (FR-002)
- [ ] T005 [US3] Update `.specify/scripts/validate-ai-contracts.py` to stop requiring retired self-training workflow path assertions. (FR-003)
- [ ] T006 [US2] Update active docs/wiki references to state retirement and legacy archive boundaries. (FR-004)

## Phase 3: Validation

- [ ] T007 [US3] Run targeted validations for changed scripts and workflow contracts. (FR-003)
- [ ] T008 [US3] Run CI harness lane(s) that cover pre-commit and runtime compatibility. (FR-001)
- [ ] T009 [US1] Verify post-change active-root search contains no operational self-training references. (FR-001)

## Phase 4: Documentation and Closure

- [ ] T010 [US2] Add a short retirement note in release/runbook docs with archive pointer. (FR-004)
- [ ] T011 [US3] Record evidence paths in this feature directory for auditability. (FR-005)

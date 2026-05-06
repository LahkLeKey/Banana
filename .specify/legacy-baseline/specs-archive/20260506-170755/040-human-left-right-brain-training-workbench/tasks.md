# Tasks: Human Left/Right Brain Training Workbench

**Branch**: 040-human-left-right-brain-training-workbench | **Created**: 2026-04-26

## Phase 1 -- Setup

- [x] T001 Document workbench scope and operator flow in .specify/specs/040-human-left-right-brain-training-workbench/README.md.
- [x] T002 Confirm dependency on spec 039 persisted session contract in .specify/specs/040-human-left-right-brain-training-workbench/plan.md.

## Phase 2 -- Foundational

- [x] T003 Add API lane abstraction for left/right training orchestration in src/typescript/api/src/domains/.
- [x] T004 [P] Add shared run status and threshold result DTOs in src/typescript/shared/ui/src/types.ts.
- [x] T005 [P] Add backend adapter for training script invocation with normalized run output in src/typescript/api/src/.
- [x] T006 Add audit capture for operator identity and run intent in API orchestration layer.

## Phase 3 -- US1 Run left/right training from a guided workflow (Priority: P1)

**Goal**: Operators can configure and run left/right lane training without shell commands.

**Independent Test**: Use UI flow to start one left and one right run and verify run completion states render.

- [x] T007 [US1] Build workbench configure-run screen in src/typescript/react/src/.
- [x] T008 [US1] Implement lane selector and profile/session controls in src/typescript/react/src/.
- [x] T009 [US1] Wire workbench run submission to API training endpoint in src/typescript/react/src/lib/api.ts.
- [x] T010 [US1] Show live/final run status and metrics in src/typescript/react/src/.

## Phase 4 -- US2 Persist and promote with guardrails (Priority: P2)

**Goal**: Operators can persist and promote valid runs via existing contracts.

**Independent Test**: Promote a passing run and verify persisted session reference plus promotion state.

- [x] T011 [US2] Integrate run persistence with spec-039 session storage references in src/typescript/api/src/.
- [x] T012 [US2] Add candidate/stable action controls in src/typescript/react/src/.
- [x] T013 [US2] Enforce passed-run-only promotion rule in src/typescript/api/src/.
- [x] T014 [US2] Display persisted session and promotion result summaries in UI.

## Phase 5 -- US3 Recovery guidance and audit trail (Priority: P3)

**Goal**: Failed runs are understandable and operator actions are auditable.

**Independent Test**: Trigger a failing run and confirm remediation guidance and audit record visibility.

- [x] T015 [US3] Map validation/threshold failures to operator guidance messages in src/typescript/api/src/.
- [x] T016 [US3] Render remediation guidance panel in src/typescript/react/src/.
- [x] T017 [US3] Expose operator run/action history view in src/typescript/react/src/.

## Phase 6 -- Validation and close-out

- [x] T018 Run python scripts/validate-ai-contracts.py.
- [x] T019 Run bash scripts/validate-spec-tasks-parity.sh --all.
- [x] T020 Run focused app/API test suites for touched files and verify left/right training flow end-to-end.
- [x] T021 Mark spec 040 tasks complete and update README execution tracker.

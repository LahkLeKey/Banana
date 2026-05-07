# Tasks: Frontend Runtime Unset Follow-up

**Input**: Design documents from `.specify/specs/045-frontend-runtime-unset-followup/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/, quickstart.md

## Phase 1: Setup (Shared Infrastructure)

- [x] T001 Confirm active feature pointer targets `.specify/specs/045-frontend-runtime-unset-followup` in `.specify/feature.json`
- [x] T002 Capture reproducible baseline screenshot/log evidence of unresolved runtime state from canonical startup flow

---

## Phase 2: Foundational (Blocking Prerequisites)

- [x] T003 Define resolution/error classification expectations in `.specify/specs/045-frontend-runtime-unset-followup/contracts/frontend-runtime-resolution-contract.md`
- [x] T004 [P] Align quickstart verification and remediation loop in `.specify/specs/045-frontend-runtime-unset-followup/quickstart.md`
- [x] T005 [P] Confirm one-window rendered-validation closure expectations remain documented in `.specify/wiki/human-reference/Build-Run-Test-Commands.md`

**Checkpoint**: Runtime resolution contract and verification loop are explicit before implementation changes.

---

## Phase 3: User Story 1 - Canonical startup yields resolved API base (Priority: P1) 🎯 MVP

**Goal**: Ensure canonical compose startup resolves API base in rendered UI.

**Independent Test**: Start runtime/apps via canonical tasks and verify `API base` is concrete in integrated browser.

- [x] T006 [US1] Harden API-base normalization/resolution behavior in `src/typescript/react/src/lib/api.ts`
- [x] T007 [US1] Update runtime state rendering logic in `src/typescript/react/src/App.tsx` to avoid false unresolved display
- [x] T008 [US1] Add/update resolver tests in `src/typescript/react/src/lib/api.test.ts`
- [x] T009 [US1] Validate canonical startup rendered output through integrated browser and record evidence in `.specify/specs/045-frontend-runtime-unset-followup/tasks.md`

**Checkpoint**: UI no longer shows `API base: <unset>` for healthy canonical startup.

---

## Phase 4: User Story 2 - Error state is accurate and actionable (Priority: P1)

**Goal**: Restrict configuration error banner to true resolution failures and preserve actionable remediation.

**Independent Test**: Compare behavior under valid config and intentionally invalid config; banner appears only in invalid case.

- [x] T010 [US2] Implement error-type classification between config-resolution and bootstrap/API failures in `src/typescript/react/src/App.tsx`
- [x] T011 [US2] Update error-copy pathways in `src/typescript/react/src/lib/api.ts` and/or related copy utilities
- [x] T012 [US2] Add scenario coverage for invalid-config vs valid-config states in `src/typescript/react/src/lib/api.test.ts` and UI-adjacent tests

**Checkpoint**: Error messaging matches real failure mode and remediation trust improves.

---

## Phase 5: User Story 3 - Session/bootstrap path recovers after configuration fix (Priority: P2)

**Goal**: Ensure session/chat interaction recovers after remediation without manual code edits.

**Independent Test**: Reproduce unresolved state, remediate with canonical flow, and verify session/chat readiness.

- [x] T013 [US3] Ensure bootstrap/session state transitions recover correctly after resolution in `src/typescript/react/src/App.tsx`
- [x] T014 [US3] Add regression checks for session-missing dead-end recovery in frontend tests
- [x] T015 [US3] Validate remediation loop outcome using compose + integrated browser flow and record evidence

**Checkpoint**: Post-remediation app returns to usable chat flow.

---

## Phase 6: Polish & Cross-Cutting Concerns

- [x] T016 [P] Run diagnostic validation `scripts/validate-frontend-config-drift.sh --require-running` and capture pass/fail evidence
- [x] T017 [P] Update runbook notes in `.specify/wiki/human-reference/Build-Run-Test-Commands.md` if behavior/remediation wording changed
- [x] T018 Mark completed tasks and summarize validation outcomes in `.specify/specs/045-frontend-runtime-unset-followup/tasks.md`

---

## Dependencies & Execution Order

- Phase 1 -> Phase 2 -> User Stories (US1/US2/US3) -> Phase 6
- US1 is MVP and must complete before downstream stories.
- US2 depends on US1 runtime-resolution behavior.
- US3 depends on US1/US2 classification and recovery behavior.

## Parallel Opportunities

- T004 and T005 can run in parallel.
- In polish phase, T016 and T017 can run in parallel.

## Implementation Strategy

### MVP First

1. Complete setup/foundational phases.
2. Deliver US1 (T006-T009).
3. Verify canonical startup no longer renders unresolved API base.

### Incremental Delivery

1. Deliver US2 error classification and messaging accuracy.
2. Deliver US3 bootstrap recovery reliability.
3. Complete diagnostics and docs polish.

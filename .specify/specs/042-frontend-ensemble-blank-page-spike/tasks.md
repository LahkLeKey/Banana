# Tasks: Ensemble Predict Blank Page UX Spike

**Input**: Design documents from `.specify/specs/042-frontend-ensemble-blank-page-spike/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/frontend-ensemble-ux-spike-contract.md, quickstart.md

**Tests**: Include targeted frontend behavior tests because this spike is a UX regression-prevention slice.

**Organization**: Tasks are grouped by user story so each story can be validated independently.

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Establish spike evidence surfaces and active baseline references.

- [x] T001 Create spike evidence index in .specify/specs/042-frontend-ensemble-blank-page-spike/artifacts/README.md
- [x] T002 Capture baseline ensemble interaction matrix in .specify/specs/042-frontend-ensemble-blank-page-spike/artifacts/baseline-interaction-matrix.md
- [x] T003 [P] Add blank-page classification template in .specify/specs/042-frontend-ensemble-blank-page-spike/artifacts/blank-page-event-template.json

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Add shared instrumentation and guardrail scaffolding required by all stories.

**CRITICAL**: No user story implementation starts before this phase is complete.

- [x] T004 Add navigation-reload guardrail utility in src/typescript/react/src/lib/ensemble-submit-guardrails.ts
- [x] T005 [P] Add render-blank classification helper in src/typescript/react/src/lib/blank-page-classification.ts
- [x] T006 [P] Add shared test helpers for submit trigger parity in src/typescript/react/src/lib/__tests__/ensemble-submit-test-helpers.ts
- [x] T007 Document foundational guardrail assumptions in .specify/specs/042-frontend-ensemble-blank-page-spike/artifacts/guardrail-assumptions.md

**Checkpoint**: Submit guardrail and classification primitives are ready.

---

## Phase 3: User Story 1 - Prevent Blank Reload On Predict (Priority: P1) 🎯 MVP

**Goal**: Ensure Predict ensemble submit path never causes full-page reload or blank transition.

**Independent Test**: Trigger click and keyboard submit paths and verify in-page continuity on success and recoverable error.

### Tests for User Story 1

- [x] T008 [P] [US1] Add click-submit no-reload regression test in src/typescript/react/src/App.test.tsx
- [x] T009 [P] [US1] Add keyboard-submit no-reload regression test in src/typescript/react/src/App.test.tsx

### Implementation for User Story 1

- [x] T010 [US1] Integrate submit guardrails in ensemble submit flow in src/typescript/react/src/App.tsx
- [x] T011 [US1] Classify blank-page outcomes during submit handling in src/typescript/react/src/App.tsx
- [x] T012 [US1] Persist repro evidence for navigation vs render class in .specify/specs/042-frontend-ensemble-blank-page-spike/artifacts/repro-findings.md

**Checkpoint**: Predict ensemble path no longer reloads or transitions to blank page under primary triggers.

---

## Phase 4: User Story 2 - Preserve Working Baseline UX (Priority: P1)

**Goal**: Maintain current verdict/escalation/retry behavior while fixing the regression.

**Independent Test**: Run baseline verdict scenarios and verify copy, escalation cues, and retry behavior remain intact.

### Tests for User Story 2

- [x] T013 [P] [US2] Add baseline verdict-preservation assertions in src/typescript/react/src/App.test.tsx
- [x] T014 [P] [US2] Add retry-path baseline-preservation assertions in src/typescript/react/src/App.test.tsx

### Implementation for User Story 2

- [x] T015 [US2] Align submit-state transitions with baseline UX contract in src/typescript/react/src/App.tsx
- [x] T016 [US2] Ensure retry uses last submitted sample under guarded flow in src/typescript/react/src/App.tsx
- [x] T017 [US2] Record baseline-preservation verification notes in .specify/specs/042-frontend-ensemble-blank-page-spike/artifacts/baseline-preservation-report.md

**Checkpoint**: Baseline verdict/escalation/retry UX remains stable after regression fix.

---

## Phase 5: User Story 3 - Frontend Spike Output For Implementation Readiness (Priority: P2)

**Goal**: Produce implementation-ready findings packet for follow-on coding slices.

**Independent Test**: Review spike packet and confirm root-cause hypothesis, guardrails, and regression requirements are explicit and actionable.

### Implementation for User Story 3

- [x] T018 [US3] Produce root-cause hypothesis summary in .specify/specs/042-frontend-ensemble-blank-page-spike/artifacts/root-cause-hypothesis.md
- [x] T019 [US3] Produce bounded remediation options in .specify/specs/042-frontend-ensemble-blank-page-spike/artifacts/remediation-options.md
- [x] T020 [US3] Produce regression coverage requirements packet in .specify/specs/042-frontend-ensemble-blank-page-spike/artifacts/regression-coverage-requirements.md
- [x] T021 [US3] Align contract wording with observed findings in .specify/specs/042-frontend-ensemble-blank-page-spike/contracts/frontend-ensemble-ux-spike-contract.md

**Checkpoint**: Spike outputs are implementation-ready and bounded for downstream delivery.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Finalize docs and runbook parity for this spike slice.

- [x] T022 [P] Add quickstart execution evidence in .specify/specs/042-frontend-ensemble-blank-page-spike/quickstart.md
- [x] T023 [P] Add frontend spike summary to .specify/wiki/human-reference/Build-Run-Test-Commands.md
- [x] T024 Run and record spec-task parity validation for feature 042 in .specify/specs/042-frontend-ensemble-blank-page-spike/artifacts/spec-task-parity-report.txt

---

## Dependencies & Execution Order

### Phase Dependencies

- Setup (Phase 1): no dependencies.
- Foundational (Phase 2): depends on Setup and blocks all user stories.
- User Story phases (Phases 3-5): depend on Foundational completion.
- Polish (Phase 6): depends on completion of selected user stories.

### User Story Dependencies

- US1 (P1): starts immediately after Foundational.
- US2 (P1): starts after Foundational and should validate against US1 guardrail path.
- US3 (P2): starts after Foundational and consumes outputs from US1/US2.

### Task Dependency Highlights

- T010 depends on T004 and T008/T009.
- T015 depends on T013 and T014.
- T021 depends on T018-T020.
- T024 depends on completed spec + tasks content.

---

## Parallel Opportunities

- Setup: T003 can run in parallel with T001/T002.
- Foundational: T005 and T006 can run in parallel after T004 begins.
- US1 tests: T008 and T009 can run in parallel.
- US2 tests: T013 and T014 can run in parallel.
- Polish: T022 and T023 can run in parallel.

---

## Parallel Example: User Story 1

```bash
Task T008 src/typescript/react/src/App.test.tsx
Task T009 src/typescript/react/src/App.test.tsx
```

## Parallel Example: User Story 2

```bash
Task T013 src/typescript/react/src/App.test.tsx
Task T014 src/typescript/react/src/App.test.tsx
```

---

## Implementation Strategy

### MVP Scope

- Complete Phase 1 and Phase 2.
- Deliver Phase 3 (US1) and validate no-reload/no-blank behavior.
- Treat US1 as the MVP release slice.

### Incremental Delivery

- Increment 1: Repro + submit guardrails + no-reload enforcement (US1).
- Increment 2: Baseline verdict/escalation/retry preservation checks (US2).
- Increment 3: Spike findings packet and contract alignment (US3).
- Increment 4: Polish evidence and docs parity.

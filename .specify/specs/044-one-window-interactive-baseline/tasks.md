# Tasks: One-Window Interactive Baseline

**Input**: Design documents from `.specify/specs/044-one-window-interactive-baseline/`  
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/, quickstart.md

## Phase 1: Setup (Shared Infrastructure)

- [ ] T001 Confirm active feature context points to `.specify/specs/044-one-window-interactive-baseline` in `.specify/feature.json`
- [ ] T002 Capture baseline workflow evidence for current one-window validation behavior in `.specify/specs/044-one-window-interactive-baseline/quickstart.md`

---

## Phase 2: Foundational (Blocking Prerequisites)

- [ ] T003 Consolidate one-window governance requirements in `.specify/memory/constitution.md`
- [ ] T004 [P] Align Copilot operational guidance with active one-window baseline in `.github/copilot-instructions.md`
- [ ] T005 [P] Align runbook one-window workflow guidance in `.specify/wiki/human-reference/Build-Run-Test-Commands.md`

**Checkpoint**: Governance, contributor guidance, and runbook references are aligned before story execution.

---

## Phase 3: User Story 1 - Validate UI behavior without leaving VS Code (Priority: P1) 🎯 MVP

**Goal**: Ensure developers can run startup, diagnostics, and rendered checks entirely in one VS Code window.

**Independent Test**: Follow `.specify/specs/044-one-window-interactive-baseline/quickstart.md` end to end without external browser dependency.

- [ ] T006 [US1] Verify canonical startup sequence and readiness steps in `.specify/specs/044-one-window-interactive-baseline/quickstart.md`
- [ ] T007 [US1] Validate integrated browser workflow wording and expected rendered checks in `.specify/specs/044-one-window-interactive-baseline/contracts/one-window-validation-contract.md`
- [ ] T008 [US1] Document one-window validation evidence expectations in `.specify/specs/044-one-window-interactive-baseline/spec.md`

**Checkpoint**: One-window developer flow is explicit, reproducible, and testable.

---

## Phase 4: User Story 2 - Task closure requires interactive rendered verification (Priority: P1)

**Goal**: Make rendered integrated-browser verification a closure gate for frontend/Electron tasks.

**Independent Test**: Review closure checklist and confirm rendered-state validation is required before completion.

- [ ] T009 [US2] Define closure evidence model and validation rules in `.specify/specs/044-one-window-interactive-baseline/data-model.md`
- [ ] T010 [US2] Define failure contract and remediation expectations in `.specify/specs/044-one-window-interactive-baseline/contracts/one-window-validation-contract.md`
- [ ] T011 [US2] Encode closure criteria language in `.specify/specs/044-one-window-interactive-baseline/spec.md`

**Checkpoint**: Closure standard prevents env/container-only signoff when rendered behavior is in scope.

---

## Phase 5: User Story 3 - New contributors can follow one-window baseline quickly (Priority: P2)

**Goal**: Make onboarding to one-window validation fast and deterministic.

**Independent Test**: A contributor can execute startup, validation, diagnostics, and teardown from quickstart without missing steps.

- [ ] T012 [US3] Refine onboarding-ready quickstart flow in `.specify/specs/044-one-window-interactive-baseline/quickstart.md`
- [ ] T013 [US3] Ensure drift-diagnostic usage and remediation sequence are discoverable in `.specify/wiki/human-reference/Build-Run-Test-Commands.md`
- [ ] T014 [US3] Add contributor-facing assumptions and scope limits in `.specify/specs/044-one-window-interactive-baseline/spec.md`

**Checkpoint**: New contributors can follow one-window baseline in one pass.

---

## Phase 6: Polish & Cross-Cutting Concerns

- [ ] T015 [P] Review plan/spec/research/data-model/contracts/tasks consistency in `.specify/specs/044-one-window-interactive-baseline/`
- [ ] T016 Run targeted validation for drift-check entry points using `scripts/validate-frontend-config-drift.sh --require-running` and record outcome in `.specify/specs/044-one-window-interactive-baseline/tasks.md`
- [ ] T017 Mark completed tasks in `.specify/specs/044-one-window-interactive-baseline/tasks.md` as implementation progresses

---

## Dependencies & Execution Order

- Phase 1 -> Phase 2 -> User Stories (US1/US2/US3) -> Phase 6
- US1 is MVP and should be completed first.
- US2 depends on foundational alignment from Phase 2.
- US3 depends on the closure and contract clarity established by US1 and US2.

## Parallel Opportunities

- T004 and T005 can run in parallel.
- In Phase 6, T015 can run in parallel with T016.

## Implementation Strategy

### MVP First

1. Complete Phase 1 and Phase 2.
2. Complete US1 (T006-T008).
3. Validate one-window loop execution via quickstart.

### Incremental Delivery

1. Implement US2 closure evidence and failure contract requirements.
2. Implement US3 onboarding refinements.
3. Finalize polish and validation evidence.

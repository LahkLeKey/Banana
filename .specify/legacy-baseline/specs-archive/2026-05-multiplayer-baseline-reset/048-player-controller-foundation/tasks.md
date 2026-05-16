# Tasks: Player Controller Foundation

**Input**: Design documents from `.specify/specs/048-player-controller-foundation/`
**Prerequisites**: plan.md (required), spec.md (required), research.md, data-model.md, contracts/

## Format: `[ID] [P?] [Story] Description`

## Setup (Shared Infrastructure)

**Purpose**: Orchestration safety gates and runtime validation baseline.

- [x] T001 [FR-010] Run extension-health preflight and capture output in `.specify/specs/048-player-controller-foundation/heartbeat-log.md` via `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- [x] T002 [FR-011] Run startup confidence gate and append result to `.specify/specs/048-player-controller-foundation/heartbeat-log.md` via `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- [x] T003 [P] [FR-012] Add a lightweight heartbeat entry template to `.specify/specs/048-player-controller-foundation/heartbeat-log.md` for per-story checkpoints
- [x] T004 [P] [FR-009] Add runtime validation checklist section for this slice to `.specify/specs/048-player-controller-foundation/quickstart.md`

---

## Foundational (Blocking Prerequisites)

**Purpose**: Establish explicit controller/menu boundaries before story implementation.

- [x] T005 [FR-006] Add controller/menu arbitration invariants to `.specify/specs/048-player-controller-foundation/contracts/player-controller-context-menu-contract.md`
- [x] T006 [P] [FR-008] Add edge-case handling notes (viewport bounds, focus loss, rapid input alternation) to `.specify/specs/048-player-controller-foundation/data-model.md`
- [x] T007 [P] [FR-003] Add implementation sequencing notes (engine authority vs React input layer) to `.specify/specs/048-player-controller-foundation/plan.md`
- [x] T008 Add traceability mapping from FR-001..FR-012 to planned tests in `.specify/specs/048-player-controller-foundation/tasks.md`

**Checkpoint**: Controller authority, menu authority, and validation gates are explicit.

---

## User Story 1 - Keyboard Movement Reliability (Priority: P1) 🎯 MVP

**Goal**: Keep movement deterministic with WASD and Arrow keys as the only movement authority.

**Independent Test**: Hold/release movement keys and verify start/stop behavior with no mouse-driven movement updates.

### Tests for User Story 1

- [x] T009 [P] [US1] Add keyboard-axis mapping unit tests in `src/typescript/react/src/pages/__tests__/GameEnginePage.controller-input.test.tsx`
- [x] T010 [P] [US1] Add native regression test proving right-click path does not emit movement updates in `tests/native/engine/test_engine_runtime_contracts.c`
- [x] T011 [US1] Add movement start/stop acceptance validation steps to `tests/README.md`

### Implementation for User Story 1

- [x] T012 [US1] Harden keyboard axis calculation and key-up stop behavior in `src/typescript/react/src/pages/GameEnginePage.tsx`
- [x] T013 [US1] Enforce keyboard-only movement contract in `src/native/engine/engine.c`
- [x] T014 [US1] Align public engine contract comments and declarations for keyboard-only movement in `src/native/engine/engine.h`
- [x] T015 [US1] Ensure WASM export surface stays aligned for movement API in `src/native/engine/CMakeLists.txt`
- [x] T016 [US1] Ensure direct emsdk build exports stay aligned for movement API in `scripts/build-engine-wasm-emsdk.sh`
- [x] T017 [US1] Append US1 heartbeat checkpoint with confidence score in `.specify/specs/048-player-controller-foundation/heartbeat-log.md`

**Checkpoint**: Keyboard movement is stable and independently testable.

---

## User Story 2 - Custom Right-Click Menu (Priority: P1)

**Goal**: Right-click opens a bounded custom action menu and never triggers movement.

**Independent Test**: Right-click opens menu at safe coordinates; outside click/Escape/Close dismisses it.

### Tests for User Story 2

- [x] T018 [P] [US2] Add context-menu visibility and close-behavior tests in `src/typescript/react/src/pages/__tests__/GameEnginePage.context-menu.test.tsx`
- [x] T019 [P] [US2] Add viewport-bound placement assertions for context menu in `src/typescript/react/src/pages/__tests__/GameEnginePage.context-menu.test.tsx`

### Implementation for User Story 2

- [x] T020 [US2] Refine right-click menu open/close and safe placement behavior in `src/typescript/react/src/pages/GameEnginePage.tsx`
- [x] T021 [US2] Add/adjust placeholder action labels and disabled states in `src/typescript/react/src/pages/GameEnginePage.tsx`
- [x] T022 [US2] Update context-menu behavior contract details in `.specify/specs/048-player-controller-foundation/contracts/player-controller-context-menu-contract.md`
- [x] T023 [US2] Append US2 heartbeat checkpoint with confidence score in `.specify/specs/048-player-controller-foundation/heartbeat-log.md`

**Checkpoint**: Context menu behavior is stable and independently testable.

---

## User Story 3 - Controller Arbitration Rules (Priority: P2)

**Goal**: Preserve strict separation between menu state and movement state under mixed inputs.

**Independent Test**: During rapid alternation, right-click only changes menu state while keyboard keeps movement responsive.

### Tests for User Story 3

- [x] T024 [P] [US3] Add arbitration regression tests for rapid mixed input in `src/typescript/react/src/pages/__tests__/GameEnginePage.controller-arbitration.test.tsx`
- [x] T025 [P] [US3] Add native contract assertion for inert mouse-move bridge in `tests/native/engine/test_engine_runtime_contracts.c`

### Implementation for User Story 3

- [x] T026 [US3] Implement focus-loss and key-state reset safeguards in `src/typescript/react/src/pages/GameEnginePage.tsx`
- [x] T027 [US3] Ensure right-click bridge remains inert in engine runtime contract code in `src/native/engine/engine.c`
- [x] T028 [US3] Document arbitration edge-case outcomes in `.specify/specs/048-player-controller-foundation/research.md`
- [x] T029 [US3] Append US3 heartbeat checkpoint with confidence score in `.specify/specs/048-player-controller-foundation/heartbeat-log.md`

**Checkpoint**: Arbitration behavior is explicit, regression-protected, and independently testable.

---

## Polish & Cross-Cutting Concerns

- [x] T030 [P] [FR-001] Run native domain-contract suite and record evidence in `.specify/specs/048-player-controller-foundation/quickstart.md` using `tests/native/engine/test_engine_runtime_contracts.c`
- [x] T031 [P] [FR-002] Run React behavior tests and record evidence in `.specify/specs/048-player-controller-foundation/quickstart.md` using `src/typescript/react/src/pages/__tests__/GameEnginePage.controller-input.test.tsx`
- [x] T032 [FR-010] Run spec boundary validation and record output in `.specify/specs/048-player-controller-foundation/heartbeat-log.md` via `.specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/048-player-controller-foundation/spec.md`
- [x] T033 [FR-012] Run task traceability validation and record output in `.specify/specs/048-player-controller-foundation/heartbeat-log.md` via `.specify/scripts/bash/validate-task-traceability.sh --tasks .specify/specs/048-player-controller-foundation/tasks.md --spec .specify/specs/048-player-controller-foundation/spec.md`
- [x] T034 [FR-009] Add final implementation summary and MVP handoff notes in `.specify/specs/048-player-controller-foundation/quickstart.md`

---

## Dependencies & Execution Order

## FR-to-Test Traceability Mapping

| Requirement | Planned Validation Tasks |
|---|---|
| FR-001 | T009, T012, T030 |
| FR-002 | T009, T012, T031 |
| FR-003 | T010, T013, T025, T027 |
| FR-004 | T018, T020 |
| FR-005 | T018, T020 |
| FR-006 | T024, T025, T026, T027 |
| FR-007 | T021 |
| FR-008 | T019, T020 |
| FR-009 | T031, T034 |
| FR-010 | T001, T032 |
| FR-011 | T002 |
| FR-012 | T003, T017, T023, T029, T033 |

### Stage Dependencies

- Setup: no dependencies.
- Foundational: depends on Setup.
- User Stories: depend on Foundational.
- Polish: depends on completion of selected user stories.

### User Story Dependencies

- US1 (P1): starts after Foundational.
- US2 (P1): starts after Foundational; can run in parallel with US1.
- US3 (P2): starts after US1 and US2 controller/menu contracts are stable.

### Within Each User Story

- Tests first.
- React/native implementation next.
- Contract/doc updates and heartbeat checkpoint last.

### Parallel Opportunities

- T003 and T004 can run in parallel.
- T006 and T007 can run in parallel.
- US1 test tasks T009/T010 can run in parallel.
- US2 test tasks T018/T019 can run in parallel.
- US3 test tasks T024/T025 can run in parallel.
- Polish evidence tasks T030/T031 can run in parallel.

---

## Parallel Example: US1

```bash
# Parallel US1 tests:
Task T009 -> src/typescript/react/src/pages/__tests__/GameEnginePage.controller-input.test.tsx
Task T010 -> tests/native/engine/test_engine_runtime_contracts.c

# Parallel foundational docs:
Task T006 -> .specify/specs/048-player-controller-foundation/data-model.md
Task T007 -> .specify/specs/048-player-controller-foundation/plan.md
```

---

## Implementation Strategy

### MVP First

1. Complete Setup and Foundational.
2. Complete US1 (keyboard reliability).
3. Validate US1 independently.

### Incremental Delivery

1. Add US2 (context menu) and validate independently.
2. Add US3 (arbitration) and validate independently.
3. Complete polish validations and traceability checks.

### Naming Note

- Feature IDs in this repository are intentionally non-contiguous.
- `048-player-controller-foundation` is valid even though older specs include lower-numbered entries (for example `006`) because numbering tracks backlog lanes, not strict sequential continuity.

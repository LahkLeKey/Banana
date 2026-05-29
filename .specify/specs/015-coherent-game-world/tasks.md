# Tasks: Coherent Game World Integration

**Input**: Design documents from `.specify/specs/015-coherent-game-world/`

**Prerequisites**: `plan.md`, `spec.md`

**Tests**: Deterministic native tests and runtime evidence are required for each user story.

## Phase 1: Setup (Shared Infrastructure)

Path targets:
- `src/native/engine/win32_dx12_poc/`
- `tests/native/runtime/engine/`
- `artifacts/native/015-coherent-game-world/`

- [x] T001 Create feature evidence directory `artifacts/native/015-coherent-game-world/` with diagnostics and continuity headings
- [x] T002 Inventory cross-slice contracts from specs `009`, `010`, `011`, and `014` into `artifacts/native/015-coherent-game-world/coherent-world-diagnostics.md`
- [x] T003 [P] Record baseline coherent-world launch signature and key diagnostics for selected variant(s)

---

## Phase 2: Foundational (Blocking Prerequisites)

Path targets:
- `src/native/engine/win32_dx12_poc/scene/`
- `src/native/engine/win32_dx12_poc/overlay/`
- `src/native/engine/win32_dx12_poc/app/`
- `tests/native/runtime/engine/`

- [x] T004 Define coherent-world profile contract in the owning scene catalog path
- [x] T005 [P] Add route/county/theme/placement consistency helpers for coherent-world validation
- [x] T006 [P] Extend launch diagnostics to emit one coherent-world identity signature
- [x] T007 Add focused validation harness entry points in native tests/CMake for coherent-world suite

**Checkpoint**: Foundation ready - coherent-world user stories can proceed

---

## Phase 3: User Story 1 - Unified World Slice Launch (Priority: P1) 🎯 MVP

**Goal**: A selected coherent-world variant launches with one consistent world identity instead of disconnected slice metadata.

**Independent Test**: Launch the coherent-world variant and verify consistent scene/region/route/theme/placement diagnostics with no mismatch errors.

### Tests for User Story 1

- [x] T008 [P] [US1] Add deterministic coherent-world launch signature test in `tests/native/runtime/engine/`
- [x] T009 [P] [US1] Add failure-path test for route/county/profile mismatch diagnostics in `tests/native/runtime/engine/`

### Implementation for User Story 1

- [x] T010 [P] [US1] Implement coherent-world profile metadata wiring in `src/native/engine/win32_dx12_poc/scene/`
- [x] T011 [US1] Update scene browser/HUD context in `src/native/engine/win32_dx12_poc/overlay/` to show coherent-world identity
- [x] T012 [US1] Wire launch path logging in `src/native/engine/win32_dx12_poc/app/` for coherent-world diagnostics
- [x] T013 [US1] Capture coherent-world launch evidence in `artifacts/native/015-coherent-game-world/coherent-world-diagnostics.md`

**Checkpoint**: Unified coherent-world launch behavior is independently functional

---

## Phase 4: User Story 2 - Cross-Slice Progression Continuity (Priority: P1)

**Goal**: Connected variants preserve deterministic continuity checkpoints when transitioning across slices.

**Independent Test**: Execute repeated connected-variant transitions and verify continuity checkpoint stability.

### Tests for User Story 2

- [x] T014 [P] [US2] Add connected-variant continuity determinism test in `tests/native/runtime/engine/`
- [x] T015 [P] [US2] Add repeated-run checkpoint drift test in `tests/native/runtime/engine/`

### Implementation for User Story 2

- [x] T016 [P] [US2] Implement continuity checkpoint composition in the owning runtime/scene path
- [x] T017 [US2] Integrate transition checkpoints into coherent-world diagnostics output
- [x] T018 [US2] Capture continuity runtime evidence in `artifacts/native/015-coherent-game-world/continuity-evidence.md`

**Checkpoint**: Coherent world continuity across slices is independently validated

---

## Phase 5: User Story 3 - World Composition Growth Path (Priority: P2)

**Goal**: New model bindings can be added through one catalog path without unrelated scene rewrites.

**Independent Test**: Introduce one new model binding and prove only targeted variant behavior changes while unrelated metadata remains stable.

### Tests for User Story 3

- [x] T019 [P] [US3] Add catalog mutation safety test for coherent-world model bindings in `tests/native/runtime/engine/`
- [x] T020 [P] [US3] Add disabled-entry diagnostics path test for coherent-world profiles in `tests/native/runtime/engine/`

### Implementation for User Story 3

- [x] T021 [P] [US3] Refine coherent-world model-key registration through one owning catalog path in `src/native/engine/win32_dx12_poc/scene/`
- [x] T022 [US3] Update coherent-world profile references to consume catalog keys rather than inlined model IDs
- [x] T023 [US3] Document add-a-model workflow in `artifacts/native/015-coherent-game-world/coherent-world-diagnostics.md`

**Checkpoint**: Coherent world growth path is maintainable and independently testable

---

## Phase 6: Polish and Cross-Cutting

- [x] T024 [P] Refresh `.specify/specs/execution-dashboard.md` with spec `015` status and next gate
- [x] T025 Update `.specify/specs/015-coherent-game-world/plan.md` with final validation command adjustments
- [x] T026 [P] Capture final coherent-world focused ctest output under `artifacts/native/015-coherent-game-world/`
- [x] T027 Run coherent-world focused validation path and record output in feature evidence docs

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies
- **Foundational (Phase 2)**: Blocks user stories
- **User Stories (Phase 3+)**: Depend on Foundational completion
- **Polish (Phase 6)**: Depends on desired user stories complete

### User Story Dependencies

- **US1 (P1)**: Baseline coherent-world launch
- **US2 (P1)**: Depends on US1 launch profile path but remains independently testable
- **US3 (P2)**: Depends on owning catalog path from US1/US2

### Parallel Opportunities

- T003, T005, T006, T008, T009, T010, T014, T015, T019, T020, T021, T024, and T026 are parallelizable where dependencies allow

## Notes

- Keep all coherent-world integration in existing owning native paths; avoid broad unrelated rewrites.
- Preserve deterministic signatures and explicit diagnostics as definition-of-done gates.
- Treat evidence capture as required completion criteria, not optional polish.

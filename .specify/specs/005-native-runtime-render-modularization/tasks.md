# Tasks: Native Runtime Render Modularization

**Input**: Design documents from `/specs/005-native-runtime-render-modularization/`

**Prerequisites**: plan.md, spec.md

**Tests**: Every user story includes focused native test-first tasks plus evidence capture tasks for the relevant CTest targets.

**Organization**: Tasks are grouped by user story so each slice can land as a small PR-sized increment with independent validation.

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Establish the feature-local planning, validation, and concern-split evidence surfaces before refactoring native code.

- [x] T001 Update .specify/specs/005-native-runtime-render-modularization/plan.md with the concrete native runtime/render structure, focused validation targets, and constitution gate notes for this refactor
- [x] T002 [P] Create .specify/specs/005-native-runtime-render-modularization/quickstart.md with focused native build and CTest commands for out/v3-native and artifact capture paths under artifacts/native/
- [x] T003 [P] Create .specify/specs/005-native-runtime-render-modularization/checklists/concern-split-checklist.md with in-scope runtime/render files, the allowed large-CMake exception, and evidence fields for FR-001, FR-009, and SC-003

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Define the shared refactor boundaries and native target wiring that all user stories rely on.

**⚠️ CRITICAL**: No user story work should be treated as complete until these boundary and validation scaffolds exist.

- [x] T004 Create .specify/specs/005-native-runtime-render-modularization/contracts/native-runtime-render-boundaries.md with the allowed controller -> service -> pipeline -> native interop direction, ABI-preservation rules, and runtime/render ownership seams
- [x] T005 Update src/native/engine/CMakeLists.txt to keep new runtime/render modularization source files and focused native validation targets grouped with the existing runtime and DX12 test registrations
- [x] T006 [P] Update docs/native-ddd-solid-refactor-plan.md with a new phase entry for feature 005 that points future PRs at .specify/specs/005-native-runtime-render-modularization/tasks.md and the planned evidence artifacts under artifacts/native/

**Checkpoint**: Shared boundaries, validation entry points, and documentation are in place for story work.

---

## Phase 3: User Story 1 - Runtime Tick Phase Decomposition (Priority: P1) 🎯 MVP

**Goal**: Split runtime tick sequencing into explicit orchestration and phase services while preserving runtime behavior and public ABI.

**Independent Test**: Run `banana_runtime_engine_tick_test`, `banana_runtime_tick_input_phase_test`, and `banana_runtime_tick_post_phase_test` from the `out/v3-native` build tree and confirm behavior parity assertions still pass.

### Tests for User Story 1

> **NOTE: Write these tests first, confirm they fail for the new seam expectations, then implement the refactor.**

- [x] T007 [P] [US1] Extend tests/native/runtime_engine_tick_test.c with failing assertions for phase ordering, callback delegation, and behavior parity around runtime_engine_tick_execute
- [ ] T008 [P] [US1] Extend tests/native/runtime_tick_input_phase_test.c with failing seam coverage for right-click polling, input normalization delegation, and null-guard behavior
- [ ] T009 [P] [US1] Extend tests/native/runtime_tick_post_phase_test.c with failing seam coverage for controller sync, camera follow, and render dispatch ordering

### Implementation for User Story 1

- [x] T010 [US1] Create src/native/engine/runtime/orchestration/runtime_tick_orchestration.c to own ordered runtime tick sequencing declared in src/native/engine/runtime/orchestration/runtime_tick_orchestration.h
- [x] T011 [US1] Refactor src/native/engine/runtime/engine_tick.c into a thin ABI-stable adapter that delegates sequencing to src/native/engine/runtime/orchestration/runtime_tick_orchestration.c
- [ ] T012 [P] [US1] Move any remaining reusable phase helpers out of src/native/engine/runtime/engine_tick.c into src/native/engine/runtime/tick_phases.c and src/native/engine/runtime/tick_phases.h instead of re-growing the adapter
- [x] T013 [US1] Update src/native/engine/CMakeLists.txt to compile src/native/engine/runtime/orchestration/runtime_tick_orchestration.c and keep the focused runtime tick test targets registered
- [ ] T014 [US1] Capture focused runtime tick validation evidence in artifacts/native/005-us1-runtime-tick.txt after running the User Story 1 targets from out/v3-native

**Checkpoint**: Runtime tick sequencing is modularized behind explicit orchestration and phase files, and the focused tick tests pass without ABI drift.

---

## Phase 4: User Story 2 - Backend Diagnostics Hardening (Priority: P2)

**Goal**: Move DX12/backend diagnostic ownership behind explicit service boundaries so failures are attributable to the owning render seam.

**Independent Test**: Run `banana_engine_dx12_runtime_smoke_test`, `banana_dx12_projection_policy_test`, `banana_dx12_scene_flow_test`, and `banana_dx12_scene_overlay_frame_test` from the `out/v3-native` build tree and confirm the focused diagnostics assertions pass.

### Tests for User Story 2

> **NOTE: Write these tests first, confirm they fail for the new diagnostics-boundary expectations, then implement the refactor.**

- [x] T015 [P] [US2] Extend tests/native/engine_dx12_runtime_smoke_test.c with failing assertions for explicit diagnostic ownership, status reporting, and failure-surface parity
- [ ] T016 [P] [US2] Extend tests/native/dx12_projection_policy_test.c with failing coverage for projection-policy diagnostics boundaries and fallback reporting
- [ ] T017 [P] [US2] Extend tests/native/dx12_scene_flow_test.c and tests/native/dx12_scene_overlay_frame_test.c with failing diagnostics-boundary coverage for scene flow and overlay reporting

### Implementation for User Story 2

- [x] T018 [US2] Create src/native/engine/render/backend_dx12_diagnostics.c and src/native/engine/render/backend_dx12_diagnostics.h to isolate DX12/backend telemetry formatting and diagnostic state transitions
- [x] T019 [US2] Refactor src/native/engine/render/backend_dx12.c to delegate diagnostic ownership to src/native/engine/render/backend_dx12_diagnostics.c without changing src/native/engine/render/backend_dx12.h
- [x] T020 [US2] Update src/native/engine/CMakeLists.txt to compile src/native/engine/render/backend_dx12_diagnostics.c and keep the DX12-focused test targets registered
- [x] T021 [US2] Capture focused DX12 diagnostics validation evidence in artifacts/native/005-us2-dx12-diagnostics.txt after running the User Story 2 targets from out/v3-native

**Checkpoint**: DX12/backend diagnostics are isolated to explicit render-owned seams and the focused diagnostics tests pass.

---

## Phase 5: User Story 3 - Architecture Direction Enforcement (Priority: P3)

**Goal**: Expand automated architecture guards so runtime and render modules cannot regress into cross-layer or cyclic spaghetti.

**Independent Test**: Run `banana_runtime_architecture_dependency_guard_test` from the `out/v3-native` build tree and confirm the updated dependency-direction rules fail on violations and pass on the modularized runtime/render layout.

### Tests for User Story 3

> **NOTE: Write these guard expansions first, confirm they fail against the missing rules, then wire the final modularized layout.**

- [x] T022 [P] [US3] Extend tests/native/runtime_architecture_dependency_guard_test.c with failing rules for src/native/engine/runtime/orchestration/runtime_tick_orchestration.c and src/native/engine/render/backend_dx12_diagnostics.c dependency direction
- [ ] T023 [P] [US3] Create tests/native/runtime_architecture_guard_rules.h to keep expanded runtime/render rule tables explicit, reviewable, and small enough for future PR-sized updates

### Implementation for User Story 3

- [ ] T024 [US3] Refactor tests/native/runtime_architecture_dependency_guard_test.c to load the expanded rule set from tests/native/runtime_architecture_guard_rules.h and cover the new runtime tick and DX12 diagnostics seams
- [ ] T025 [US3] Update .specify/specs/005-native-runtime-render-modularization/checklists/concern-split-checklist.md with final split evidence and explicit exceptions for any intentionally large runtime/render files
- [ ] T026 [US3] Capture architecture guard validation evidence in artifacts/native/005-us3-architecture-guards.txt after running banana_runtime_architecture_dependency_guard_test and the guard-adjacent focused native targets from out/v3-native

**Checkpoint**: Architecture guards enforce the intended dependency direction and the concern-split checklist records the resulting file decomposition evidence.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Close the loop on constitution evidence, task traceability, and final focused validation.

- [ ] T027 [P] Update .specify/specs/005-native-runtime-render-modularization/plan.md and .specify/specs/005-native-runtime-render-modularization/quickstart.md with completed constitution-alignment notes, evidence paths, and task traceability back to FR-001 through FR-010 and SC-001 through SC-005
- [ ] T028 [P] Update docs/native-ddd-solid-refactor-plan.md with the completed phase summary and links to artifacts/native/005-us1-runtime-tick.txt, artifacts/native/005-us2-dx12-diagnostics.txt, and artifacts/native/005-us3-architecture-guards.txt
- [ ] T029 Run the final focused native modularization sweep and capture artifacts/native/005-final-focused-suite.txt from the out/v3-native CTest targets covering runtime tick, DX12 diagnostics, and architecture guards

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies; start immediately.
- **Foundational (Phase 2)**: Depends on Setup; blocks completion of every user story.
- **User Story 1 (Phase 3)**: Depends on Foundational; this is the MVP slice.
- **User Story 2 (Phase 4)**: Depends on Foundational; can proceed independently of US1 once shared boundaries exist.
- **User Story 3 (Phase 5)**: Depends on Foundational; initial guard work can start early, but final completion depends on the new seams introduced by US1 and US2.
- **Polish (Phase 6)**: Depends on the desired user stories being complete.

### User Story Dependencies

- **US1**: No dependency on other stories after Phase 2.
- **US2**: No dependency on other stories after Phase 2.
- **US3**: Uses outputs from US1 and US2 for its final rule set, but the helper/rule-table scaffolding can begin immediately after Phase 2.

### Within Each User Story

- Tests must be written and shown failing before implementation tasks start.
- New orchestration or diagnostics files should be added before trimming the existing owner file.
- CMake target updates must land in the same slice that introduces a new native source file.
- Evidence capture tasks run only after the focused targets are green.

### Parallel Opportunities

- T002 and T003 can run in parallel.
- T004 and T006 can run in parallel after T001.
- T007, T008, and T009 can run in parallel.
- T015, T016, and T017 can run in parallel.
- T022 and T023 can run in parallel.
- T027 and T028 can run in parallel after story completion.

---

## Parallel Example: User Story 1

```bash
# Write the focused runtime tick tests first:
Task: "Extend tests/native/runtime_engine_tick_test.c with failing assertions for phase ordering, callback delegation, and behavior parity around runtime_engine_tick_execute"
Task: "Extend tests/native/runtime_tick_input_phase_test.c with failing seam coverage for right-click polling, input normalization delegation, and null-guard behavior"
Task: "Extend tests/native/runtime_tick_post_phase_test.c with failing seam coverage for controller sync, camera follow, and render dispatch ordering"
```

---

## Parallel Example: User Story 2

```bash
# Expand the DX12 diagnostics tests together before implementation:
Task: "Extend tests/native/engine_dx12_runtime_smoke_test.c with failing assertions for explicit diagnostic ownership, status reporting, and failure-surface parity"
Task: "Extend tests/native/dx12_projection_policy_test.c with failing coverage for projection-policy diagnostics boundaries and fallback reporting"
Task: "Extend tests/native/dx12_scene_flow_test.c and tests/native/dx12_scene_overlay_frame_test.c with failing diagnostics-boundary coverage for scene flow and overlay reporting"
```

---

## Parallel Example: User Story 3

```bash
# Split architecture-guard preparation from final enforcement wiring:
Task: "Extend tests/native/runtime_architecture_dependency_guard_test.c with failing rules for runtime tick orchestration and DX12 diagnostics dependency direction"
Task: "Create tests/native/runtime_architecture_guard_rules.h to keep expanded runtime/render rule tables explicit, reviewable, and small enough for future PR-sized updates"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup.
2. Complete Phase 2: Foundational.
3. Complete Phase 3: User Story 1.
4. Validate the focused runtime tick suite and archive artifacts/native/005-us1-runtime-tick.txt.
5. Stop for review before opening the DX12 and architecture guard slices.

### Incremental Delivery

1. Land Setup + Foundational as documentation and validation scaffolding.
2. Land US1 as the first code slice and validate the runtime tick suite.
3. Land US2 as a separate DX12 diagnostics slice and validate the DX12 suite.
4. Land US3 as a separate architecture-guard slice and validate the guard suite.
5. Finish with constitution evidence, traceability updates, and the final focused modularization sweep.

### Parallel Team Strategy

1. One developer owns the feature docs and CMake scaffolding in Phases 1 and 2.
2. After Phase 2, one developer can own US1 while another prepares the failing DX12 diagnostics tests for US2.
3. Once the new seams exist, a third developer can finish US3 by expanding the architecture guard rules and evidence capture.

---

## Notes

- Every checklist task follows the required `- [ ] T### [P] [US#] Description with file path` format where applicable.
- The task list intentionally favors narrow PR-sized seams over broad rewrites.
- Public ABI preservation is required for every runtime/render code slice.
- The concern-split checklist is the acceptance surface for FR-001, FR-009, and SC-003.
---
description: "Task list for CMake Preset Activation Fix"
---

# Tasks: CMake Preset Activation Fix

**Input**: Design documents from `.specify/specs/008-cmake-preset-activation-fix/`
**Prerequisites**: plan.md (required), spec.md (required), research.md, data-model.md, contracts/cmake-preset-activation-contract.md, quickstart.md

**Tests**: This feature requires explicit CMake Tools validation plus native lane execution evidence.

**Organization**: Tasks are grouped by user story so each story can be implemented and tested independently.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: User story mapping (`US1`, `US2`, `US3`)
- Include exact file paths in each task description

## Phase 1: Setup (Shared Scope)

**Purpose**: Align feature workspace and initial validation references.

- [x] T001 Create 008 execution tracker note in `.specify/specs/008-cmake-preset-activation-fix/README.md`
- [x] T002 [P] Capture in-scope config surfaces in `.specify/specs/008-cmake-preset-activation-fix/analysis/in-scope-files.md`
- [x] T003 [P] Capture validation command references in `.specify/specs/008-cmake-preset-activation-fix/analysis/validation-commands.md`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Establish deterministic activation contract before user-story implementation.

**⚠️ CRITICAL**: User story tasks start only after this phase is complete.

- [x] T004 Define preset activation matrix in `.specify/specs/008-cmake-preset-activation-fix/analysis/preset-activation-matrix.md`
- [x] T005 Define workspace binding matrix in `.specify/specs/008-cmake-preset-activation-fix/analysis/workspace-binding-matrix.md`
- [x] T006 Define diagnostic classification expectations in `.specify/specs/008-cmake-preset-activation-fix/analysis/diagnostic-classification.md`

**Checkpoint**: Activation/binding/diagnostic contract is fixed and reviewable.

---

## Phase 3: User Story 1 - Configure Works Without Manual Preset Rescue (Priority: P1) 🎯 MVP

**Goal**: Ensure CMake Tools configure/build activates intended preset in supported workspace contexts.

**Independent Test**: Invoke CMake Tools build from root and tests context and verify configure succeeds without ad hoc preset rescue.

### Tests for User Story 1

- [x] T007 [P] [US1] Add CMake Tools configure acceptance checklist in `.specify/specs/008-cmake-preset-activation-fix/analysis/cmake-tools-configure-checklist.md`

### Implementation for User Story 1

- [x] T008 [US1] Align root preset activation settings in `.vscode/settings.json`
- [x] T009 [US1] Align tests-folder workspace binding settings in `tests/.vscode/settings.json`
- [x] T010 [US1] Align workspace-level defaults in `Banana.code-workspace`
- [x] T011 [US1] Validate preset catalog contract in `CMakePresets.json`
- [x] T012 [US1] Run CMake Tools build configure path and capture evidence in `.specify/specs/008-cmake-preset-activation-fix/analysis/us1-configure-evidence.md`

**Checkpoint**: Configure path succeeds without manual preset rescue in supported contexts.

---

## Phase 4: User Story 2 - Native Targets and Tests Are Discoverable (Priority: P2)

**Goal**: Ensure target/test discovery is available once configure succeeds.

**Independent Test**: List targets/tests via CMake Tools and confirm expected native entries are visible.

### Tests for User Story 2

- [x] T013 [P] [US2] Define discovery expectation list in `.specify/specs/008-cmake-preset-activation-fix/analysis/discovery-expected-surfaces.md`

### Implementation for User Story 2

- [x] T014 [US2] Run `ListBuildTargets_CMakeTools` and capture evidence in `.specify/specs/008-cmake-preset-activation-fix/analysis/us2-target-discovery-evidence.md`
- [x] T015 [US2] Run `ListTests_CMakeTools` and capture evidence in `.specify/specs/008-cmake-preset-activation-fix/analysis/us2-test-discovery-evidence.md`
- [x] T016 [US2] Run native lane via CMake Tools and capture pass/fail evidence in `.specify/specs/008-cmake-preset-activation-fix/analysis/us2-native-lane-evidence.md`

**Checkpoint**: Discovery and native lane execution are verifiably available in tool-integrated flow.

---

## Phase 5: User Story 3 - Actionable Failure Diagnostics (Priority: P3)

**Goal**: Ensure invalid activation states produce deterministic, actionable diagnostics.

**Independent Test**: Introduce controlled invalid states and verify diagnostics classify failure correctly.

### Tests for User Story 3

- [x] T017 [P] [US3] Define negative-test scenarios in `.specify/specs/008-cmake-preset-activation-fix/analysis/diagnostic-negative-scenarios.md`

### Implementation for User Story 3

- [x] T018 [US3] Execute missing-preset simulation and capture output in `.specify/specs/008-cmake-preset-activation-fix/analysis/us3-missing-preset-evidence.md`
- [x] T019 [US3] Execute context-mismatch simulation and capture output in `.specify/specs/008-cmake-preset-activation-fix/analysis/us3-context-mismatch-evidence.md`
- [x] T020 [US3] Consolidate diagnostic evidence in `.specify/specs/008-cmake-preset-activation-fix/analysis/diagnostic-summary.md`

**Checkpoint**: Diagnostics are deterministic and actionable for defined failure classes.

---

## Phase 6: Polish & Cross-Cutting Completion

**Purpose**: Final consistency, quickstart alignment, and closure evidence.

- [x] T021 Reconcile terminology across `.specify/specs/008-cmake-preset-activation-fix/`
- [x] T022 [P] Update `.specify/specs/008-cmake-preset-activation-fix/quickstart.md` with exact commands used during execution
- [x] T023 [P] Update `.specify/specs/008-cmake-preset-activation-fix/contracts/cmake-preset-activation-contract.md` if execution changed closure details
- [x] T024 Verify required analysis files exist in `.specify/specs/008-cmake-preset-activation-fix/analysis/`
- [x] T025 Mark completion evidence and task statuses in `.specify/specs/008-cmake-preset-activation-fix/tasks.md`

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies.
- **Phase 2 (Foundational)**: Depends on Setup; blocks all user stories.
- **Phase 3 (US1)**: Depends on Foundational.
- **Phase 4 (US2)**: Depends on US1 configure success.
- **Phase 5 (US3)**: Depends on US1/US2 validated baseline behavior.
- **Phase 6 (Polish)**: Depends on all user-story phases.

### User Story Dependencies

- **US1 (P1)**: MVP and bug-fix gate.
- **US2 (P2)**: Requires stable configure behavior from US1.
- **US3 (P3)**: Requires established baseline to validate negative diagnostics.

### Parallel Opportunities

- T002 and T003 can run in parallel.
- T007 can run in parallel with early configuration edits once foundational tasks are done.
- T013 and early evidence-file scaffolding can run in parallel.
- T022 and T023 can run in parallel.

---

## Implementation Strategy

### MVP First (US1 Only)

1. Complete Phase 1 and Phase 2.
2. Complete Phase 3 (US1).
3. Validate configure success in tool-integrated path.

### Incremental Delivery

1. Deliver US1 configure stabilization.
2. Deliver US2 discovery and lane execution confidence.
3. Deliver US3 diagnostic determinism.
4. Complete final polish and closure updates.

### Notes

- Preserve existing native feature contracts.
- Keep scope bounded to preset activation and discovery behavior.
- Prefer CMake Tools lane as the primary closure path.

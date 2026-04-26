---
description: "Task list for Right Brain Binary Runnable C Code"
---

# Tasks: Right Brain Binary Runnable C Code

**Input**: Design documents from `.specify/specs/011-right-brain-binary-runnable/`
**Prerequisites**: plan.md (required), spec.md (required), source SPIKE 009-ml-brain-domain-spike

**Tests**: This feature requires explicit native lane tests (positive, negative samples, threshold sweep, JSON contract).

**Organization**: Tasks are grouped by user story so each story can be implemented and tested independently.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: User story mapping (`US1`, `US2`, `US3`, `US4`)
- Include exact file paths in each task description

## Phase 1: Setup (Shared Scope)

**Purpose**: Stage feature workspace and confirm preset path is healthy.

- [ ] T001 Create execution tracker in `.specify/specs/011-right-brain-binary-runnable/README.md`
- [ ] T002 [P] Capture in-scope files in `.specify/specs/011-right-brain-binary-runnable/analysis/in-scope-files.md`
- [ ] T003 [P] Capture validation commands in `.specify/specs/011-right-brain-binary-runnable/analysis/validation-commands.md`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Lock operating point and JSON contract before model or test work.

**⚠️ CRITICAL**: User story tasks start only after this phase is complete.

- [ ] T004 [P] Choose and document operating point (precision target, recall target, default threshold) in `src/native/core/domain/ml/binary/banana_ml_binary.h`
- [ ] T005 [P] Document JSON shape contract (`model`, `label`, `confidence`, `confusion_matrix`, `jaccard`) in `src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.h`
- [ ] T006 Define affine calibration constants (`a`, `b`) and document semantics in `src/native/core/domain/ml/binary/banana_ml_binary.h`

**Checkpoint**: Operating point and contracts are fixed and reviewable.

---

## Phase 3: User Story 1 - Pinned Operating Point and Calibration (Priority: P1) 🎯 MVP

**Goal**: Calibrated probability and pinned operating point reproducible across deployments.

**Independent Test**: Run calibration anchor block in `banana_test_ml_binary`.

### Implementation for User Story 1

- [ ] T007 [US1] Apply affine calibration of raw score before threshold in `src/native/core/domain/ml/binary/banana_ml_binary.c`
- [ ] T008 [US1] Add positive-sample calibration test (label=banana, confidence>=threshold) in `tests/native/test_ml_binary.c`
- [ ] T009 [US1] Register `banana_test_ml_binary` ctest target in `tests/native/CMakeLists.txt`
- [ ] T010 [US1] Capture US1 evidence in `.specify/specs/011-right-brain-binary-runnable/analysis/us1-operating-point-evidence.md`

**Checkpoint**: Operating point is enforced at runtime with passing native test.

---

## Phase 4: User Story 2 - Negative-Sample Coverage (Priority: P1)

**Goal**: Reliably reject non-banana inputs to bound false-positive cost (RB-R04).

**Independent Test**: Run negative-sample block in `banana_test_ml_binary`.

### Implementation for User Story 2

- [ ] T011 [US2] Add obviously-not-banana payload test (engine oil) in `tests/native/test_ml_binary.c`
- [ ] T012 [US2] Add empty-text payload test (deterministic documented behavior) in `tests/native/test_ml_binary.c`
- [ ] T013 [US2] Add banana-adjacent payload test (banana-flavored / banana-yellow plastic) in `tests/native/test_ml_binary.c`
- [ ] T014 [US2] Capture US2 evidence in `.specify/specs/011-right-brain-binary-runnable/analysis/us2-negative-samples-evidence.md`

**Checkpoint**: Negative-sample acceptance scenario is enforced.

---

## Phase 5: User Story 3 - Configurable Threshold (Priority: P2)

**Goal**: Per-deployment threshold tuning (RB-R01) without recompilation.

**Independent Test**: Run threshold-sweep block in `banana_test_ml_binary`.

### Implementation for User Story 3

- [ ] T015 [US3] Confirm or add `_with_threshold` entry point in `src/native/core/domain/ml/binary/banana_ml_binary.h` and `.c`
- [ ] T016 [US3] Wire threshold parameter through `src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.c`
- [ ] T017 [US3] Add monotone threshold-sweep test (label transitions banana→not_banana exactly once) in `tests/native/test_ml_binary.c`
- [ ] T018 [US3] Capture US3 evidence in `.specify/specs/011-right-brain-binary-runnable/analysis/us3-threshold-sweep-evidence.md`

**Checkpoint**: Threshold is operator-tunable and verifiably monotone.

---

## Phase 6: User Story 4 - Locked JSON Contract (Priority: P2)

**Goal**: Stable JSON shape so .NET interop deserialization does not silently break.

**Independent Test**: Run JSON field-presence block in `banana_test_ml_binary`.

### Implementation for User Story 4

- [ ] T019 [US4] Ensure deterministic field ordering in `src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.c`
- [ ] T020 [US4] Add explicit field-presence assertions for `model`, `label`, `confidence`, `confusion_matrix`, `jaccard` in `tests/native/test_ml_binary.c`
- [ ] T021 [US4] Add null-pointer guard test for binary classify in `tests/native/test_ml_binary.c`

**Checkpoint**: JSON contract is field-locked, not substring-matched.

---

## Phase 7: Polish & Cross-Cutting Completion

**Purpose**: Final consistency, validation, and closure.

- [ ] T022 Run `cmake --preset default && cmake --build --preset default && ctest --preset default --output-on-failure` and capture full evidence in `.specify/specs/011-right-brain-binary-runnable/analysis/native-lane-evidence.md`
- [ ] T023 [P] Update `.specify/specs/011-right-brain-binary-runnable/quickstart.md` (create if missing) with exact commands used
- [ ] T024 Verify analysis artifact completeness in `.specify/specs/011-right-brain-binary-runnable/analysis/`
- [ ] T025 Mark all tasks complete in `.specify/specs/011-right-brain-binary-runnable/tasks.md`

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies.
- **Phase 2 (Foundational)**: Depends on Setup; blocks all user stories.
- **Phase 3 (US1)**: Depends on Foundational.
- **Phase 4 (US2)**: Depends on US1 test target registration (T009).
- **Phase 5 (US3)**: Depends on Foundational; can parallelize with US2.
- **Phase 6 (US4)**: Depends on US1 test target registration (T009); can parallelize with US2/US3.
- **Phase 7 (Polish)**: Depends on all user-story phases.

### Parallel Opportunities

- T002, T003 can run in parallel.
- T004, T005 can run in parallel.
- US2, US3, US4 can run in parallel after US1 test target is registered.

---

## Implementation Strategy

### MVP First (US1 Only)

1. Complete Phase 1, Phase 2, Phase 3.
2. Confirm operating point is enforced and tested.

### Incremental Delivery

1. US1 operating point + calibration.
2. US2 negative-sample coverage.
3. US3 configurable threshold.
4. US4 locked JSON contract.
5. Polish closure.

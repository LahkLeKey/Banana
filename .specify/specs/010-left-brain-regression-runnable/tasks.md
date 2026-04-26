---
description: "Task list for Left Brain Regression Runnable C Code"
---

# Tasks: Left Brain Regression Runnable C Code

**Input**: Design documents from `.specify/specs/010-left-brain-regression-runnable/`
**Prerequisites**: plan.md (required), spec.md (required), source SPIKE 009-ml-brain-domain-spike

**Tests**: This feature requires explicit native lane tests (range, boundary, calibration, null/NaN guards).

**Organization**: Tasks are grouped by user story so each story can be implemented and tested independently.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: User story mapping (`US1`, `US2`, `US3`, `US4`)
- Include exact file paths in each task description

## Phase 1: Setup (Shared Scope)

**Purpose**: Stage feature workspace and confirm preset path is healthy.

- [ ] T001 Create execution tracker in `.specify/specs/010-left-brain-regression-runnable/README.md`
- [ ] T002 [P] Capture in-scope files in `.specify/specs/010-left-brain-regression-runnable/analysis/in-scope-files.md`
- [ ] T003 [P] Capture validation commands in `.specify/specs/010-left-brain-regression-runnable/analysis/validation-commands.md`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Lock the input/output contract before any guard or test work begins.

**⚠️ CRITICAL**: User story tasks start only after this phase is complete.

- [ ] T004 [P] Document input feature contract (slot names, ranges) in `src/native/core/domain/ml/regression/banana_ml_regression.h`
- [ ] T005 [P] Document output range and clamping contract in `src/native/core/domain/ml/regression/banana_ml_regression.h`
- [ ] T006 Document consumer usage pattern (ordinal-only, threshold guidance) in `src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.h`

**Checkpoint**: Contract surface is fixed and reviewable.

---

## Phase 3: User Story 1 - Locked Input/Output Contract (Priority: P1) 🎯 MVP

**Goal**: Lock and exercise the regression input feature vector and output range contract.

**Independent Test**: Run the new contract block in `banana_test_ml_regression`; assert all named input slots and output range.

### Implementation for User Story 1

- [ ] T007 [US1] Add NaN/Inf rejection guard in `src/native/core/domain/ml/regression/banana_ml_regression.c`
- [ ] T008 [US1] Add null-pointer guard contract test in `tests/native/test_ml_regression.c`
- [ ] T009 [US1] Add full-feature-vector contract test in `tests/native/test_ml_regression.c`
- [ ] T010 [US1] Register `banana_test_ml_regression` ctest target in `tests/native/CMakeLists.txt`
- [ ] T011 [US1] Capture US1 evidence in `.specify/specs/010-left-brain-regression-runnable/analysis/us1-contract-evidence.md`

**Checkpoint**: Contract is enforced at runtime with passing native test.

---

## Phase 4: User Story 2 - Deterministic Boundary and Clamping Tests (Priority: P1)

**Goal**: Prevent out-of-range scores under boundary or distribution-shifted inputs.

**Independent Test**: Run `banana_test_ml_regression` boundary block and confirm clamping holds.

### Implementation for User Story 2

- [ ] T012 [US2] Add all-zero feature vector boundary test in `tests/native/test_ml_regression.c`
- [ ] T013 [US2] Add all-one feature vector boundary test in `tests/native/test_ml_regression.c`
- [ ] T014 [US2] Add out-of-range input (e.g., 5.0 / -5.0) clamping test in `tests/native/test_ml_regression.c`
- [ ] T015 [US2] Capture US2 evidence in `.specify/specs/010-left-brain-regression-runnable/analysis/us2-boundary-evidence.md`

**Checkpoint**: Boundary contract is verifiably enforced.

---

## Phase 5: User Story 3 - Calibration Spot-Check (Priority: P2)

**Goal**: Pin 3 reference inputs (clearly-banana, ambiguous, clearly-not-banana) so coefficient drift surfaces in CI.

**Independent Test**: Run the calibration anchor block in `banana_test_ml_regression`.

### Implementation for User Story 3

- [ ] T016 [US3] Add clearly-banana calibration anchor (`score > 0.65`) in `tests/native/test_ml_regression.c`
- [ ] T017 [US3] Add ambiguous calibration anchor (`0.30 < score < 0.70`) in `tests/native/test_ml_regression.c`
- [ ] T018 [US3] Add clearly-not-banana calibration anchor (`score < 0.35`) in `tests/native/test_ml_regression.c`
- [ ] T019 [US3] Capture US3 evidence in `.specify/specs/010-left-brain-regression-runnable/analysis/us3-calibration-evidence.md`

**Checkpoint**: Calibration drift is detectable at CI time.

---

## Phase 6: User Story 4 - Documented Consumer Contract (Priority: P3)

**Goal**: Eliminate downstream threshold fragmentation by documenting consumer guidance in the wrapper header.

**Independent Test**: Inspect `banana_wrapper_ml_regression.h` for documented threshold guidance.

### Implementation for User Story 4

- [ ] T020 [US4] Add ordinal-only interpretation note in `src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.h`
- [ ] T021 [US4] Add threshold guidance referencing calibration anchors in `src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.h`

**Checkpoint**: Consumer contract is explicit and discoverable in headers.

---

## Phase 7: Polish & Cross-Cutting Completion

**Purpose**: Final consistency, validation, and closure.

- [ ] T022 Run `cmake --preset default && cmake --build --preset default && ctest --preset default --output-on-failure` and capture full evidence in `.specify/specs/010-left-brain-regression-runnable/analysis/native-lane-evidence.md`
- [ ] T023 [P] Update `.specify/specs/010-left-brain-regression-runnable/quickstart.md` (create if missing) with exact commands used
- [ ] T024 Verify analysis artifact completeness in `.specify/specs/010-left-brain-regression-runnable/analysis/`
- [ ] T025 Mark all tasks complete in `.specify/specs/010-left-brain-regression-runnable/tasks.md`

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies.
- **Phase 2 (Foundational)**: Depends on Setup; blocks all user stories.
- **Phase 3 (US1)**: Depends on Foundational.
- **Phase 4 (US2)**: Depends on US1 test target registration (T010).
- **Phase 5 (US3)**: Depends on US1 test target registration (T010).
- **Phase 6 (US4)**: Independent of US2/US3 once Foundational complete.
- **Phase 7 (Polish)**: Depends on all user-story phases.

### Parallel Opportunities

- T002, T003 can run in parallel.
- T004, T005 can run in parallel (same file, different sections — coordinate edit).
- US2, US3, US4 can run in parallel after US1 test target is registered.

---

## Implementation Strategy

### MVP First (US1 Only)

1. Complete Phase 1, Phase 2, Phase 3.
2. Confirm contract is locked and tested.

### Incremental Delivery

1. US1 contract lock.
2. US2 boundary safety.
3. US3 calibration anchor.
4. US4 consumer contract.
5. Polish closure.

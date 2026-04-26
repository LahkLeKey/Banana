---
description: "Task list for Full Brain Transformer Runnable C Code"
---

# Tasks: Full Brain Transformer Runnable C Code

**Input**: Design documents from `.specify/specs/012-full-brain-transformer-runnable/`
**Prerequisites**: plan.md (required), spec.md (required), source SPIKE 009-ml-brain-domain-spike

**Tests**: This feature requires explicit native lane tests (idempotence, embedding helper, attention toggle, null guard).

**Organization**: Tasks are grouped by user story so each story can be implemented and tested independently.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: User story mapping (`US1`, `US2`, `US3`, `US4`)
- Include exact file paths in each task description

## Phase 1: Setup (Shared Scope)

**Purpose**: Stage feature workspace and confirm preset path is healthy.

- [ ] T001 Create execution tracker in `.specify/specs/012-full-brain-transformer-runnable/README.md`
- [ ] T002 [P] Capture in-scope files in `.specify/specs/012-full-brain-transformer-runnable/analysis/in-scope-files.md`
- [ ] T003 [P] Capture validation commands in `.specify/specs/012-full-brain-transformer-runnable/analysis/validation-commands.md`

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Lock context token schema and JSON output shape before any seed or attention work.

**⚠️ CRITICAL**: User story tasks start only after this phase is complete.

- [ ] T004 [P] Document context token schema (Left score slot, Right label slot, raw feature slots) in `src/native/core/domain/ml/transformer/banana_ml_transformer.h`
- [ ] T005 [P] Document JSON output shape contract in `src/native/wrapper/domain/ml/transformer/banana_wrapper_ml_transformer.h`
- [ ] T006 Define embedding output shape (dim, expected L2 norm bound) in `src/native/core/domain/ml/transformer/banana_ml_transformer.h`

**Checkpoint**: Input/output contracts are fixed and reviewable.

---

## Phase 3: User Story 1 - Locked Input Format Contract (Priority: P1) 🎯 MVP

**Goal**: Stable context token schema so Left/Right outputs can be passed in without surprise (FB-R05).

**Independent Test**: Run context token contract block in `banana_test_ml_transformer`.

### Implementation for User Story 1

- [ ] T007 [US1] Add null-pointer guard for features and out_result in `src/native/core/domain/ml/transformer/banana_ml_transformer.c`
- [ ] T008 [US1] Add well-formed context token block contract test in `tests/native/test_ml_transformer.c`
- [ ] T009 [US1] Add null-pointer guard test in `tests/native/test_ml_transformer.c`
- [ ] T010 [US1] Register `banana_test_ml_transformer` ctest target in `tests/native/CMakeLists.txt`
- [ ] T011 [US1] Capture US1 evidence in `.specify/specs/012-full-brain-transformer-runnable/analysis/us1-input-contract-evidence.md`

**Checkpoint**: Input format contract is enforced at runtime with passing native test.

---

## Phase 4: User Story 2 - Deterministic Seed Handling (Priority: P1)

**Goal**: Identical inputs produce identical outputs across repeated calls so the transformer is unit-testable.

**Independent Test**: Run idempotence block in `banana_test_ml_transformer` (5 repeated calls).

### Implementation for User Story 2

- [ ] T012 [US2] Audit `src/native/core/domain/ml/transformer/banana_ml_transformer.c` for nondeterminism (time, PRNG, uninitialized memory)
- [ ] T013 [US2] Lock seed to documented constant in `src/native/core/domain/ml/transformer/banana_ml_transformer.c`
- [ ] T014 [US2] Add 5-call byte-identical idempotence test in `tests/native/test_ml_transformer.c`
- [ ] T015 [US2] Capture US2 evidence in `.specify/specs/012-full-brain-transformer-runnable/analysis/us2-idempotence-evidence.md`

**Checkpoint**: Idempotence is enforced and verifiable.

---

## Phase 5: User Story 3 - Embedding Assertion Test Pattern (Priority: P2)

**Goal**: Reusable embedding-level assertion helper (FB-R04) — scalar assertions are insufficient for vector outputs.

**Independent Test**: Run embedding helper block in `banana_test_ml_transformer`.

### Implementation for User Story 3

- [ ] T016 [US3] Define `assert_embedding_valid(const double* vec, size_t dim, double max_norm)` helper in `tests/native/test_ml_transformer.c`
- [ ] T017 [US3] Apply helper to current transformer output (stub fingerprint vector) in `tests/native/test_ml_transformer.c`
- [ ] T018 [US3] Add helper failure-mode test (NULL vec, zero dim) in `tests/native/test_ml_transformer.c`
- [ ] T019 [US3] Capture US3 evidence in `.specify/specs/012-full-brain-transformer-runnable/analysis/us3-embedding-helper-evidence.md`

**Checkpoint**: Embedding assertion pattern is reusable for future vector outputs.

---

## Phase 6: User Story 4 - Attention-Map Logging Hook (Priority: P3)

**Goal**: Explainability evidence on demand (FB-R03) without paying cost on the hot path by default.

**Independent Test**: Run attention-on and attention-off blocks in `banana_test_ml_transformer`.

### Implementation for User Story 4

- [ ] T020 [US4] Add `BANANA_TRANSFORMER_LOG_ATTENTION` env-var or compile-flag toggle in `src/native/core/domain/ml/transformer/banana_ml_transformer.c`
- [ ] T021 [US4] Populate per-row attention buffer summing to ~1.0 when toggle is on in `src/native/core/domain/ml/transformer/banana_ml_transformer.c`
- [ ] T022 [US4] Skip allocation when toggle is off (default-off bit-for-bit equivalence) in `src/native/core/domain/ml/transformer/banana_ml_transformer.c`
- [ ] T023 [US4] Add attention-on test (buffer populated, weights finite, sum ≈ 1.0) in `tests/native/test_ml_transformer.c`
- [ ] T024 [US4] Add attention-off test (buffer NULL, output unchanged vs prior baseline) in `tests/native/test_ml_transformer.c`
- [ ] T025 [US4] Capture US4 evidence in `.specify/specs/012-full-brain-transformer-runnable/analysis/us4-attention-evidence.md`

**Checkpoint**: Attention-map logging is operational under toggle.

---

## Phase 7: Polish & Cross-Cutting Completion

**Purpose**: Final consistency, validation, and closure.

- [ ] T026 Run `cmake --preset default && cmake --build --preset default && ctest --preset default --output-on-failure` and capture full evidence in `.specify/specs/012-full-brain-transformer-runnable/analysis/native-lane-evidence.md`
- [ ] T027 [P] Update `.specify/specs/012-full-brain-transformer-runnable/quickstart.md` (create if missing) with exact commands used
- [ ] T028 Verify analysis artifact completeness in `.specify/specs/012-full-brain-transformer-runnable/analysis/`
- [ ] T029 Document residual U-001 quantization follow-up in `.specify/specs/012-full-brain-transformer-runnable/analysis/residual-followups.md`
- [ ] T030 Mark all tasks complete in `.specify/specs/012-full-brain-transformer-runnable/tasks.md`

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies.
- **Phase 2 (Foundational)**: Depends on Setup; blocks all user stories.
- **Phase 3 (US1)**: Depends on Foundational.
- **Phase 4 (US2)**: Depends on US1 test target registration (T010).
- **Phase 5 (US3)**: Depends on US1 test target registration (T010); can parallelize with US2.
- **Phase 6 (US4)**: Depends on US1 test target registration (T010); can parallelize with US2/US3.
- **Phase 7 (Polish)**: Depends on all user-story phases.

### Parallel Opportunities

- T002, T003 can run in parallel.
- T004, T005 can run in parallel.
- US2, US3, US4 can run in parallel after US1 test target is registered.

---

## Implementation Strategy

### MVP First (US1 Only)

1. Complete Phase 1, Phase 2, Phase 3.
2. Confirm input format contract is enforced.

### Incremental Delivery

1. US1 input contract.
2. US2 deterministic seed.
3. US3 embedding helper.
4. US4 attention-map logging.
5. Polish + residual U-001 follow-up note.

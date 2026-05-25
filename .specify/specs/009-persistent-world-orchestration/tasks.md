# Tasks: Deterministic Persistent World Orchestration

**Input**: Design documents from `.specify/specs/009-persistent-world-orchestration/`

**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Tests**: Validation evidence is required for each user story per plan.md and quickstart.md.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: User story label (US1, US2, US3)
- Every task includes at least one concrete file path

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Create feature scaffolding and wire new orchestration surfaces into build/runtime entry points.

- [x] T001 Create persistent-world domain scaffolding in src/typescript/api/src/domains/persistent-world-orchestration/index.ts
- [x] T002 [P] Add deterministic orchestration contracts module in src/typescript/api/src/lib/contracts/v1/persistentWorld.ts
- [x] T003 Register persistent-world domain bootstrap in src/typescript/api/src/index.ts
- [x] T004 Add new deterministic persistence test targets in src/native/engine/CMakeLists.txt and src/native/CMakeLists.txt

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Implement shared deterministic contracts that block all stories: root seed derivation, area identity/chunk partitioning, and mesh parity controls.

**CRITICAL**: No user story implementation starts before this phase is complete.

- [x] T005 Implement canonical world-seed derivation helpers in src/native/engine/runtime/terrain/terrain_generation.c
- [x] T006 [P] Expose deterministic generation fingerprint fields in src/native/engine/runtime/chunk/chunk_stream_packet.c
- [x] T007 Implement stable area identity hash and partition tuple mapping in src/native/engine/runtime/terrain/terrain_chunks.c
- [x] T008 [P] Enforce deterministic adjacency ordering and boundary hash emission in src/native/engine/runtime/terrain/terrain_chunks.c
- [x] T009 Add mesh vector signature/parity controls in src/native/engine/render/mesh/mesh_terrain.c
- [x] T010 [P] Wire foundational deterministic telemetry fields in src/native/engine/runtime/world/world_telemetry.c
- [x] T011 Add TypeScript area identity and version DTOs in src/typescript/api/src/lib/contracts/v1/persistentWorld.ts
- [x] T012 Implement API-side deterministic input normalization helpers in src/typescript/api/src/domains/persistent-world-orchestration/normalization.ts
- [x] T013 Add foundational contract tests for seed/identity/fingerprint normalization in src/typescript/api/src/domains/persistent-world-orchestration/normalization.test.ts

**Checkpoint**: Deterministic seed, area identity/chunk partition, and mesh parity contract are stable and reusable by all stories.

---

## Phase 3: User Story 1 - Revisit Same Area Across Sessions (Priority: P1) 🎯 MVP

**Goal**: Ensure deterministic viewport continuity and revisit parity so returning players see the same terrain, mesh vectors, and continuity at chunk seams.

**Independent Test**: Generate and persist area A, end session, reopen session, revisit area A, and verify terrain/mesh/continuity signatures and state match prior authoritative snapshot.

### Tests for User Story 1

- [x] T014 [P] [US1] Add native deterministic revisit generation parity test in tests/native/runtime/terrain/runtime_terrain_generation_revisit_parity_test.c
- [x] T015 [P] [US1] Add native chunk seam continuity parity test in tests/native/terrain/terrain_chunk_seam_continuity_test.c
- [x] T016 [P] [US1] Add API revisit integration scenario in src/typescript/api/src/integration/persistent-world-revisit-parity.integration.test.ts
- [x] T049 [P] [US1] Add initial-vs-revisit same-contract-path parity test for baseline plus delta replay in src/typescript/api/src/integration/persistent-world-contract-path-parity.integration.test.ts

### Implementation for User Story 1

- [x] T017 [US1] Implement deterministic revisit baseline fetch/rebuild path in src/native/engine/runtime/terrain/terrain_runtime.c
- [x] T018 [US1] Enforce viewport chunk continuity transforms for revisit rendering in src/native/engine/runtime/render/submit/render_submit.c
- [x] T019 [P] [US1] Propagate deterministic chunk fingerprint metadata into world streaming in src/native/engine/runtime/chunk/chunk_stream_packet.c
- [x] T020 [US1] Add world chunk replay endpoint and request validation in src/typescript/api/src/routes/world.ts
- [x] T021 [US1] Implement authoritative revisit baseline read service in src/typescript/api/src/domains/persistent-world-orchestration/services/revisitBaselineService.ts
- [x] T022 [US1] Integrate revisit baseline service into world route wiring in src/typescript/api/src/domains/persistent-world-orchestration/index.ts
- [x] T023 [US1] Add deterministic revisit error mapping and response codes in src/typescript/api/src/lib/errors/domainErrors.ts
- [x] T051 [US1] Enforce one shared orchestration path selector for initial generation and revisit reconstruction in src/typescript/api/src/domains/persistent-world-orchestration/services/orchestrationPathService.ts

**Checkpoint**: User Story 1 is independently functional and satisfies revisit parity plus viewport continuity behavior.

---

## Phase 4: User Story 2 - Cross-Player Consistency On Modified Areas (Priority: P1)

**Goal**: Persist authoritative baseline + deltas and replay deterministic merge outputs so later players always observe canonical modified area state.

**Independent Test**: Player 1 modifies area B, Player 2 arrives later and after reconnect, and both observe the same canonical post-merge state with stale updates rejected.

### Tests for User Story 2

- [x] T024 [P] [US2] Add API contract test for authoritative baseline and delta persistence endpoints in src/typescript/api/src/routes/world.persistent.contract.test.ts
- [x] T025 [P] [US2] Add API integration test for cross-player modified-area parity with <=1500 ms synchronization window assertion in src/typescript/api/src/integration/persistent-world-cross-player-parity.integration.test.ts
- [x] T026 [P] [US2] Add API integration test for stale-update rejection/version monotonicity in src/typescript/api/src/integration/persistent-world-stale-update.integration.test.ts
- [x] T050 [P] [US2] Add repeated overlapping-delta conflict-parity integration test matrix for deterministic merge outputs in src/typescript/api/src/integration/persistent-world-conflict-parity.integration.test.ts

### Implementation for User Story 2

- [x] T027 [US2] Implement authoritative baseline repository operations in src/typescript/api/src/domains/persistent-world-orchestration/persistence/baselineRepository.ts
- [x] T028 [P] [US2] Implement ordered interaction delta repository and idempotency checks in src/typescript/api/src/domains/persistent-world-orchestration/persistence/deltaRepository.ts
- [x] T029 [US2] Implement deterministic merge/replay service with canonical ordering in src/typescript/api/src/domains/persistent-world-orchestration/services/replayMergeService.ts
- [x] T030 [US2] Implement area state versioning and stale-update handling in src/typescript/api/src/domains/persistent-world-orchestration/services/areaStateVersionService.ts
- [x] T031 [US2] Add authoritative persistence + replay handlers to world routes in src/typescript/api/src/routes/world.ts
- [x] T032 [US2] Wire persistence source-of-record bootstrap requirements for world orchestration in src/typescript/api/src/services/databaseRuntime.ts
- [x] T033 [US2] Add native runtime apply-delta hook for canonical state updates in src/native/engine/runtime/terrain/terrain_mutation.c

**Checkpoint**: User Story 2 is independently functional and guarantees authoritative cross-player consistency with deterministic merge outcomes.

---

## Phase 5: User Story 3 - Deterministic Pipeline For Unexplored Areas (Priority: P2)

**Goal**: Guarantee deterministic unexplored-area generation under equivalent inputs and deterministic retry behavior when recoverable failures occur.

**Independent Test**: Request unexplored area C repeatedly with identical contract inputs, inject recoverable failures, retry deterministically, and verify parity without duplicate area identity.

### Tests for User Story 3

- [x] T034 [P] [US3] Add native deterministic unexplored-area parity test matrix in tests/native/runtime/terrain/runtime_terrain_unexplored_parity_test.c
- [x] T035 [P] [US3] Add native failure-injected deterministic retry test in tests/native/runtime/terrain/runtime_terrain_retry_determinism_test.c
- [x] T036 [P] [US3] Add API integration test for deterministic retry classification and operator context in src/typescript/api/src/integration/persistent-world-failure-retry.integration.test.ts

### Implementation for User Story 3

- [x] T037 [US3] Implement deterministic unexplored-area generation contract enforcement in src/native/engine/runtime/terrain/terrain_generation.c
- [x] T038 [P] [US3] Implement recoverable/non-recoverable failure classification in src/native/engine/runtime/terrain/terrain_host.c
- [x] T039 [US3] Implement deterministic retry fingerprint lineage tracking in src/native/engine/runtime/terrain/terrain_abi.c
- [x] T040 [US3] Implement API retry orchestration service for generation/persistence/replay failures in src/typescript/api/src/domains/persistent-world-orchestration/services/retryOrchestrationService.ts
- [x] T041 [US3] Add failure and retry orchestration endpoints in src/typescript/api/src/routes/world.ts
- [x] T042 [US3] Emit operator-facing non-recoverable context payloads in src/typescript/api/src/domains/persistent-world-orchestration/services/failureContextService.ts

**Checkpoint**: User Story 3 is independently functional and validates deterministic unexplored generation with deterministic retries.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Final hardening, evidence capture, and documentation updates across all stories.

- [x] T043 [P] Capture native deterministic baseline parity evidence in artifacts/native/009-deterministic-baseline-parity.txt
- [x] T044 [P] Capture native seam continuity evidence in artifacts/native/009-chunk-seam-continuity.txt
- [x] T045 [P] Capture API authoritative replay parity evidence in artifacts/api/009-authoritative-replay-parity.txt
- [x] T046 [P] Capture API cross-player merge consistency evidence in artifacts/api/009-cross-player-merge-consistency.txt
- [x] T047 [P] Capture failure and deterministic retry evidence in artifacts/api/009-failure-retry-determinism.txt
- [x] T048 Update deterministic validation runbook steps in .specify/specs/009-persistent-world-orchestration/quickstart.md

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies.
- **Phase 2 (Foundational)**: Depends on Phase 1 and blocks all user stories.
- **Phase 3 (US1)**: Depends on Phase 2.
- **Phase 4 (US2)**: Depends on Phase 2.
- **Phase 5 (US3)**: Depends on Phase 2 and can run in parallel with US2 after deterministic contracts are in place.
- **Phase 6 (Polish)**: Depends on completion of US1, US2, and US3.

### User Story Dependencies

- **US1 (P1)**: First MVP slice after foundational work.
- **US2 (P1)**: No dependency on US1 implementation; shares foundational deterministic contracts only.
- **US3 (P2)**: Uses foundational deterministic contracts and can proceed after Phase 2, but final sign-off waits for US1 and US2 evidence.

### Within Each User Story

- Implement tests before implementation tasks and confirm they fail first.
- Implement core domain/repository logic before route handlers.
- Wire native hooks before parity/failure evidence capture.

### Parallel Opportunities

- Phase 1: T002 can run in parallel with T001/T003.
- Phase 2: T006, T008, T010, and T012 can run in parallel after T005/T007 start.
- US1: T014, T015, and T016 can run in parallel.
- US2: T024, T025, and T026 can run in parallel; T028 can run in parallel with T027.
- US3: T034, T035, and T036 can run in parallel; T038 can run in parallel with T037.
- Polish: T043-T047 can run in parallel.

---

## Parallel Example: User Story 1

- Task: T014 [US1] tests/native/runtime/terrain/runtime_terrain_generation_revisit_parity_test.c
- Task: T015 [US1] tests/native/terrain/terrain_chunk_seam_continuity_test.c
- Task: T016 [US1] src/typescript/api/src/integration/persistent-world-revisit-parity.integration.test.ts

---

## Parallel Example: User Story 2

- Task: T024 [US2] src/typescript/api/src/routes/world.persistent.contract.test.ts
- Task: T025 [US2] src/typescript/api/src/integration/persistent-world-cross-player-parity.integration.test.ts
- Task: T026 [US2] src/typescript/api/src/integration/persistent-world-stale-update.integration.test.ts

---

## Parallel Example: User Story 3

- Task: T034 [US3] tests/native/runtime/terrain/runtime_terrain_unexplored_parity_test.c
- Task: T035 [US3] tests/native/runtime/terrain/runtime_terrain_retry_determinism_test.c
- Task: T036 [US3] src/typescript/api/src/integration/persistent-world-failure-retry.integration.test.ts

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1 and Phase 2.
2. Complete US1 (Phase 3).
3. Validate revisit parity and viewport continuity using T014-T016.
4. Capture initial evidence artifacts and demo deterministic revisit continuity.

### Incremental Delivery

1. Deliver MVP with US1.
2. Add US2 for authoritative baseline+delta replay/merge and stale-update handling.
3. Add US3 for deterministic unexplored generation and retry hardening.
4. Run Phase 6 evidence capture and update quickstart runbook.

### Parallel Team Strategy

1. Team A: Native deterministic contracts and parity controls (T005-T010).
2. Team B: API normalization/contracts and US1/US2 route-service orchestration (T011-T033).
3. Team C: Retry/failure hardening and validation matrix (T034-T047).

---

## Notes

- [P] tasks are designed to avoid file conflicts when executed concurrently.
- Story labels preserve traceability from spec.md user stories to implementation work.
- This plan keeps deterministic seed derivation, area identity/chunk partitioning, mesh vector parity, authoritative replay/merge, stale-update handling, and deterministic retry validation explicit and independently testable.

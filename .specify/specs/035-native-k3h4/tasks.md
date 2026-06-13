---

description: "Tasks for 035-native-k3h4"
---

# Tasks: Native K3H4 Analytics

**Input**: Design documents from `.specify/specs/035-native-k3h4/`

**Prerequisites**: `plan.md`, `spec.md`, `research.md`, `data-model.md`, `contracts/native-k3h4-abi.md`, `contracts/api-netcode-k3h4.md`

**Tests**: Native C, API contract/integration, and React consumer contract behavior tests are required for this feature.

**Organization**: Tasks are grouped by user story so each story can be implemented and validated independently.

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Establish feature scaffolding and validation entry points.

- [x] T001 Create feature fixture directories for native/API/React validation in tests/native/runtime/netcode/fixtures/, src/typescript/api/src/routes/__fixtures__/netcode/, and src/typescript/react/src/domain/notebook/__fixtures__/
- [x] T002 [P] Add feature rollout environment variable notes and command matrix updates in .specify/specs/035-native-k3h4/quickstart.md
- [x] T003 [P] Add feature evidence artifact index for deterministic and rollout captures in artifacts/native/k3h4/README.md

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Build shared contract scaffolding required by all user stories.

**CRITICAL**: No US1/US2/US3 implementation tasks start before this phase completes.

- [x] T004 Define shared native envelope/version/error declarations in src/native/include/banana_native_v3.h and src/native/engine/runtime/abi/netcode/netcode_abi.h
- [x] T005 [P] Introduce API rollout gate plumbing for BANANA_NETCODE_K3H4_ENABLED and BANANA_NETCODE_K3H4_COHORT in src/typescript/api/src/routes/netcode.ts
- [x] T006 [P] Add baseline TypeScript contract placeholders for k3h4 payload shapes in src/typescript/api/src/services/nativeNetcode.ts and src/typescript/react/src/domain/notebook/network-domain.ts
- [x] T007 Register new native test executables and ctest entries for k3h4 suites in tests/native/CMakeLists.txt

**Checkpoint**: Shared scaffolding is ready; user stories can proceed in priority order.

---

## Phase 3: User Story 1 - Deterministic Compute (Priority: P1) 🎯 MVP

**Goal**: Deliver deterministic fixed-point K-means compute, inscribed radius geometry, weighted Voronoi scoring, and spectral proxy outputs.

**Independent Test**: Repeated identical requests produce byte-identical deterministic payload fields and deterministic hash values.

### Tests for User Story 1

- [x] T008 [P] [US1] Add deterministic native compute repeatability test in tests/native/runtime/netcode/netcode_k3h4_determinism_test.c
- [x] T009 [P] [US1] Add native edge-case test for single-cluster and near-zero-radius handling in tests/native/runtime/netcode/netcode_k3h4_edge_cases_test.c

### Implementation for User Story 1

- [x] T010 [P] [US1] Create fixed-point math and deterministic rounding helpers in src/native/engine/runtime/netcode/vector/netcode_fixed_point.h and src/native/engine/runtime/netcode/vector/netcode_fixed_point.c
- [x] T011 [P] [US1] Implement deterministic K-means iteration core and stable assignment ordering in src/native/engine/runtime/netcode/vector/netcode_k3h4.h and src/native/engine/runtime/netcode/vector/netcode_k3h4.c
- [x] T012 [US1] Integrate K-means core into vector compute orchestration in src/native/engine/runtime/netcode/vector/netcode_vector.c and src/native/engine/runtime/netcode/vector/netcode_vector.h
- [x] T013 [US1] Extend K3H4 output model with inscribed radius, weighted Voronoi scores, and spectral proxies in src/native/engine/runtime/netcode/k3h4/netcode_k3h4_metrics.h and src/native/engine/runtime/netcode/k3h4/netcode_k3h4_metrics.c
- [x] T014 [US1] Extend native ABI bridge structs for deterministic compute outputs in src/native/engine/runtime/abi/netcode/netcode_abi.h and src/native/engine/runtime/abi/netcode/netcode_abi.c
- [x] T015 [US1] Expose new compute outputs through exported ABI boundary in src/native/include/banana_native_v3.h and src/native/scaffold/native_entry.c
- [x] T016 [US1] Wire new netcode fixed-point and k3h4 runtime modules into build in src/native/engine/CMakeLists.txt
- [x] T017 [US1] Register US1 deterministic native tests in tests/native/CMakeLists.txt

**Checkpoint**: Deterministic native compute is complete and independently testable.

---

## Phase 4: User Story 2 - ABI Contract Reliability (Priority: P2)

**Goal**: Guarantee endianness-safe, versioned, and deterministic ABI/API contract behavior with explicit failures.

**Independent Test**: Mixed-endianness fixtures round-trip successfully, while malformed and unsupported-version payloads fail with deterministic error contracts.

### Tests for User Story 2

- [x] T018 [P] [US2] Add native mixed-endianness envelope conformance test in tests/native/runtime/netcode/netcode_abi_envelope_endianness_test.c
- [x] T019 [P] [US2] Add native malformed payload and unsupported version test in tests/native/runtime/netcode/netcode_abi_error_contract_test.c
- [x] T020 [P] [US2] Expand API contract tests for /api/netcode/analytics k3h4 success/error payloads in src/typescript/api/src/routes/netcode.contract.test.ts
- [x] T021 [P] [US2] Add API integration test for FFI decode failures and rollout-off fallback behavior in src/typescript/api/src/routes/netcode.integration.test.ts

### Implementation for User Story 2

- [x] T022 [US2] Implement little-endian envelope encode/decode with CRC, payload length, and deterministic status handling in src/native/engine/runtime/abi/netcode/netcode_abi.c
- [x] T023 [US2] Finalize ABI envelope constants, status enums, and payload section structs in src/native/engine/runtime/abi/netcode/netcode_abi.h
- [x] T024 [US2] Propagate contract version/error states through exported native entrypoints in src/native/include/banana_native_v3.h and src/native/scaffold/native_entry.c
- [x] T025 [US2] Update FFI decoding and type mapping for k3h4 envelope payloads in src/typescript/api/src/services/nativeNetcode.ts
- [x] T026 [US2] Add fail-fast coverage for unsupported version, CRC mismatch, and truncated payload handling in src/typescript/api/src/services/nativeNetcode.fail-fast.test.ts
- [x] T027 [US2] Update /api/netcode/analytics response shaping, deterministic ordering, and rollout controls in src/typescript/api/src/routes/netcode.ts
- [x] T028 [US2] Register US2 ABI reliability native tests in tests/native/CMakeLists.txt

**Checkpoint**: Cross-boundary contract reliability and deterministic error behavior are independently validated.

---

## Phase 5: User Story 3 - Presentation-Only Consumer (Priority: P3)

**Goal**: Ensure React renders orchestrated k3h4 outputs without local production recomputation fallback.

**Independent Test**: Consumer tests show rendered values always come from API payloads and unavailable states are explicit when compute is absent.

### Tests for User Story 3

- [x] T029 [P] [US3] Add React consumer contract test for rendering API-provided k3h4 values in src/typescript/react/src/domain/notebook/useNetcodeSession.test.ts
- [x] T030 [P] [US3] Add React fallback-guard test proving no local production recompute in src/typescript/react/src/domain/notebook/network-domain.test.ts

### Implementation for User Story 3

- [x] T031 [US3] Extend notebook domain contract types for centers/radii/weighted scores/spectral proxy/observability in src/typescript/react/src/domain/notebook/network-domain.ts
- [x] T032 [US3] Update session hook mapping for k3h4 payload consumption and unavailable-state handling in src/typescript/react/src/domain/notebook/useNetcodeSession.ts
- [x] T033 [US3] Update API client response typing for contractVersion and k3h4 payloads in src/typescript/react/src/lib/api.ts
- [x] T034 [US3] Update notebook presentation panels to consume orchestrated metrics only in src/typescript/react/src/components/notebook-client/NotebookOperationsPanel.tsx and src/typescript/react/src/components/notebook-client/NotebookGameplaySurface.tsx

**Checkpoint**: Presentation-only consumer behavior is complete and independently testable.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Final validation, documentation alignment, and evidence capture across domains.

- [x] T035 Update feature runbook and rollout/rollback drill details in .specify/specs/035-native-k3h4/quickstart.md
- [x] T036 [P] Capture native/API/React validation evidence references in artifacts/native/k3h4/README.md and artifacts/api/README.md
- [x] T037 [P] Update onboarding and contract-flow documentation for deterministic netcode analytics in docs/developer-onboarding.md

---

## Phase 7: Reorchestration Follow-Up

**Purpose**: Align the implemented K3H4 pipeline with the dual-mode, spectral-gated, hardware-first operating model from the refactoring report.

- [x] T038 Define explicit K3H4 mode selection contract (`multiplicative | power`) in native request structs, FFI types, and API request shaping.
- [x] T039 Move byte-order, dtype, and alignment declaration checks into a required preflight stage before any clustering entrypoint executes.
- [x] T040 Split multiplicative and power scoring into explicit native scoring paths with deterministic mode-specific tests.
- [x] T041 Add orchestration gating so spectral embedding runs only when an affinity-graph-backed mode requests it.
- [x] T042 Extend API analytics responses with selected K3H4 mode and spectral activation metadata.
- [x] T043 Update React notebook consumer models to display returned mode/spectral metadata without local decision-making.
- [x] T044 Add validation coverage for hardware-preflight failures, multiplicative vs power divergence, and spectral-bypass vs spectral-enabled flows.

---

## Dependencies & Execution Order

### Phase Dependencies

- Phase 1 (Setup) has no dependencies.
- Phase 2 (Foundational) depends on Phase 1 and blocks all user story phases.
- Phase 3 (US1) depends on Phase 2.
- Phase 4 (US2) depends on Phase 3 outputs for envelope payload shape and deterministic fields.
- Phase 5 (US3) depends on Phase 4 API contract finalization.
- Phase 6 (Polish) depends on completion of desired user stories.

### User Story Dependencies

- US1 (P1) is the MVP and has no user-story dependency.
- US2 (P2) depends on US1 deterministic compute fields being available in native output.
- US3 (P3) depends on US2 API contract reliability and rollout behaviors.

### Within Each User Story

- Test tasks execute before implementation tasks.
- Data/model changes precede bridge/service/route updates.
- Native ABI export updates complete before TypeScript consumer mapping updates.

## Parallel Execution Examples

### US1

- Run T008 and T009 in parallel.
- Run T010 and T011 in parallel.

### US2

- Run T018, T019, T020, and T021 in parallel.
- Run T022 and T023 sequentially before T025 and T027.

### US3

- Run T029 and T030 in parallel.
- Run T031 and T033 in parallel, then execute T032 and T034.

## Implementation Strategy

### MVP First

1. Complete Phase 1 and Phase 2.
2. Complete US1 (Phase 3) and validate deterministic compute.
3. Stop for MVP demo/validation before expanding scope.

### Incremental Delivery

1. Deliver US1 deterministic native compute.
2. Deliver US2 ABI/API reliability and rollout safety.
3. Deliver US3 presentation-only consumer behavior.
4. Finish with Phase 6 polish and evidence capture.

## Notes

- All tasks use exact Banana repository paths.
- [P] indicates tasks safe for parallel execution because they touch different files or independent tests.
- User story phases remain independently testable at each checkpoint.

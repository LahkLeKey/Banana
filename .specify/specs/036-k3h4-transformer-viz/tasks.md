---

description: "Tasks for 036-k3h4-transformer-viz"
---

# Tasks: K3H4 Transformer-Comparative Visualizations

**Input**: Design documents from `.specify/specs/036-k3h4-transformer-viz/`

**Prerequisites**: `plan.md`, `spec.md`, `research.md`, `data-model.md`, `contracts/native-k3h4-export-abi.md`, `contracts/api-k3h4-viz-endpoints.md`, `quickstart.md`

**Tests**: Native CTest, API contract/unit, and React component tests are required for this feature per the constitution quality gates.

**Organization**: Tasks are grouped by user story so each story can be implemented and validated independently. US2 and US4 share P2 priority; US4 is sequenced after US2 because both consume the training artifact endpoint and mode field, enabling shared fixture reuse.

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Establish feature scaffolding, fixture directories, and gitignore hygiene for generated artifact paths.

- [ ] T001 Create fixture directories for native/API/React validation: `tests/native/k3h4/`, `src/typescript/api/src/tests/fixtures/k3h4/`, and `src/typescript/react/src/domain/notebook/viz/__fixtures__/`
- [ ] T002 [P] Add gitignore entries for generated artifact paths `artifacts/native/k3h4-scaling-benchmark.json` and `artifacts/native/k3h4-training/` in `.gitignore`
- [ ] T003 [P] Update `.specify/specs/036-k3h4-transformer-viz/quickstart.md` with completed validation entry points (CTest, API tests, compose steps, curl smoke)

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Extend the ABI envelope, declare the export hook interface, wire CMake targets, and scaffold TypeScript/React types. No user story implementation begins before this phase completes.

**CRITICAL**: All phases 3–6 depend on this phase.

- [ ] T004 Extend `src/native/engine/runtime/abi/netcode/netcode_abi.h` — add `RUNTIME_K3H4_ARTIFACT_TYPE_TRAINING = 0x01` discriminant, `BananaK3h4TrainingArtifactHeader` struct (23-byte fixed header matching binary layout in `contracts/native-k3h4-export-abi.md`), and `ClusterRecord` struct
- [ ] T005 [P] Implement `banana_k3h4_artifact_encode()`, `banana_k3h4_artifact_validate()`, `banana_k3h4_artifact_write_trailer()`, and `banana_k3h4_artifact_check_trailer()` helpers in `src/native/engine/runtime/abi/netcode/netcode_abi.c`
- [ ] T006 [P] Create `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_export.h` — declare `BananaK3h4ExportStatus` enum (`OK`, `PREFLIGHT_FAILED`, `WRITE_FAILED`, `INVALID_SESSION`, `VERSION_UNSUPPORTED`) and `banana_k3h4_export_training_artifact()` function signature per the ABI contract
- [ ] T007 Register new native source files in CMake: add `netcode_k3h4_export.c` and `netcode_k3h4_benchmark.c` to the `netcode_k3h4` CMake target in `src/native/engine/CMakeLists.txt`; register placeholder `banana_k3h4_export_hook_*` and `banana_k3h4_scaling_benchmark` test executables in `tests/native/CMakeLists.txt`
- [ ] T008 [P] Scaffold `src/typescript/api/src/services/k3h4TrainingService.ts` — declare TypeScript types only (no implementation): `ScalingBenchmarkResult`, `ScalingSeriesEntry`, `TrainingSession`, `TrainingSessionMode`, `ConfidenceTimeSeries`, `EpochConfidence`, `SessionStatus`, `EpochGeometry`, `ClusterGeometry2d`, `ProjectionMetadata`, `K3h4VizServiceError`
- [ ] T009 [P] Scaffold `src/typescript/react/src/domain/notebook/k3h4/k3h4PresentationDomain.ts` — add presentation types mirroring API response shapes: `ScalingSeriesEntry`, `EpochGeometry`, `ClusterGeometry2d`, `ProjectionMetadata`, `ConfidenceTimeSeries`, `TokenScore` (types only, no compute logic)

**Checkpoint**: ABI contracts, export interface, CMake wiring, and TypeScript type scaffolding are ready; all user story phases can begin.

---

## Phase 3: User Story 1 — Scaling Curve Chart (Priority: P1) 🎯 MVP

**Goal**: Deliver measured log-log scaling comparison chart showing k3h4 vs transformer attention cost at five input sizes, driven by native CTest benchmark outputs surfaced through the API.

**Independent Test**: Run `banana_k3h4_scaling_benchmark` CTest to generate `artifacts/native/k3h4-scaling-benchmark.json`, call `GET /api/netcode/k3h4/scaling-benchmark`, verify the chart renders both labeled series with slope annotations within 3 seconds.

### Tests for User Story 1

- [ ] T010 [P] [US1] Add CTest scaling benchmark test in `tests/native/k3h4/test_k3h4_scaling_benchmark.c` — validates: benchmark writes JSON with all five `n` values, k3h4 slope within ±5% of 1.0 on log-log axis, attention slope within ±5% of 2.0, re-run produces times within ±10% of prior run
- [ ] T011 [P] [US1] Extend `src/typescript/api/src/routes/netcode.contract.test.ts` with `GET /api/netcode/k3h4/scaling-benchmark` cases — happy path (all five entries, camelCase fields), `benchmark_not_found` (404 when JSON missing), `benchmark_unavailable` (503 on I/O error)
- [ ] T012 [P] [US1] Add `src/typescript/react/src/domain/notebook/viz/ScalingCurveChart.test.tsx` — asserts: renders with ≥ 5 data points per series, both series labeled "Transformer Attention" and "k3h4 Power-Mode", slope annotations present, renders loading state when series is empty, no k3h4 math functions called during render

### Implementation for User Story 1

- [ ] T013 [P] [US1] Implement `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_benchmark.c` — measure wall-clock via `clock_gettime(CLOCK_MONOTONIC)` for k3h4 power-mode pipeline and O(n²) attention proxy at n∈{64,256,1024,4096,16384}, write atomic JSON to `artifacts/native/k3h4-scaling-benchmark.json`, assert slope bounds inline
- [ ] T014 [US1] Register `banana_k3h4_scaling_benchmark` CTest target with slope-assertion output in `tests/native/CMakeLists.txt`
- [ ] T015 [US1] Implement `readScalingBenchmark(): Promise<ScalingBenchmarkResult>` in `src/typescript/api/src/services/k3h4TrainingService.ts` — reads + parses `artifacts/native/k3h4-scaling-benchmark.json`; throws `benchmark_not_found` if missing, `benchmark_unavailable` on I/O error; uses async `fs/promises` only
- [ ] T016 [US1] Register `GET /api/netcode/k3h4/scaling-benchmark` route and `handleScalingBenchmark` handler in `src/typescript/api/src/routes/netcode.ts` — handler calls `readScalingBenchmark()`, serializes response with sorted keys
- [ ] T017 [US1] Implement `src/typescript/react/src/domain/notebook/viz/ScalingCurveChart.tsx` — log-log chart, two labeled series from API `series[]`, slope annotation computed from log-linear regression on measured data, responsive layout (no label overlap on resize), loading/error state when series is absent
- [ ] T018 [US1] Wire `ScalingCurveChart` into the notebook domain view routing in `src/typescript/react/src/domain/notebook/` — add route entry and lazy import; no client-side data computation

**Checkpoint**: Scaling curve chart is independently deployable — CTest benchmark produces JSON, API serves it, React renders the log-log chart.

---

## Phase 4: User Story 2 — Hypersphere Geometry Overlay (Priority: P2)

**Goal**: Deliver SVG overlay of Voronoi cell boundaries, inscribed radius circles, and projected transformer token positions for a given training epoch artifact, with cluster selection and PCA metadata annotation.

**Independent Test**: Load static `epoch-0.bin` fixture via `GET .../epoch/0/geometry`, verify overlay renders correct Voronoi cell count, radius circles, and token positions; verify epoch-switch prop update triggers re-render within 2 seconds without full page reload.

### Tests for User Story 2

- [ ] T019 [P] [US2] Add `tests/native/k3h4/test_k3h4_export_hook.c` covering: `test_export_creates_artifact` (happy path — write, validate integrity marker + CRC), `test_export_preflight_rejected` (no file created on preflight failure), `test_export_partial_write_detection` (truncated file → `artifact_incomplete`), `test_export_deterministic` (same input state → byte-identical output), `test_export_version_rejection` (unknown ABI version → `VERSION_UNSUPPORTED`), `test_export_session_id_validation` (session_id > 15 chars → `INVALID_SESSION`)
- [ ] T020 [P] [US2] Extend `src/typescript/api/src/routes/netcode.contract.test.ts` with `POST /api/netcode/k3h4/training-session` (201 with sessionId + mode + createdAtUtc) and `GET .../epoch/:n/geometry` cases — happy path, `session_not_found` (404), `artifact_not_found` (404), `artifact_incomplete` (422), `version_unsupported` (422), `invalid_mode` (400)
- [ ] T021 [P] [US2] Create `src/typescript/api/src/tests/k3h4-viz-determinism.contract.test.ts` — calls `GET .../epoch/:n/geometry` twice with identical `(session, epoch, mode)` inputs, asserts byte-identical JSON response bodies (validates SC-005 / FR-013)
- [ ] T022 [P] [US2] Add `src/typescript/react/src/domain/notebook/viz/HypersphereGeometryOverlay.test.tsx` — verifies: Voronoi cell count matches `geometry.clusters.length`, radius circles sized to `inscribedRadius`, PCA metadata annotation displayed, cluster selection highlights contained tokens and shows Voronoi score, epoch-switch prop change triggers re-render (not full page reload), no k3h4 math called during render

### Implementation for User Story 2

- [ ] T023 [P] [US2] Implement `src/native/engine/runtime/netcode/k3h4/netcode_k3h4_export.c` — `banana_k3h4_export_training_artifact()`: reuse 035 hardware preflight, validate session_id (≤ 15 chars, alphanumeric + hyphen), create `artifacts/native/k3h4-training/<session_id>/` directory, serialize `RuntimeNetcodeK3h4Output` to `TrainingArtifact` binary layout (ascending `center[0]` cluster order), write to `.tmp` file, append `0xDEADBEEF` + CRC32 (ISO 3309), rename to final path; unlink `.tmp` on any failure
- [ ] T024 [US2] Register `banana_k3h4_export_hook_determinism`, `banana_k3h4_export_hook_preflight`, `banana_k3h4_export_hook_integrity`, `banana_k3h4_preflight_blocks_export` CTest targets in `tests/native/CMakeLists.txt`
- [ ] T025 [US2] Implement `createTrainingSession()` and `readEpochGeometry()` in `src/typescript/api/src/services/k3h4TrainingService.ts` — `createTrainingSession()` creates session directory + metadata JSON, returns `TrainingSession`; `readEpochGeometry()` reads + decodes `epoch-<N>.bin`, validates ABI version + integrity marker + CRC32, applies 2D PCA projection to cluster centers, populates `projectionMetadata` (method, components, explainedVariance); throws structured `K3h4VizServiceError` for all error paths
- [ ] T026 [US2] Register `POST /api/netcode/k3h4/training-session` and `GET /api/netcode/k3h4/training-session/:id/epoch/:n/geometry` routes and handlers in `src/typescript/api/src/routes/netcode.ts` — mode param default `"power"`, 400 for invalid mode, no business logic in handlers
- [ ] T027 [US2] Implement `src/typescript/react/src/domain/notebook/viz/HypersphereGeometryOverlay.tsx` — SVG rendering: power-mode cells as distance²−radius² bisector convex cells; multiplicative-mode cells as non-convex variant (mode read from `geometry.mode`); inscribed radius circles; projected token positions; `projectionMetadata` annotation below overlay; selected cluster highlights tokens + shows Voronoi score tooltip; receives new epoch via `geometry` prop (no full reload)
- [ ] T028 [US2] Wire `HypersphereGeometryOverlay` into notebook domain with epoch-selector prop pattern in `src/typescript/react/src/domain/notebook/` — fetch geometry on epoch change, pass as `geometry` prop

**Checkpoint**: Export hook, geometry API, and overlay component are independently testable against static fixture artifacts.

---

## Phase 5: User Story 4 — Mode-Differentiation View (Priority: P2)

**Goal**: Deliver side-by-side comparison of multiplicative vs power score distributions for the same input batch, with divergent-token highlighting, cross-panel selection, and degenerate-result annotation.

**Independent Test**: Call geometry endpoint twice with same batch (once per mode), verify both score distributions returned; verify React renders both distributions side by side on same x-axis scale with divergent tokens visually distinguished in both panels.

### Tests for User Story 4

- [ ] T029 [P] [US4] Extend `src/typescript/api/src/routes/netcode.contract.test.ts` with mode parameter validation cases: `mode=multiplicative` returns multiplicative scores, `mode=power` returns power scores, omitted `mode` defaults to `"power"` with `metadata.mode="power"` in response, `mode=invalid` returns 400 `invalid_mode`
- [ ] T030 [P] [US4] Add `src/typescript/react/src/domain/notebook/viz/ModeDifferentiationView.test.tsx` — verifies: both panels render on same x-axis scale and same token order, tokens with divergent cluster assignment are visually distinguished in both panels simultaneously, selecting a token in one panel highlights it in the other, degenerate case (identical distributions) annotates "degenerate result — mode separation not observed", no k3h4 math called during render

### Implementation for User Story 4

- [ ] T031 [US4] Implement `src/typescript/react/src/domain/notebook/viz/ModeDifferentiationView.tsx` — side-by-side bar/point charts from `multiplicativeScores: TokenScore[]` and `powerScores: TokenScore[]` props; same x-axis scale and token order; divergent-cluster-assignment tokens colored distinctly in both panels; cross-panel selection via shared selection state; degenerate distribution detection + annotation
- [ ] T032 [US4] Add `TokenScore` type and dual-mode fetch helper (fetches same session/epoch twice with different mode params) in `src/typescript/react/src/domain/notebook/k3h4/k3h4PresentationDomain.ts`
- [ ] T033 [US4] Wire `ModeDifferentiationView` into notebook domain with dual-mode fetch pattern in `src/typescript/react/src/domain/notebook/` — fetch multiplicative + power geometry for same epoch, extract `TokenScore[]` from both, pass as props

**Checkpoint**: Mode-differentiation view is independently testable against the static geometry fixture from Phase 4.

---

## Phase 6: User Story 3 — Training Confidence Dashboard (Priority: P3)

**Goal**: Deliver per-epoch ML confidence time-series chart with peak epoch annotation and rolling 3-epoch trend, polled at configurable interval, with explicit waiting and error states.

**Independent Test**: Seed two canned epoch artifacts for `test-session`, start polling consumer against mock endpoint, verify dashboard renders both epoch confidence values with peak annotation; seed zero epochs and verify waiting state renders with session ID and pending message.

### Tests for User Story 3

- [ ] T034 [P] [US3] Extend `src/typescript/api/src/routes/netcode.contract.test.ts` with `GET .../confidence` cases — active session (epochs array + status "active" + peakEpoch + rollingAverage3), pending session (empty epochs + status "pending", NOT 404), unknown session (404 `session_not_found`), invalid mode (400 `invalid_mode`)
- [ ] T035 [P] [US3] Create `src/typescript/api/src/services/k3h4TrainingService.test.ts` — unit tests for `readConfidenceTimeSeries()`: active session enumerates epoch files in ascending order, pending session returns `{status:"pending", epochs:[]}`, partial artifact is omitted from series without error, `session_not_found` error for unknown session ID
- [ ] T036 [P] [US3] Add `src/typescript/react/src/domain/notebook/viz/TrainingConfidenceDashboard.test.tsx` — verifies: time-series chart renders epoch data from API response, peak epoch annotated, rolling 3-epoch trend line shown when ≥ 3 epochs, waiting state rendered with session ID when status is "pending", error state rendered without crash when session is not found, polling stops when status is "completed", poll interval configurable via `VITE_K3H4_CONFIDENCE_POLL_MS` env var

### Implementation for User Story 3

- [ ] T037 [US3] Implement `readConfidenceTimeSeries(sessionId, mode): Promise<ConfidenceTimeSeries>` in `src/typescript/api/src/services/k3h4TrainingService.ts` — enumerate `epoch-*.bin` files in session directory in ascending epoch order, decode confidence scalar (mean weighted Voronoi score from fixed-point fields), return ordered `EpochConfidence[]` with `status` field; partial artifact silently omitted; missing session → throws `session_not_found`
- [ ] T038 [US3] Register `GET /api/netcode/k3h4/training-session/:id/confidence` route and `handleGetConfidence` handler in `src/typescript/api/src/routes/netcode.ts` — mode param default `"power"`, returns `{contractVersion, sessionId, status, mode, epochs[], metadata:{peakEpoch, rollingAverage3}}`
- [ ] T039 [US3] Implement `src/typescript/react/src/domain/notebook/viz/TrainingConfidenceDashboard.tsx` — polls `GET .../confidence` at interval from `VITE_K3H4_CONFIDENCE_POLL_MS` env var (default 4000 ms) with exponential backoff on consecutive empty responses; renders time-series line chart; annotates peak epoch; shows rolling 3-epoch trend line; renders waiting state (session ID + "first epoch pending") when `status === "pending"`; renders structured error state without crash when session not found; poll stops when `status === "completed"`
- [ ] T040 [US3] Wire `TrainingConfidenceDashboard` into notebook domain with session-keyed routing in `src/typescript/react/src/domain/notebook/` — session ID passed as route param or prop; no client-side confidence computation

**Checkpoint**: Confidence dashboard is independently testable — mock the confidence endpoint and verify the full polling + render + annotation flow.

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Integration validation, delivery evidence capture, and documentation alignment. Validation tasks mirror spec.md success criteria SC-001 through SC-008 and constitution quality gates 1:1.

- [ ] T041 [US1] Validate SC-001: measure scaling curve chart render time — verify `ScalingCurveChart` renders within 3 seconds of notebook view open under normal API response conditions; add performance assertion to `src/typescript/react/src/domain/notebook/viz/ScalingCurveChart.test.tsx`
- [ ] T042 [US1] Validate SC-002: run `banana_k3h4_scaling_benchmark` CTest and assert transformer series log-log slope is within ±5% of 2.0 and k3h4 series slope is within ±5% of 1.0 from `artifacts/native/k3h4-scaling-benchmark.json` output; assert self-reproducibility within ±10% across two consecutive runs
- [ ] T043 [US2] Validate SC-003: verify `HypersphereGeometryOverlay` re-renders within 2 seconds when `geometry` prop changes to next epoch — add epoch-switch render-timing assertion to `src/typescript/react/src/domain/notebook/viz/HypersphereGeometryOverlay.test.tsx`
- [ ] T044 [US3] Validate SC-004: confidence dashboard integration test — write two canned `epoch-0.bin` and `epoch-1.bin` artifacts to `artifacts/native/k3h4-training/test-session/`, start polling consumer, assert `ConfidenceTimeSeries` reflects both epochs within 5 seconds in `src/typescript/api/src/services/k3h4TrainingService.test.ts`
- [ ] T045 Validate SC-005: run cross-layer determinism drill — call `GET /api/netcode/k3h4/scaling-benchmark` twice and diff response bodies; call `GET .../epoch/0/geometry` twice with identical params and diff response bodies; assert byte-identity in `src/typescript/api/src/tests/k3h4-viz-determinism.contract.test.ts`
- [ ] T046 [US4] Validate SC-006: run mode-differentiation fixture test with known-divergent input batch — assert `ModeDifferentiationView` correctly identifies and visually distinguishes all tokens with divergent cluster assignments between multiplicative and power modes in `src/typescript/react/src/domain/notebook/viz/ModeDifferentiationView.test.tsx`
- [ ] T047 [US2] Validate SC-007: run native export determinism test (`test_export_deterministic` in `tests/native/k3h4/test_k3h4_export_hook.c`) on both Windows (MSVC) and Linux (GCC/Clang) CI targets and compare artifact CRC32 checksums — must be byte-identical across platforms
- [ ] T048 [US3] Validate SC-008: reference training fixture — run 5 epoch writes to `artifacts/native/k3h4-training/ref-session/`, call `readConfidenceTimeSeries()`, assert confidence values are monotonically non-decreasing across all 5 epochs in `src/typescript/api/src/services/k3h4TrainingService.test.ts`
- [ ] T049 [P] Validate FR-018 production-only gate: run `NODE_ENV=production` Vite build and assert build output contains no references to native FFI clustering paths from `src/domain/notebook/viz/` components; add build-gate note to `src/typescript/react/src/domain/notebook/viz/` README
- [ ] T050 [P] Validate NFR-004 + ABI version rejection: native test with artifact containing unknown version byte returns `VERSION_UNSUPPORTED`; API test with same fixture returns 422 `version_unsupported`; covered by `test_export_version_rejection` in `tests/native/k3h4/test_k3h4_export_hook.c` and `src/typescript/api/src/routes/netcode.contract.test.ts`
- [ ] T051 [P] Capture delivery evidence artifact index in `artifacts/native/README.md` — document paths for `k3h4-scaling-benchmark.json`, `k3h4-training/<session_id>/epoch-<N>.bin`, CTest XML output location (`out/v3-native/Testing/`), and determinism test pass reference
- [ ] T052 [P] Update developer onboarding documentation in `docs/developer-onboarding.md` — add k3h4 viz endpoint surface, training artifact write/read flow, and quickstart.md cross-reference

---

## Dependencies & Execution Order

### Phase Dependencies

- Phase 1 (Setup): No dependencies — start immediately.
- Phase 2 (Foundational): Depends on Phase 1. **Blocks all user story phases.**
- Phase 3 (US1 — Scaling Curve, P1): Depends on Phase 2. No dependency on US2/US4/US3.
- Phase 4 (US2 — Geometry Overlay, P2): Depends on Phase 2. Depends on Phase 3 for benchmark endpoint pattern reuse.
- Phase 5 (US4 — Mode-Differentiation, P2): Depends on Phase 4 (reuses training artifact endpoint + mode field).
- Phase 6 (US3 — Confidence Dashboard, P3): Depends on Phase 4 (session + epoch artifact infrastructure).
- Phase 7 (Polish & Validation): Depends on all story phases (3–6).

### User Story Dependencies

- **US1 (P1)**: Can start immediately after Phase 2. No dependency on other user stories.
- **US2 (P2)**: Can start immediately after Phase 2. The geometry overlay is independently testable against static fixture artifacts.
- **US4 (P2)**: Depends on US2's training artifact endpoint and mode field being registered. Share test fixtures.
- **US3 (P3)**: Depends on US2's `createTrainingSession()` and epoch artifact infrastructure in `k3h4TrainingService.ts`.

### Within Each User Story

- Tests run in parallel (all marked `[P]`) — write against contracts before implementation.
- Native implementation before CTest target registration.
- Service implementation before route handler registration.
- Route handler registered before React component wired to live endpoint.
- Component implemented before notebook domain wiring.

### Parallel Opportunities Per Story

| Story | Parallelizable work |
|---|---|
| US1 | T010/T011/T012 (tests) + T013 (native impl) can all run in parallel against contract |
| US2 | T019/T020/T021/T022 (tests) + T023 (native impl) can all run in parallel |
| US4 | T029/T030 (tests) can run in parallel; T031/T032 can run in parallel |
| US3 | T034/T035/T036 (tests) can run in parallel; T037 + T039 can run in parallel |
| Phase 7 | T049/T050/T051/T052 can all run in parallel |

### Implementation Strategy

**MVP = Phase 3 (US1) only.** The scaling curve chart is independently deployable — it requires only native benchmark, one API endpoint, and one React component. No training session lifecycle needed. Delivers the core k3h4 vs transformer cost argument as a standalone research artifact.

**Phase 4 (US2)** unlocks all artifact-backed features (geometry overlay, mode-differentiation, confidence dashboard) because it introduces the native export hook and training artifact infrastructure shared by US2, US4, and US3.

**Suggested delivery sequence**:
1. Phase 1–2 (setup + foundations): 1 sprint
2. Phase 3 (US1 MVP): 1 sprint → deployable scaling chart
3. Phase 4 (US2 geometry): 1 sprint → native export + geometry overlay
4. Phase 5 (US4 mode-diff): ½ sprint → mode selection UI
5. Phase 6 (US3 confidence): ½ sprint → confidence dashboard
6. Phase 7 (validation): ½ sprint → evidence + cross-platform CI gate

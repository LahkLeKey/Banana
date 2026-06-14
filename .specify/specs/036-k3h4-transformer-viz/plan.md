# Implementation Plan: K3H4 Transformer-Comparative Visualizations

**Branch**: `feat/036-k3h4-transformer-viz` | **Date**: 2026-06-14 | **Spec**: `.specify/specs/036-k3h4-transformer-viz/spec.md`

**Input**: Feature specification from `.specify/specs/036-k3h4-transformer-viz/spec.md`

## Summary

Make the k3h4 vs transformer scaling relationship measurable and legible: add a native CTest scaling benchmark (k3h4 power-mode vs synthetic transformer attention cost at five input sizes), a native training artifact export hook (cluster geometry + Voronoi scores + spectral proxy in a versioned little-endian binary), four new API endpoints surfacing benchmark and training data, and four React visualization components in the notebook domain (scaling curve chart, hypersphere geometry overlay, training confidence dashboard, mode-differentiation view). All clustering math stays native; React is a read/render consumer only.

## Technical Context

**Language/Version**: C17 (native runtime), TypeScript 5.x + Bun runtime (API + React)

**Primary Dependencies**:
- Native: CMake toolchain under `src/native`, existing k3h4 runtime modules (`engine/runtime/netcode/k3h4/`), ABI bridge (`engine/runtime/abi/netcode/`), CTest framework
- API: Fastify routes/services in `src/typescript/api/src/routes/netcode.ts` and `src/typescript/api/src/services/`
- React: hooks/components under `src/typescript/react/src/domain/notebook/`, Vite build, existing chart library

**Storage**: File-backed artifact store under `artifacts/native/` (benchmark JSON + training artifact binary files per session/epoch). No PostgreSQL persistence in this feature.

**Testing**:
- Native: CTest in `out/v3-native` — new `banana_k3h4_scaling_benchmark` and `banana_k3h4_export_hook_*` suites
- API: Bun test — `netcode.contract.test.ts` extended, new `k3h4TrainingService.test.ts`, new `k3h4-viz-determinism.contract.test.ts`
- React: Bun test — new component tests under `src/domain/notebook/viz/`

**Target Platform**:
- Native compute runtime: Windows (MSVC) + Linux (GCC/Clang) via CMake
- API: Bun/Fastify (local + container overworld compose)
- Consumer: React web shell on Banana runtime channels

**Project Type**: Multi-project monorepo feature spanning native library + web API + web client consumer

**Performance Goals**:
- Scaling curve chart renders within 3 seconds of notebook open (NFR-001)
- Geometry overlay updates within 2 seconds on epoch switch (NFR-002)
- Confidence dashboard reflects new epoch within 5 seconds of artifact availability (NFR-003)
- Benchmark reproducibility: ±10% across re-runs on same hardware (FR-008)

**Constraints**:
- All clustering math, Voronoi scoring, and artifact serialization in native/API only (FR-018)
- ABI payloads must be endianness-safe and versioned (FR-002, NFR-004)
- Output ordering deterministic across clusters/metrics (FR-013)
- Failure states explicit and non-partial (FR-003, FR-010)

**Scale/Scope**:
- Five benchmark input sizes: n ∈ {64, 256, 1024, 4096, 16384}
- Cluster count bounded (k ≤ 16, d ≤ 16) — existing runtime constants
- Touched domains: `src/native`, `src/typescript/api`, `src/typescript/react`

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

### Pre-Phase 0 Gate

- [x] Player trust and disclosure alignment verified.
  - Outcome: Internal research/training visualization only. No new storefront claims or player-facing disclosure copy required.
- [x] Storefront governance artifacts identified when public Steam copy is affected.
  - Outcome: Not applicable; this feature produces researcher tooling only and does not alter public Steam copy.
- [x] Cross-domain contracts mapped for touched layers.
  - Outcome: Mapped native export hook → versioned ABI binary → artifact files → API training service → API routes → React presentation components. Explicit contracts defined in `contracts/native-k3h4-export-abi.md` and `contracts/api-k3h4-viz-endpoints.md`.
- [x] Quality gates defined with measurable checks.
  - Outcome: Determinism (FR-002, FR-013, SC-005), slope validation (SC-002), partial-write detection (FR-003), version rejection (NFR-004), and presentation-only consumer checks (FR-018) are all scoped.
- [x] Reproducible delivery path identified.
  - Outcome: CMake + Bun + existing overworld compose path documented in `quickstart.md`.

### Post-Phase 1 Re-check

- [x] Research/design artifacts explicitly preserve controller → service → pipeline → native interop flow.
- [x] Versioned ABI + API contracts capture deterministic errors, ordering guarantees, and partial-artifact detection.
- [x] Test strategy covers native (CTest benchmark + export hook), API orchestration (contract + determinism tests), and React presentation-only requirements.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/036-k3h4-transformer-viz/
├── plan.md                           # This file
├── research.md                       # Phase 0 — research findings
├── data-model.md                     # Phase 1 — entity model
├── quickstart.md                     # Phase 1 — dev + validation entry points
├── contracts/
│   ├── native-k3h4-export-abi.md    # Phase 1 — native binary contract
│   └── api-k3h4-viz-endpoints.md    # Phase 1 — API endpoint contracts
└── tasks.md                          # Phase 2 — /speckit.tasks output (not yet created)
```

### Source Code (repository root)

```text
src/native/
├── engine/runtime/netcode/k3h4/
│   ├── netcode_k3h4_export.h         # NEW: export hook header
│   ├── netcode_k3h4_export.c         # NEW: export hook implementation
│   └── netcode_k3h4_benchmark.c      # NEW: scaling benchmark implementation
├── engine/runtime/abi/netcode/
│   ├── netcode_abi.h                 # EXTEND: training artifact record type
│   └── netcode_abi.c                 # EXTEND: artifact encode/decode helpers
└── (CMakeLists.txt at engine/runtime — EXTEND: add benchmark + export targets)

tests/native/
├── CMakeLists.txt                    # EXTEND: register new CTest targets
└── k3h4/
    ├── test_k3h4_export_hook.c       # NEW: export hook unit tests
    └── test_k3h4_scaling_benchmark.c # NEW: benchmark determinism + slope tests

artifacts/native/
├── k3h4-scaling-benchmark.json       # Generated by CTest benchmark (gitignored)
└── k3h4-training/                    # Generated training artifacts (gitignored)
    └── <session_id>/
        └── epoch-<N>.bin

src/typescript/api/src/
├── routes/
│   └── netcode.ts                    # EXTEND: register 4 new k3h4 viz routes
├── services/
│   ├── nativeNetcode.ts              # UNCHANGED
│   └── k3h4TrainingService.ts        # NEW: training artifact I/O + session mgmt
└── tests/
    ├── netcode.contract.test.ts      # EXTEND: add scaling + viz endpoint cases
    ├── k3h4TrainingService.test.ts   # NEW: training service unit tests
    └── k3h4-viz-determinism.contract.test.ts  # NEW: determinism contract tests

src/typescript/react/src/domain/notebook/
├── viz/
│   ├── ScalingCurveChart.tsx         # NEW: log-log scaling comparison chart
│   ├── HypersphereGeometryOverlay.tsx # NEW: Voronoi + inscribed radius SVG
│   ├── TrainingConfidenceDashboard.tsx # NEW: per-epoch confidence time-series
│   ├── ModeDifferentiationView.tsx   # NEW: multiplicative vs power side-by-side
│   ├── ScalingCurveChart.test.tsx    # NEW: component tests
│   ├── HypersphereGeometryOverlay.test.tsx
│   ├── TrainingConfidenceDashboard.test.tsx
│   └── ModeDifferentiationView.test.tsx
├── k3h4/
│   └── k3h4PresentationDomain.ts    # EXTEND: add viz-specific presentation types
└── useNetcodeSession.ts              # UNCHANGED (or minimal extension for session create)
```

**Structure Decision**: Banana monorepo multi-domain pattern (native / API / React). No new top-level projects introduced. Compute ownership centralized in native/API; React is read/render only.

## Phase Breakdown

### Phase 0: Research (Completed)

Output: `.specify/specs/036-k3h4-transformer-viz/research.md`

All NEEDS CLARIFICATION items resolved before design began:

- **Scaling model confirmed**: Transformer $O(n^2 d)$ vs k3h4 $O(n \cdot k \cdot d \cdot i)$; five batch sizes cover three decades on log-log axis.
- **Benchmark authoritative**: Measured wall-clock via CTest is the slope source; React reads API, does not compute.
- **Artifact format**: Extends existing 035 ABI envelope with `TRAINING_ARTIFACT` record type; atomic temp-file write + trailing `0xDEADBEEF` + CRC32 for partial-write detection.
- **API pattern**: New routes in existing `netcode.ts`, new `k3h4TrainingService.ts` service; no mutation of `nativeNetcode.ts`.
- **React chart library**: Existing library with log-axis support; no new charting dependency.
- **Confidence polling**: 5-second poll interval with exponential backoff; SSE/WS deferred.

### Phase 1: Design and Contracts (Completed)

Outputs:
- `.specify/specs/036-k3h4-transformer-viz/data-model.md`
- `.specify/specs/036-k3h4-transformer-viz/contracts/native-k3h4-export-abi.md`
- `.specify/specs/036-k3h4-transformer-viz/contracts/api-k3h4-viz-endpoints.md`
- `.specify/specs/036-k3h4-transformer-viz/quickstart.md`

Design outcomes:

- **TrainingArtifact** entity: 23-byte fixed header + `cluster_count × ClusterRecord` + 8-byte trailer (integrity marker + CRC32). Deterministic cluster ordering (ascending `center[0]`). Atomic write via temp-file rename.
- **ScalingBenchmarkResult** entity: JSON with `schema_version`, `platform`, `series[]` of `{n, k3h4_ns, attention_ns}`. Written to `artifacts/native/k3h4-scaling-benchmark.json`.
- **ConfidenceTimeSeries** entity: per-session ordered epoch confidence scalars with `status` field (`pending / active / completed`). Empty session returns `[]` + `status: "pending"` (not 404).
- **Export hook C signature** defined: `banana_k3h4_export_training_artifact(session_id, epoch_index, state, out_dir)` returning `BananaK3h4ExportStatus`.
- **Four API endpoints** specified with full request/response/error contracts.
- **React component surface** specified: four presentation-only components under `src/domain/notebook/viz/`, no production client-side compute.

### Phase 2: Native Workstream

**Files touched**: `src/native/engine/runtime/netcode/k3h4/`, `src/native/engine/runtime/abi/netcode/`, `tests/native/k3h4/`, `src/native/CMakeLists.txt` (runtime subtree)

1. **Extend ABI envelope** (`netcode_abi.h` / `netcode_abi.c`):
   - Add `RUNTIME_K3H4_ARTIFACT_TYPE_TRAINING = 0x01` discriminant constant.
   - Add `BananaK3h4TrainingArtifactHeader` struct (matches binary layout in contract).
   - Add `banana_k3h4_artifact_encode()` and `banana_k3h4_artifact_validate()` helpers.
   - Integrity marker write/read helpers: `banana_k3h4_artifact_write_trailer()` / `banana_k3h4_artifact_check_trailer()`.

2. **Native export hook** (new `netcode_k3h4_export.h` / `netcode_k3h4_export.c`):
   - Implement `banana_k3h4_export_training_artifact()` per the contract signature.
   - Preflight check (reuses existing hardware preflight from 035).
   - Serialize `RuntimeNetcodeK3h4Output` → `TrainingArtifact` binary layout.
   - Atomic write: `fopen(path.tmp)` → write → `fclose` → `rename(path.tmp, path)`.
   - On any I/O failure: unlink temp file, return `BANANA_K3H4_EXPORT_WRITE_FAILED`.
   - Directory creation: `mkdir -p artifacts/native/k3h4-training/<session_id>/` before write.

3. **Scaling benchmark CTest** (new `netcode_k3h4_benchmark.c`):
   - Implements `banana_k3h4_scaling_benchmark` test registered with CTest.
   - For each $n \in \{64, 256, 1024, 4096, 16384\}$:
     - Allocate synthetic input batch of size $n$ (deterministic seeded values).
     - Measure wall-clock via `clock_gettime(CLOCK_MONOTONIC)` before/after k3h4 power-mode pipeline call.
     - Measure synthetic transformer attention cost model ($n \times n$ inner product accumulation loop as $O(n^2)$ proxy).
   - Write `artifacts/native/k3h4-scaling-benchmark.json` atomically.
   - Assert: k3h4 slope on log-log axis within ±5% of 1.0; attention slope within ±5% of 2.0 (validates SC-002 in CI).
   - Assert: re-run produces times within ±10% of prior values when baseline exists (FR-008 self-check).

4. **CMake wiring**:
   - Add `netcode_k3h4_export.c` to the existing `netcode_k3h4` CMake target.
   - Add new `banana_k3h4_scaling_benchmark` CTest target.
   - Add new `banana_k3h4_export_hook_*` CTest targets.
   - Update `tests/native/CMakeLists.txt` accordingly.

5. **Native tests** (`tests/native/k3h4/test_k3h4_export_hook.c`):
   - `test_export_creates_artifact`: happy path write + validate integrity marker + CRC.
   - `test_export_preflight_rejected`: verify no file created on preflight failure.
   - `test_export_partial_write_detection`: truncate written file, verify reader returns `artifact_incomplete`.
   - `test_export_deterministic`: same input state → byte-identical output files.
   - `test_export_version_rejection`: reader rejects unknown ABI version.
   - `test_export_session_id_validation`: rejects session_id > 15 chars.

### Phase 3: API Workstream

**Files touched**: `src/typescript/api/src/routes/netcode.ts`, `src/typescript/api/src/services/k3h4TrainingService.ts` (new), `src/typescript/api/src/tests/`

1. **New service: `k3h4TrainingService.ts`**:
   - `readScalingBenchmark(): Promise<ScalingBenchmarkResult>` — reads + parses `artifacts/native/k3h4-scaling-benchmark.json`; throws `benchmark_not_found` if missing.
   - `createTrainingSession(mode): Promise<TrainingSession>` — creates session directory, writes session metadata JSON, returns `session_id`.
   - `readConfidenceTimeSeries(sessionId, mode): Promise<ConfidenceTimeSeries>` — enumerates epoch artifact files, decodes confidence scalar from each, returns ordered series. Missing session → throws `session_not_found`. Partial artifacts → epoch silently omitted from series.
   - `readEpochGeometry(sessionId, epochIndex, mode): Promise<EpochGeometry>` — reads and decodes `epoch-<N>.bin`, applies 2D PCA projection to cluster centers, returns full geometry payload. `artifact_incomplete` → throws structured error. `version_unsupported` → throws structured error.
   - All file reads use Node.js `fs/promises` with explicit error handling; no sync I/O.

2. **Route extensions (`netcode.ts`)**:
   - Register `GET /api/netcode/k3h4/scaling-benchmark` → `handleScalingBenchmark`.
   - Register `POST /api/netcode/k3h4/training-session` → `handleCreateTrainingSession`.
   - Register `GET /api/netcode/k3h4/training-session/:id/confidence` → `handleGetConfidence`.
   - Register `GET /api/netcode/k3h4/training-session/:id/epoch/:n/geometry` → `handleGetEpochGeometry`.
   - Each handler: validates input → calls service → serializes response. No business logic in handler.
   - `mode` query param: default `"power"` if omitted; 400 if present but invalid.

3. **TypeScript types** (co-located in `k3h4TrainingService.ts`):
   - `ScalingBenchmarkResult`, `ScalingSeriesEntry`
   - `TrainingSession`, `TrainingSessionMode`
   - `ConfidenceTimeSeries`, `EpochConfidence`, `SessionStatus`
   - `EpochGeometry`, `ClusterGeometry2d`, `ProjectionMetadata`
   - `K3h4VizServiceError` (structured error with `code` and `message`)

4. **API tests**:
   - `netcode.contract.test.ts` — extend with `scaling-benchmark`, `training-session`, `confidence`, and `geometry` endpoint cases (happy path + all error paths).
   - `k3h4TrainingService.test.ts` — unit tests for each service function with file fixtures.
   - `k3h4-viz-determinism.contract.test.ts` — calls `geometry` endpoint twice with identical `(session, epoch, mode)` and asserts byte-identical JSON response bodies (SC-005).

5. **Determinism requirement**: JSON responses serialized with sorted keys and no trailing whitespace. `JSON.stringify(payload, Object.keys(payload).sort())` pattern enforced in all four handler serializations.

### Phase 4: React Workstream

**Files touched**: `src/typescript/react/src/domain/notebook/viz/` (all new), `src/typescript/react/src/domain/notebook/k3h4/k3h4PresentationDomain.ts` (extend)

All components are presentation-only. No clustering math, Voronoi scoring, or artifact serialization executes client-side in production.

1. **`ScalingCurveChart.tsx`** (FR-014, US-1):
   - Props: `series: ScalingSeriesEntry[]` (from API response)
   - Renders log-log chart with two labeled series ("Transformer Attention", "k3h4 Power-Mode")
   - Computes and annotates slope of each series from the measured data (log-linear regression on log(n) vs log(cost))
   - Asserts at least five data points per series; renders loading/error state if fewer
   - Responsive: axes labeled, no overlap on resize
   - Uses existing chart library with log-scale axis configuration

2. **`HypersphereGeometryOverlay.tsx`** (FR-015, US-2):
   - Props: `geometry: EpochGeometry`, `onClusterSelect?: (clusterId: number) => void`
   - Renders SVG: Voronoi cell boundaries (power-mode: distance² − radius² bisector; multiplicative-mode: non-convex variant — mode read from `geometry.mode`), inscribed radius circles, projected token positions
   - `projectionMetadata` (PCA components + explained variance) displayed as annotation below overlay
   - Selected cluster highlights contained tokens + shows Voronoi score tooltip
   - Epoch switch: component receives new `geometry` prop; no full page reload required

3. **`TrainingConfidenceDashboard.tsx`** (FR-016, US-3):
   - Props: `sessionId: string`, `mode?: TrainingSessionMode`
   - Polls `GET /api/netcode/k3h4/training-session/:id/confidence` at 5-second intervals
   - Renders time-series line chart; annotates peak epoch; shows rolling 3-epoch trend line
   - Empty session: renders waiting state with session ID and "first epoch pending" message
   - Unknown/expired session: renders error state with structured error message (no crash)
   - Poll stops when `status === "completed"`; exponential backoff on consecutive empty-epoch responses

4. **`ModeDifferentiationView.tsx`** (FR-017, US-4):
   - Props: `multiplicativeScores: TokenScore[]`, `powerScores: TokenScore[]`
   - Side-by-side bar/point charts, same x-axis scale, same token order
   - Tokens with different cluster assignment between modes visually distinguished (distinct color) in both panels simultaneously
   - Cross-panel token selection: selecting a token in one panel highlights it in both
   - Degenerate case (identical distributions): annotates "degenerate result — mode separation not observed"

5. **Presentation-only gate tests**:
   - Each component test asserts that no k3h4 math functions are invoked during render
   - FR-018 production build gate: Vite build with `NODE_ENV=production` must produce no warnings about client-side clustering imports

6. **`k3h4PresentationDomain.ts` extension**:
   - Add `ScalingSeriesEntry`, `EpochGeometry`, `ClusterGeometry2d`, `ProjectionMetadata`, `ConfidenceTimeSeries`, `TokenScore` presentation types (mirroring API response shapes)
   - No compute logic; types only

### Phase 5: Integration and Validation

1. **Cross-layer determinism drill**:
   - Run CTest benchmark → verify `k3h4-scaling-benchmark.json` written.
   - Call `GET /api/netcode/k3h4/scaling-benchmark` twice → diff response bodies → must be byte-identical.
   - Write canned training artifact fixture → call `geometry` endpoint twice → byte-identical.
   - Covered by `k3h4-viz-determinism.contract.test.ts`.

2. **Slope validation (SC-002)**:
   - CTest `banana_k3h4_scaling_benchmark` self-asserts slope bounds at runtime.
   - API `scaling-benchmark` contract test loads the artifact and re-validates slope ±5%.

3. **Confidence dashboard end-to-end (SC-004)**:
   - Integration test: write two canned epoch artifacts to `artifacts/native/k3h4-training/test-session/`, start polling mock, assert confidence time-series reflects both epochs within 5-second timeout.

4. **Partial-artifact rejection**:
   - Native test: truncate artifact → reader returns `artifact_incomplete`.
   - API test: serve truncated artifact fixture → endpoint returns 422 with `artifact_incomplete`.

5. **Mode separation (US-4 acceptance scenario 4)**:
   - API contract test: call geometry endpoint with same input under both modes → multiplicative and power scores are numerically distinct for at least one token per cluster.

6. **ABI version rejection**:
   - Native test: artifact with unknown version byte → reader returns `version_unsupported`.
   - API test: artifact with unknown version → endpoint returns 422 with `version_unsupported`.

## Storefront Deliverables

Not required for this feature. No public Steam/storefront copy or asset changes are in scope. This is a researcher-facing training signal and interpretability tool.

## Complexity Tracking

No constitution violations or justified exceptions recorded. The feature spans three layers (native / API / React) but follows the existing Banana monorepo multi-domain pattern established by 035.

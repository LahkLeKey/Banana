# Feature Specification: K3H4 Transformer-Comparative Visualizations

**Feature Branch**: `feat/036-k3h4-transformer-viz`

**Created**: 2026-06-14

**Status**: Draft

## Motivation

Standard transformer self-attention scales as $O(n^2 d)$ per layer — sequence length $n$ dominates compute and memory cost as batches grow. k3h4 power-mode clustering scales as $O(n \cdot k \cdot d \cdot i)$, where cluster count $k$ and iteration bound $i$ are both fixed constants in the Banana runtime, yielding linear scaling in $n$. Approximate spectral embedding (Nyström / randomized SVD) reduces the naive $O(n^2)$ eigendecomposition to $O(n \cdot r)$ where $r \ll n$.

This scaling gap is the core training-confidence argument for k3h4. Today it is invisible: there is no measurement, no rendered comparison, and no per-epoch signal confirming that confidence is moving in the right direction. This feature makes the scaling relationship legible — as a training signal and as an interpretability layer — so the ML confidence metric improves with each training cycle and researchers can justify the k3h4 path in production training loops with evidence rather than theory.

## User Scenarios & Testing *(mandatory)*

### User Story 1 — Scaling Curve Chart (Priority: P1)

A researcher opens the notebook domain in the React shell and sees a log-log chart comparing measured compute cost against input size $n$ for two paths: the transformer self-attention path and the k3h4 power-mode clustering path. The chart plots at least five batch sizes and shows the measured slope of each curve, confirming the expected $O(n^2)$ vs $O(n \cdot k)$ relationship from real benchmarks rather than estimated values.

**Why this priority**: This is the foundational justification artifact. A researcher who cannot see measured scaling evidence cannot make a credible production argument for k3h4. All other visualizations build on this baseline. It is independently deployable as an API endpoint + React chart with no dependency on training loop hooks.

**Independent Test**: Can be fully tested by calling the scaling benchmark API endpoint with a range of input sizes and verifying the chart renders the measured cost series correctly. Delivers standalone value as a justified-cost comparison report.

**Acceptance Scenarios**:

1. **Given** the notebook is open with a valid API session, **When** the researcher navigates to the scaling curve view, **Then** a log-log chart renders within 3 seconds showing at least five distinct input sizes ($n \in \{64, 256, 1024, 4096, 16384\}$) with two labeled series: "Transformer Attention" and "k3h4 Power-Mode".
2. **Given** the chart is rendered, **When** the researcher inspects the slope annotations, **Then** the transformer series slope is within ±5% of 2.0 on the log-log axis and the k3h4 series slope is within ±5% of 1.0, derived from native CTest benchmark outputs rather than synthetic data.
3. **Given** the benchmark data has been produced by a native CTest run, **When** the API returns the scaling series, **Then** the payload is deterministic: identical CTest inputs produce byte-identical JSON series responses.
4. **Given** the chart is rendered with default settings, **When** the researcher resizes the browser window, **Then** the chart remains legible and axis labels do not overlap.

---

### User Story 2 — Hypersphere Geometry Overlay (Priority: P2)

A researcher, after completing a training epoch, views a 2D projection of Voronoi cell boundaries with inscribed radius circles overlaid against the transformer's final-layer token embedding positions (projected to the same 2D space). The overlay makes it visually apparent where k3h4 cluster assignment and transformer attention weight concentration agree and where they diverge.

**Why this priority**: This is the interpretability artifact. It answers the question "is k3h4 learning the same structure as the transformer?" and identifies per-epoch alignment drift. It depends on the k3h4 native state export (training artifact) introduced by this feature, so it follows P1 in delivery order.

**Independent Test**: Can be fully tested by loading a static k3h4 export artifact and a static transformer embedding snapshot via the API and verifying the overlay renders the correct number of Voronoi cells, radius circles, and projected token positions. The export/import API path is testable without a live training loop.

**Acceptance Scenarios**:

1. **Given** a training artifact exists for a completed epoch, **When** the researcher requests the geometry overlay view for that epoch, **Then** the visualization renders one Voronoi cell per cluster, each with an inscribed radius circle sized to the cluster's reported radius.
2. **Given** the overlay is rendered, **When** the researcher selects a specific cluster, **Then** the view highlights the transformer token positions that fell within that cluster's cell boundary and displays the weighted Voronoi score for each selected token.
3. **Given** two consecutive epochs have artifacts, **When** the researcher toggles between them, **Then** the overlay updates within 2 seconds without a full page reload and visually marks any cluster whose centroid shifted by more than 5% of the bounding box diagonal.
4. **Given** the power mode is active, **When** the overlay renders, **Then** cell boundaries are drawn as convex weighted cells consistent with the $\text{distance}^2 - \text{radius}^2$ bisector, not the multiplicative-mode non-convex variant.

---

### User Story 3 — Training Confidence Dashboard (Priority: P3)

A researcher starts a training workflow and watches a per-epoch ML confidence metric update in near-real-time in the notebook domain. The confidence metric is derived from weighted Voronoi scores accumulated by the k3h4 native runtime and surfaced through the API confidence time-series endpoint. The researcher can see at a glance whether confidence is trending upward and can stop the workflow early if it plateaus.

**Why this priority**: This is the live training feedback artifact. It closes the training loop: researchers need a signal that training is working, not just a post-hoc comparison. It depends on both the native export hook and the API confidence endpoint, making it the highest-integration-complexity story. P3 reflects delivery order, not value — a stalled training run without this signal is a research productivity loss.

**Independent Test**: Can be tested by seeding a confidence time-series in the API with a canned epoch sequence and verifying the dashboard renders the correct trend curve and plateau detection annotation. The polling/subscription consumer is independently testable against a mock endpoint.

**Acceptance Scenarios**:

1. **Given** a training session has been started with a known session identifier, **When** the researcher opens the confidence dashboard for that session, **Then** the chart renders the confidence time-series from the first available epoch and updates within 5 seconds of each new epoch completing.
2. **Given** the confidence time-series has at least 5 epochs, **When** the researcher views the dashboard, **Then** the chart annotates the epoch at which the highest confidence value was achieved and shows whether the current epoch is above or below the rolling 3-epoch average.
3. **Given** the training workflow has produced 0 epochs yet, **When** the researcher opens the dashboard, **Then** a waiting state is shown rather than an empty chart, with a message indicating the session identifier and that the first epoch is pending.
4. **Given** the training session does not exist or has expired, **When** the researcher requests the confidence time-series, **Then** the API returns a structured error with a human-readable reason, and the dashboard renders the error state without crashing.

---

### User Story 4 — Mode-Differentiation View (Priority: P2)

A researcher can view a side-by-side comparison of multiplicative scoring ($\text{distance} / \text{radius}$) versus power scoring ($\text{distance}^2 - \text{radius}^2$) applied to the same input batch. The comparison shows per-token score distributions for each mode and highlights tokens whose cluster assignment differs between modes, giving the researcher enough information to choose the better mode for their domain.

**Why this priority**: The reorchestration contract from 035 demands explicit dual-mode support. Without a visual comparison, mode selection is a blind configuration choice. P2 shares priority with the geometry overlay because both are interpretability artifacts with overlapping API surface (same training artifact endpoint, same mode field).

**Independent Test**: Can be tested end-to-end with a static input fixture: call the API with the same input body twice (once per mode), verify both score distributions are returned deterministically, and verify the React view renders both distributions side by side with a diff annotation for assignment-divergent tokens.

**Acceptance Scenarios**:

1. **Given** an input batch is available, **When** the researcher opens the mode-differentiation view, **Then** both the multiplicative and power score distributions are displayed side by side with the same x-axis scale and the same token order.
2. **Given** the two distributions are rendered, **When** any token has a different cluster assignment under the two modes, **Then** that token's bar or point is visually distinguished (e.g., colored differently) in both panels simultaneously.
3. **Given** the researcher selects a specific token in one panel, **When** the selection is made, **Then** the corresponding token is highlighted in the other panel, showing its score under each mode.
4. **Given** the API returns the same input processed under both modes, **When** the scores are compared, **Then** the multiplicative scores and power scores are numerically distinct for at least one token per cluster — confirming mode separation is not degenerate.

---

### Edge Cases

- What happens when the input batch size exceeds the maximum the native CTest benchmark was calibrated against? The API must return the measured series up to the last calibrated size and annotate that extrapolated values are estimates only.
- What happens when a training session artifact is partially written (native export interrupted mid-write)? The API must reject partial artifacts with an explicit `artifact_incomplete` error rather than silently returning incomplete geometry data.
- What happens when the transformer embedding dimensionality is higher than the k3h4 feature dimension? The projection to 2D must be declared explicitly in the API response metadata (e.g., PCA component count, explained variance) so the researcher knows the dimensionality reduction method applied.
- What happens when spectral embedding is disabled but the geometry overlay is requested? The overlay must render using Euclidean Voronoi cells only and must annotate that spectral boundaries are not active in the returned payload.
- What happens when multiplicative and power modes produce identical score distributions? The mode-differentiation view must annotate this as a degenerate result rather than silently rendering two identical charts.

## Requirements *(mandatory)*

### Functional Requirements

#### Native Export Hook

- **FR-001**: The native k3h4 runtime MUST expose a serialization hook that writes the current k3h4 state — cluster centers, inscribed radii, weighted Voronoi scores (both multiplicative and power mode), and spectral proxy values — into a training artifact file compatible with the existing `artifacts/` directory structure.
- **FR-002**: The native export hook MUST produce deterministic output: identical k3h4 state inputs MUST produce byte-identical artifact files across platforms (enforced via little-endian encoding and the existing ABI versioning contract).
- **FR-003**: The native export hook MUST write the artifact atomically: a partially-written artifact file (interrupted mid-write) MUST NOT be indistinguishable from a complete one. The format MUST include a trailing integrity marker so readers can detect truncation.
- **FR-004**: The native export hook MUST include the active assignment mode (`multiplicative` or `power`) and spectral-layer activation flag in the artifact header, so consumers know which configuration produced the artifact without out-of-band metadata.
- **FR-005**: Hardware/decode preflight (byte order, dtype, alignment) MUST be declared and validated before the export hook runs; an export attempted before preflight MUST return an explicit error and produce no artifact file.

#### Scaling Benchmark (Native CTest)

- **FR-006**: The native CTest suite MUST include a scaling benchmark test that measures wall-clock compute cost for k3h4 power-mode clustering across at least five input sizes ($n \in \{64, 256, 1024, 4096, 16384\}$) and emits a machine-readable result file to `artifacts/native/` so the API can serve the measured series without embedding synthetic constants.
- **FR-007**: The benchmark MUST also measure a reference transformer self-attention cost (or a parameterized $O(n^2)$ proxy) at the same input sizes so the scaling curve chart reflects a measured ratio rather than a theoretical one.
- **FR-008**: Benchmark results MUST be reproducible: re-running the CTest benchmark on the same hardware with the same inputs MUST produce results within ±10% of the prior run for the same input size.

#### API Endpoints

- **FR-009**: The API MUST expose a scaling benchmark endpoint that reads the latest benchmark artifact and returns the cost series for both paths (transformer reference and k3h4) in a structured, versioned response payload.
- **FR-010**: The API MUST expose a training artifact endpoint that accepts a session identifier and epoch number and returns the k3h4 geometry state (centers, radii, scores, spectral proxy, active mode, spectral activation flag) for that epoch. Missing or partial artifacts MUST return a structured error with `artifact_incomplete` or `artifact_not_found` codes.
- **FR-011**: The API MUST expose a confidence time-series endpoint that accepts a session identifier and returns the per-epoch ML confidence metric derived from weighted Voronoi scores for all epochs completed in that session. An empty session MUST return an empty array with a `pending` status field, not a 404.
- **FR-012**: The API MUST accept explicit mode selection (`multiplicative` or `power`) as a query parameter on the training artifact and confidence endpoints; omitting mode MUST default to `power` and document the default in the response metadata.
- **FR-013**: All API responses that carry k3h4 geometry or benchmark data MUST be deterministic: identical inputs to the same session/epoch/mode MUST produce byte-identical JSON response bodies.

#### React Visualization Consumer

- **FR-014**: The React notebook domain MUST render the scaling curve chart as a log-log plot with both series labeled, at least five data points per series, and slope annotations derived from the measured data. React MUST NOT compute scaling values itself; all data MUST originate from the API.
- **FR-015**: The React notebook domain MUST render the hypersphere geometry overlay showing Voronoi cell boundaries, inscribed radius circles, and transformer token positions (2D-projected) for a given epoch's training artifact. The projection method used MUST be displayed as metadata.
- **FR-016**: The React notebook domain MUST render the training confidence dashboard showing the per-epoch confidence time-series, annotating the peak epoch and the rolling 3-epoch trend. The dashboard MUST poll or subscribe to the confidence endpoint and update within 5 seconds of a new epoch becoming available.
- **FR-017**: The React notebook domain MUST render the mode-differentiation view showing the multiplicative and power score distributions side by side for the same input batch, with tokens that change cluster assignment between modes visually distinguished.
- **FR-018**: React MUST remain presentation-only: no clustering math, Voronoi scoring, or artifact serialization may execute client-side in production. Debug-mode preview computations, if any, MUST be explicitly gated and excluded from production builds.

#### Non-Functional Requirements

- **NFR-001**: The scaling curve chart MUST render within 3 seconds of the researcher opening the notebook view under normal API response conditions.
- **NFR-002**: The geometry overlay MUST update within 2 seconds when the researcher switches between consecutive epochs.
- **NFR-003**: The confidence dashboard MUST reflect new epoch data within 5 seconds of the epoch artifact becoming available on the API.
- **NFR-004**: The ABI envelope version MUST be incremented if the export artifact format changes, and the API MUST reject artifacts with unsupported version bytes with an explicit `version_unsupported` error.
- **NFR-005**: Asymptotic scaling claims in documentation and UI annotations MUST be backed by the measured CTest benchmark results, not theoretical constants alone.

### Key Entities

- **Training Artifact**: An immutable, versioned, little-endian binary file written by the native export hook for a specific session + epoch. Contains: ABI version, active mode, spectral activation flag, cluster centers, inscribed radii, weighted Voronoi scores (both modes), spectral proxy values, and an integrity marker.
- **Scaling Benchmark Result**: A machine-readable record (written to `artifacts/native/`) capturing measured wall-clock cost at each calibrated input size for both the k3h4 path and the transformer reference path. Consumed by the API scaling endpoint.
- **Confidence Time-Series**: A per-session ordered sequence of per-epoch ML confidence scalars derived from the weighted Voronoi scores in each epoch's training artifact. Owned and served by the API; consumed by the React dashboard.
- **Training Session**: An identifier that groups a sequence of training artifacts and their associated confidence scores across epochs. Created by the caller; not managed by this feature.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: A researcher can open the scaling curve chart and read the measured cost-ratio between transformer and k3h4 paths for any calibrated input size without leaving the notebook domain — achieving this in under 3 seconds from page open.
- **SC-002**: The slope of the transformer series on the log-log chart is within ±5% of 2.0 and the slope of the k3h4 series is within ±5% of 1.0, validated by native CTest benchmark outputs on the CI target platform.
- **SC-003**: The geometry overlay renders all Voronoi cells, radius circles, and projected token positions for a given epoch within 2 seconds of epoch selection — with no missing clusters and no missing token positions for the provided artifact.
- **SC-004**: The confidence dashboard updates to reflect a newly completed epoch within 5 seconds of the artifact becoming available, verified by an integration test that writes a canned artifact and measures dashboard update latency.
- **SC-005**: All API responses that carry k3h4 geometry or benchmark data are byte-identical across repeated requests with identical inputs — verified by a determinism contract test in the API test suite.
- **SC-006**: The mode-differentiation view correctly identifies and visually distinguishes all tokens with divergent cluster assignments between multiplicative and power modes, verified by a fixture test with a known-divergent input batch.
- **SC-007**: The native export hook produces byte-identical artifact files for identical k3h4 state inputs on both Windows and Linux build targets, verified by the cross-platform determinism CTest.
- **SC-008**: The confidence metric value increases monotonically across at least 5 consecutive training epochs on the reference training fixture — confirming that training is driving confidence forward, not merely fluctuating.

## Assumptions

- Researchers using this feature are operating through the Banana notebook domain in the React web shell; mobile and desktop native shell consumers are out of scope for visualization rendering.
- The transformer self-attention reference benchmark is a parameterized cost measurement (not a full transformer model inference); a production-equivalent transformer weight loading and inference pipeline is out of scope.
- 2D projection of transformer token embeddings for the geometry overlay uses PCA unless a future spec explicitly extends this; the projection method is displayed as metadata and is not user-configurable in this feature.
- Training session lifecycle management (creation, deletion, expiry) is handled by the caller; this feature only reads and writes artifacts for sessions identified by the caller-provided identifier.
- The `artifacts/` directory structure established by 035 is the stable write target for native export artifacts; no new top-level directory is introduced.
- Spectral embedding remains optional (per the 035 reorchestration contract) and is bypassed unless explicitly activated in the request; the geometry overlay renders Euclidean Voronoi cells when spectral embedding is inactive.
- The confidence metric scalar is derived from the mean weighted Voronoi score across all tokens in the epoch's artifact; a more sophisticated metric definition is deferred to a future spec.
- React polling interval for the confidence dashboard is configurable via an environment variable (defaulting to 4 seconds) to allow testing without live clock waits.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: This feature introduces no player-facing store claims, public Steam copy changes, or AI disclosure updates. All new behavior is internal to the researcher-facing notebook domain. No storefront governance artifacts are affected.
- **Cross-Domain Contracts**:
  - *Native → API*: The native export hook writes training artifacts to `artifacts/native/`; the API reads and serves them. The artifact format is versioned and little-endian per the existing ABI envelope contract. Any format change requires an ABI version increment.
  - *Native → CTest*: The scaling benchmark test writes machine-readable results to `artifacts/native/`; the API scaling endpoint consumes this file. The file format must be stable between benchmark runs and API reads.
  - *API → React*: All geometry, scaling, and confidence data flows from API to React over HTTP. React has no write path to native state and performs no production clustering math.
  - *API → Artifacts*: The confidence time-series is owned and persisted by the API layer (session-keyed); it is not written by native directly.
- **Quality Gates**:
  - *Native*: CTest must pass `k3h4_export_determinism`, `k3h4_export_integrity_marker`, `k3h4_scaling_benchmark`, and `k3h4_preflight_blocks_export` tests before any API consumer is exercised.
  - *API*: Contract tests must cover the scaling, artifact, and confidence endpoints including version-mismatch, artifact-incomplete, and empty-session error paths.
  - *React*: Consumer tests must assert that no production clustering math runs client-side (by asserting no direct calls to native FFI paths from the notebook domain components).
  - *Cross-platform*: A CI job must run the native export determinism test on both Windows and Linux and compare artifact checksums.
- **Delivery Evidence**: The following artifacts prove reproducible behavior:
  - `artifacts/native/scaling-benchmark-<timestamp>.json` — machine-readable benchmark results from the CTest suite.
  - `artifacts/native/<session-id>/<epoch>.k3h4art` — training artifact files written by the native export hook for each completed epoch.
  - CTest XML output (`out/v3-native/Testing/`) for `k3h4_export_*` and `k3h4_scaling_benchmark` tests.
  - API test run output confirming determinism contract tests pass.

## Out of Scope

- Production transformer model inference or weight loading; the benchmark uses a parameterized $O(n^2)$ cost reference only.
- Training session lifecycle management (creation, deletion, expiry policy).
- Mobile or desktop native shell rendering of any visualization.
- User-configurable 2D projection methods for the geometry overlay (PCA is the default and only method in this feature).
- Client-side production clustering math in React under any mode.
- Any changes to the public Steam store page, player-facing disclosure copy, or controller support declarations.
- Long-term artifact storage or archival policy; artifact retention follows existing `artifacts/` conventions.

# Feature Specification: Telemetry Dashboard + Production WASM Worker Assurance (260)

**Feature Branch**: `260-telemetry-dashboard-wasm-observability`
**Created**: 2026-05-05
**Status**: Proposed
**Input**: Replace the Review Spikes tab with a Telemetry dashboard that supports observability drill-down directly in the website, verifies WASM workers in production, and exposes native-level telemetry for core runtime paths.

## Problem Statement

The existing Review Spikes surface is not optimized for operational triage. We need an in-product Telemetry dashboard that lets operators inspect key observability components without leaving the website, while also proving that production WASM workers (from specs 257-259) are healthy, fast, and safely falling back when needed. We also need drill-down telemetry that reaches native execution layers so operators can trace failures and latency across frontend, managed API pipeline, wrapper interop, and native core/DAL paths.

## In Scope

- Replace the current Review Spikes tab in the web app with a Telemetry dashboard route/tab.
- Define dashboard sections and drill-down model for website observability components.
- Define production-facing WASM worker telemetry contract (init, execution, timeout, cancellation, fallback, variant usage).
- Define production validation gates and evidence expectations for WASM worker performance and reliability.
- Define degraded-mode UX and alert states when worker metrics indicate instability.
- Define native-level telemetry drill-down contracts spanning API pipeline, interop wrapper, native core, and DAL-backed native operations.

## Out of Scope

- Replacing existing backend observability infrastructure (Sentry, logs, traces) at source.
- Building new infrastructure-level monitoring systems outside the website scope.
- Redesigning unrelated DS surfaces not tied to telemetry dashboard replacement.
- Native ABI changes for telemetry-only goals.

## User Scenarios & Testing

### User Story 1 - Replace Review Spikes With Actionable Telemetry (Priority: P1)

As an operator, I can open a Telemetry dashboard in place of Review Spikes and immediately see overall health signals for runtime, API, and WASM worker behavior.

**Why this priority**: This is the direct product requirement and unlocks operational value for every release.

**Independent Test**: Navigate to the previous Review Spikes entry point and verify it now renders the Telemetry dashboard with summary cards and links to drill-down sections.

**Acceptance Scenarios**:

1. **Given** the website is running in production mode, **When** the operator opens the former Review Spikes tab location, **Then** the Telemetry dashboard appears instead.
2. **Given** telemetry data exists, **When** the dashboard loads, **Then** summary health indicators render without console/runtime errors.

### User Story 2 - Drill Into WASM Worker Behavior (Priority: P1)

As an operator, I can drill into WASM worker metrics to verify init latency, execution latency, fallback rate, timeout events, and scalar vs SIMD usage.

**Why this priority**: WASM worker reliability is a release-critical risk area from the prior spikes.

**Independent Test**: From dashboard summary, open the WASM drill-down and verify filters/time windows expose key metrics and event samples.

**Acceptance Scenarios**:

1. **Given** worker events are emitted from production traffic, **When** the operator selects the WASM section, **Then** init latency, call duration, fallback count, timeout count, and cancellation count are visible for the selected time range.
2. **Given** mixed browser capabilities, **When** the operator views variant data, **Then** SIMD/scalar distribution is visible and attributable to environment dimensions.

### User Story 3 - Verify Production Readiness Signals Before Promotion (Priority: P2)

As a release owner, I can validate a defined set of worker SLO/SLA-like thresholds directly from the website telemetry view before approving rollout/promotion.

**Why this priority**: This reduces risk of promoting a release with hidden client-side compute regressions.

**Independent Test**: Evaluate dashboard thresholds for a release window and confirm pass/fail status is explicit and auditable.

**Acceptance Scenarios**:

1. **Given** a release candidate is live on production, **When** the owner opens telemetry thresholds, **Then** pass/fail badges are shown for worker init success, fallback ratio, and timeout ratio.

### User Story 4 - Trace Incidents Down To Native Layer (Priority: P1)

As an operator, I can trace a degraded result from website signal through API/interop into native layer telemetry so I can identify whether regressions originate in frontend execution, wrapper bindings, native core logic, or native DAL/database paths.

**Why this priority**: Native-level blind spots make production incidents expensive to triage and block confident rollback/promotion decisions.

**Independent Test**: Trigger a known degraded path and verify the dashboard can drill from high-level symptom to native-layer telemetry attributes and status distribution.

**Acceptance Scenarios**:

1. **Given** a production request shows elevated latency, **When** the operator opens telemetry drill-down, **Then** native-wrapper and native-core timing/status breakdowns are visible.
2. **Given** DAL/native database flows are exercised, **When** an error spike occurs, **Then** operators can see error-code and lane distribution attributable to native-layer paths.

### Edge Cases

- Dashboard receives partial telemetry (some components missing) and must still render available sections with clear "data unavailable" states.
- Worker telemetry ingestion lags behind real-time traffic and must display freshness timestamp/staleness warning.
- Burst traffic causes transient timeout spikes; dashboard must separate sustained incidents from short-lived spikes.
- Browser environments without SIMD must not be treated as errors when scalar fallback remains healthy.
- Native telemetry payloads may be partially unavailable for older runtime revisions; dashboard must mark missing native fields explicitly without failing the entire view.

## Requirements

### Functional Requirements

- **FR-001**: The website MUST replace the Review Spikes tab/entry with a Telemetry dashboard surface.
- **FR-002**: The Telemetry dashboard MUST provide drill-down navigation for core observability components (at minimum: runtime health, API health, frontend error signals, and WASM worker health).
- **FR-003**: The WASM worker section MUST expose production metrics for init success rate, init latency, call duration, timeout count/rate, cancellation count/rate, fallback count/rate, and error-code distribution.
- **FR-004**: The WASM worker section MUST support segmentation by time window and channel/environment dimensions needed for production triage.
- **FR-005**: The dashboard MUST display scalar vs SIMD execution mix and allow operators to distinguish expected scalar usage from worker failures.
- **FR-006**: The dashboard MUST define thresholded readiness indicators for WASM worker health and clearly flag pass/fail status.
- **FR-007**: The feature MUST preserve safe degraded behavior by showing fallback status and reason categories without requiring backend log access.
- **FR-008**: The feature MUST define validation evidence required to claim WASM workers are performing as expected in production.
- **FR-009**: The dashboard MUST provide native-level drill-down metrics and status distributions for core runtime paths (wrapper interop, native core, and native DAL where applicable).
- **FR-010**: The telemetry model MUST support cross-layer correlation so a single operator investigation can connect website events to API/interop/native traces.

### Key Entities

- **TelemetryDashboardPanel**: A top-level observability panel (runtime, API, frontend, WASM worker) with summary health state and drill-down target.
- **WasmWorkerHealthSnapshot**: Aggregated metrics for a selected interval (init success, p95 init latency, p95 execution latency, timeout/fallback/error rates, variant mix).
- **WasmWorkerEventSample**: Individual telemetry event sample with timestamp, operation, duration, status/reason code, variant, and channel metadata.
- **ReadinessThresholdResult**: Computed pass/fail evaluation against configured production thresholds for release decisions.
- **NativeHealthSnapshot**: Aggregated native-layer metrics for selected interval including latency percentiles, status/error distribution, and lane/channel attribution.
- **CrossLayerTraceReference**: Correlation identifiers and linkage metadata that connect frontend telemetry events to API pipeline and native-layer execution records.

## Success Criteria

### Measurable Outcomes

- **SC-001**: 100% of user paths that previously opened Review Spikes now route to the Telemetry dashboard without dead links.
- **SC-002**: Operators can reach WASM worker drill-down in at most 2 interactions from the dashboard landing state.
- **SC-003**: Dashboard presents production WASM worker health metrics for the selected window with data freshness under 5 minutes (or explicit stale warning).
- **SC-004**: Release owner can determine pass/fail for defined worker readiness thresholds in under 3 minutes without external tooling.
- **SC-005**: At least 90% of production WASM-relevant incident investigations can be traced from dashboard symptom to native-layer status/latency evidence without leaving the website.

## Assumptions

- Existing frontend telemetry events from the worker integration spike can be extended and mapped into dashboard views.
- Website deployment remains Vercel-hosted with API fallback/telemetry backends reachable through existing production contracts.
- Production validation uses real production traffic windows, not synthetic-only evidence.
- Mobile/native app telemetry surfaces are out of scope unless routed through the website dashboard contract.
- Native observability rollout may ship in phased depth by lane, but banana/not-banana/ripeness critical paths are included in initial release scope.

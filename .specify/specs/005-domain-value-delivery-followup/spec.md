# Feature Specification: Domain Value Delivery Follow-Up (Phase 2)

**Feature Branch**: `005-domain-value-delivery-followup`
**Created**: 2026-04-26
**Status**: Draft
**Input**: Follow-up to `.specify/specs/004-domain-value-delivery` to close remaining delivery gaps and harden production contracts.

## In Scope *(mandatory)*

- Harden native-unavailable error semantics so `BANANA_NATIVE_PATH` load failures return a dedicated API error contract (503 with native-runtime remediation), not a database-oriented fallback message.
- Move controller-level JSON interpretation into a dedicated pipeline/service boundary for ML and Not-Banana responses, avoiding ad-hoc deserialization in controllers.
- Add the missing TypeScript API ripeness route (`src/typescript/api/src/routes/ripeness.ts`) and wire API-level contract tests for `/ripeness/predict`.
- Add ASP.NET HTTP surfaces for Harvest/Truck operations already exposed through `INativeBananaClient`.
- Add a repository-standard integration test surface for API contract checks (or formally codify `tests/e2e` as the integration-equivalent lane and update docs/tasks accordingly).
- Ensure all new contracts are reflected in spec artifacts and tests so subsequent SDLC orchestration can validate without manual interpretation.

## Out of Scope *(mandatory)*

- New CI workflow families or orchestration redesign.
- New compose profiles or runtime channel additions.
- Prisma `banana_profile` schema work and DAL persistence expansion.
- Not-banana model retraining workflows.
- Mobile/Electron feature additions beyond contract compatibility.

## User Scenarios & Testing *(mandatory)*

### User Story 1 — Native Runtime Failure Contract Is Accurate (Priority: P1)

As an API consumer, when native runtime loading fails, I receive a clear 503 error with native-runtime remediation guidance.

**Why this priority**: Current fallback semantics are misleading (`BANANA_PG_CONNECTION` remediation for native load failures), which slows triage and breaks audit quality.

**Independent Test**: Start API with invalid `BANANA_NATIVE_PATH`; call `/ripeness/predict`; assert HTTP 503 plus dedicated native-runtime error payload.

**Acceptance Scenarios**:

1. **Given** `BANANA_NATIVE_PATH` points to a missing library, **When** `/ripeness/predict` is called, **Then** response is 503 with `error = "native_unavailable"` and remediation referencing `BANANA_NATIVE_PATH`.
2. **Given** native runtime is available, **When** `/ripeness/predict` succeeds, **Then** no fallback runtime-error payload is emitted.

---

### User Story 2 — API Contract Ownership Is Centralized (Priority: P2)

As a maintainer, I can evolve ML/Not-Banana response contracts through one pipeline/service layer instead of per-controller ad-hoc JSON handling.

**Why this priority**: Controller-level deserialization introduces drift and reduces cross-endpoint consistency.

**Independent Test**: Unit tests assert controller actions delegate payload shaping to pipeline/service abstractions; controller assertions validate HTTP semantics only.

**Acceptance Scenarios**:

1. **Given** a binary ML prediction call, **When** response mapping runs, **Then** mapping is executed by pipeline/service code and returned as a typed payload.
2. **Given** Not-Banana prediction, **When** mapping runs, **Then** controller has no ad-hoc JSON field extraction logic.

---

### User Story 3 — TypeScript API Exposes Ripeness Endpoint (Priority: P2)

As a client of the TS API layer, I can call `/ripeness/predict` from the Fastify surface with a typed contract.

**Why this priority**: 004 shipped React->ASP.NET directly; TS API still lacks parity for ripeness.

**Independent Test**: Route test in `src/typescript/api/src/routes/` passes for valid/invalid payloads and matches response schema.

**Acceptance Scenarios**:

1. **Given** valid input payload, **When** `POST /ripeness/predict` is called on TS API, **Then** response includes `label` and `confidence` with valid types.
2. **Given** invalid payload, **When** route is called, **Then** TS API returns explicit 4xx validation error.

---

### User Story 4 — Harvest/Truck HTTP Surfaces Are Reachable (Priority: P3)

As an API consumer, I can invoke harvest and truck lifecycle operations already exposed in native interop through documented ASP.NET endpoints.

**Why this priority**: Current interop methods exist but are not fully surfaced over HTTP.

**Independent Test**: Unit or e2e tests cover create/status/update operations for harvest batch and truck operations with expected status mapping.

**Acceptance Scenarios**:

1. **Given** valid harvest payload, **When** create/status endpoints are called, **Then** API returns typed status payloads.
2. **Given** valid truck payloads, **When** register/load/unload/relocate/status endpoints are called, **Then** API returns expected statuses and error mapping.

---

### Edge Cases

- Native library present but missing required symbol (entrypoint mismatch).
- TS API ripeness proxy timeout or upstream 5xx propagation.
- Harvest/truck IDs not found (must map to 404 consistently).
- Invalid JSON body shape across ASP.NET and TS API routes should produce deterministic 4xx behavior.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: Native runtime load failures MUST map to a dedicated API error contract (`native_unavailable`) with remediation pointing to `BANANA_NATIVE_PATH`.
- **FR-002**: ML and Not-Banana HTTP response shaping MUST be owned by pipeline/service mapping abstractions, not ad-hoc controller JSON logic.
- **FR-003**: `src/typescript/api/src/routes/ripeness.ts` MUST expose `POST /ripeness/predict` with typed request/response contracts.
- **FR-004**: ASP.NET MUST expose missing harvest/truck HTTP endpoints for currently available `INativeBananaClient` operations.
- **FR-005**: A stable integration validation lane MUST exist and be documented for contract-level API checks (new `tests/integration` or explicit `tests/e2e` contract designation).
- **FR-006**: New/changed endpoint contracts MUST include tests that validate top-level payload shape and status-code mapping.

### Key Entities *(include if feature involves data)*

- **`NativeRuntimeError`**: API error payload for native library unavailability (`error`, `remediation`, optional `details`).
- **`BinaryClassificationResult`**: Typed ML response projection used by pipeline/service mapping.
- **`NotBananaClassificationResult`**: Typed Not-Banana response projection.
- **`RipenessRouteRequest/Response` (TS API)**: Fastify route schema for ripeness prediction.
- **`HarvestBatchResult` / `TruckResult`**: Typed HTTP response models for new endpoint surfaces.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Invalid `BANANA_NATIVE_PATH` produces 503 with `native_unavailable` payload in 100% of tested cases.
- **SC-002**: No ASP.NET controller in this slice contains ad-hoc `JsonElement` response shaping for ML/Not-Banana/ripeness payload contracts.
- **SC-003**: TS API ripeness route has passing route tests for success and validation failure paths.
- **SC-004**: Harvest/truck endpoint coverage includes at least one success and one failure-path assertion per operation family.
- **SC-005**: Integration validation command and lane are documented and runnable without manual interpretation.

## Assumptions

- The native wrapper ABI remains stable in this slice.
- Existing compose/runtime setup remains sufficient for endpoint validation.
- No breaking frontend contract changes are required beyond added API parity.
- Existing `004` functionality remains intact and is treated as regression-protected baseline.

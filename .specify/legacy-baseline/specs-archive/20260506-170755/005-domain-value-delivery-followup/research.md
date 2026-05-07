# Research: Domain Value Delivery Follow-Up (Phase 2)

**Branch**: `005-domain-value-delivery-followup` | **Phase**: 0 | **Date**: 2026-04-26

## R-01: Dedicated native runtime unavailable contract

**Question**: How should interop load failures be represented without conflating them with DB configuration failures?

**Decision**: Add a dedicated status code in managed interop mapping (`NativeUnavailable`) and map it to HTTP 503 with payload:

```json
{
  "error": "native_unavailable",
  "remediation": "Set BANANA_NATIVE_PATH to a valid native library path."
}
```

**Rationale**: Interop-load failures (DLL missing, symbol missing, bad image) are operationally distinct from `BANANA_PG_CONNECTION` problems.

**Alternatives considered**:
- Reuse `DbNotConfigured`: rejected because it provides wrong remediation guidance.
- Return 500 only: rejected because this is an availability/configuration condition with known remediation.

## R-02: Controller JSON shaping ownership

**Question**: Where should JSON payload conversion from native strings occur for ML and not-banana endpoints?

**Decision**: Move response shaping into pipeline/service mapping abstractions under `src/c-sharp/asp.net/Pipeline/Mapping/`, with controllers delegating mapping and owning only HTTP status behavior.

**Rationale**: Keeps controllers thin and enforces consistent payload shape across endpoints.

**Alternatives considered**:
- Keep `JsonElement` parsing in controllers: rejected due drift risk and duplicated logic.
- Move deserialization into `NativeBananaClient`: rejected because interop seam should remain transport-level and type-agnostic.

## R-03: TS API ripeness parity design

**Question**: How should Fastify expose `/ripeness/predict` while preserving existing ASP.NET ownership?

**Decision**: Add `src/typescript/api/src/routes/ripeness.ts` as a proxy route with schema-validated request/response and upstream error propagation; register it from `src/typescript/api/src/index.ts`.

**Rationale**: Provides typed TS API parity without moving ASP.NET ownership of native prediction.

**Alternatives considered**:
- Implement ripeness logic directly in Fastify: rejected because it would duplicate native integration responsibilities.

## R-04: Harvest/Truck HTTP surface exposure

**Question**: Which missing interop operations should be surfaced over HTTP in this slice?

**Decision**: Add dedicated controllers for harvest and truck families and surface all currently available methods in `INativeBananaClient`.

**Rationale**: Interop methods already exist; endpoint gaps are delivery-level, not domain-level.

**Alternatives considered**:
- Extend `BatchController` for all operations: rejected due endpoint sprawl and unclear ownership.

## R-05: Integration validation lane codification

**Question**: Should a new `tests/integration` lane be created now or should existing `tests/e2e` be codified as integration-equivalent?

**Decision**: Codify `tests/e2e` as the current integration-equivalent lane in this feature, add explicit contract lane tests and documentation references.

**Rationale**: Lowest-risk path with immediate reproducibility and no harness redesign.

**Alternatives considered**:
- Create new `tests/integration` project in this slice: rejected as out-of-scope foundation expansion for this feature.

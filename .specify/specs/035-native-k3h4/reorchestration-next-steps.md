# Reorchestration Next Steps: Dual-Mode K3H4

## Purpose

Capture the next implementation slice after the initial K3H4 migration so the runtime, ABI, API, and React layers align with the stronger operating model from the K3H4 refactoring report.

## Core Reorchestration Direction

### 1. Support Two K3H4 Modes Explicitly

The runtime should expose two assignment modes rather than treating K3H4 as a single scoring regime.

- `multiplicative`
  - Score: `distance / radius`
  - Use when the desired interpretation is normalized distance in units of local cluster radius.
  - Accept that resulting weighted cells may be non-convex or disconnected.

- `power`
  - Score: `distance^2 - radius^2`
  - Use when convex weighted cells, simpler bisectors, and stable geometric infrastructure are the priority.
  - Treat this as the preferred mode for infrastructure that depends on stable cell boundaries or deterministic point-location behavior.

### 2. Gate The Spectral Layer

The spectral layer should remain optional and should not run by default just because it exists.

- Bypass spectral embedding when Euclidean clustering over decoded feature vectors is sufficient.
- Enable spectral embedding only when the affinity graph carries information that plain Euclidean cells do not.
- Treat spectral activation as an explicit orchestration decision rather than an implicit runtime side effect.

### 3. Enforce Hardware Contract Declaration Before Clustering

The decode and hardware layer must be a hard precondition, not a downstream validation detail.

Required declared fields before clustering begins:

- byte order
- dtype / numeric encoding
- alignment

Operational requirement:

- If these values are absent, malformed, or unsupported, clustering must not start.

## Proposed Architecture Adjustments

### Native

- Promote assignment family from environment-driven tuning to an explicit runtime request/config field.
- Split pipeline entry into:
  1. hardware/decode preflight
  2. optional spectral embedding
  3. assignment-mode-specific clustering
  4. envelope publication
- Keep multiplicative and power scoring logic in isolated functions so tests can assert mode-specific semantics directly.

### API

- Accept explicit K3H4 orchestration mode selection as part of authoritative analytics request shaping.
- Surface selected mode and spectral-layer activation in response metadata for auditability.
- Reject requests that imply clustering before the ABI/decode contract is validated.

### Frontend

- Continue presentation-only behavior.
- Render returned mode metadata and spectral activation state, but never choose a production mode client-side.

## Validation Priorities

1. Deterministic multiplicative-mode fixtures.
2. Deterministic power-mode fixtures.
3. Mode-differentiation tests proving the same input can yield different weighted score semantics while preserving stable ordering.
4. Spectral-bypass vs spectral-enabled orchestration tests.
5. Preflight tests proving missing byte order, dtype, or alignment blocks clustering before any geometry work starts.

## Recommended Execution Order

1. Make mode selection explicit in native request/config structs and TypeScript interop types.
2. Move hardware preflight ahead of clustering entrypoints and fail fast on invalid declaration.
3. Separate multiplicative and power scoring paths behind one orchestrated interface.
4. Add spectral-layer gating at orchestration boundaries.
5. Publish mode/spectral metadata through API and React read models.
> **Superseded-by-031**: This spec is folded into [`031-unified-coherent-world`](../031-unified-coherent-world/spec.md) as of 2026-05-27. Do not extend this slice; extend 031 instead.

# Spec 024: Route Signature Cohesion

**Status**: Complete

**Input**: User description: "we have alot of slices, id like to combine them into one coherent game world"

## Summary

Tightened persistent-world continuity by enforcing canonical (trim + lowercase) `checkpointRouteSignatureTag`, proving replay determinism under route-signature-drift retries, and growing one additive continuity attribute (`checkpointAttunementLaneTag`).

## User Stories

### US1 — Canonical Route Signature Tag (P1)

Reject continuity payloads whose `checkpoint.checkpointRouteSignatureTag` is not canonical with diagnostic `persistent_world_continuity_checkpoint_route_signature_mismatch`.

### US2 — Replay Determinism Under Route Signature Drift (P1)

Drift retries must not corrupt replay lineage; canonical commit lands at `areaStateVersion: 1`.

### US3 — Additive Attunement Lane Attribute (P2)

Add additive `checkpointAttunementLaneTag` (default `'attunement:lane-default'`) included in deterministic signature.

## Functional Requirements

- FR-001: Reject non-canonical `checkpointRouteSignatureTag`.
- FR-002: Diagnostic carries reason `non_canonical_checkpoint_route_signature_format`.
- FR-003: Route-signature drift retries do not corrupt replay state.
- FR-004: Canonical commit after retry drift yields `areaStateVersion: 1`.
- FR-005: `checkpointAttunementLaneTag` participates in transition signature.
- FR-006: Growth-path integration round-trips the additive attribute.

## Implementation Outcomes

- API continuity unit + route suite: passing.
- API continuity integration suite: passing.
- Native scene-catalog coherence suites: passing.

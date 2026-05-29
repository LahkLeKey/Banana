> **Superseded-by-031**: This spec is folded into [`031-unified-coherent-world`](../031-unified-coherent-world/spec.md) as of 2026-05-27. Do not extend this slice; extend 031 instead.

# Spec 023: Checkpoint Id Cohesion

**Status**: Complete

**Input**: User description: "we have alot of slices, id like to combine them into one coherent game world"

## Summary

Tightened coherence of the persistent-world continuity surface by enforcing canonical (trim + lowercase) `checkpointId` formatting, proving replay determinism under checkpoint-id-drift retries, and growing one additive continuity attribute (`checkpointObservationLaneTag`) without destabilising owning contracts.

## User Stories

### US1 — Canonical Checkpoint Id Formatting (P1)

Reject continuity payloads whose `checkpoint.checkpointId` is not canonical (trim + lowercase) with deterministic diagnostic `persistent_world_continuity_checkpoint_id_mismatch`.

### US2 — Replay Determinism Under Checkpoint Id Drift (P1)

Repeated checkpoint-id-drift commits must not corrupt authoritative replay lineage; a subsequent canonical commit lands at `areaStateVersion: 1` and rehydrates the canonical id.

### US3 — Additive Cohesion Attribute (P2)

Add an additive `checkpointObservationLaneTag` (default `'observation:lane-default'`) to `ContinuityCheckpointStateSchema`, include it in the deterministic transition signature, and prove growth-path persistence/rehydration.

## Functional Requirements

- FR-001: `validateAndNormalize` rejects non-canonical `checkpointId` formatting.
- FR-002: Diagnostic carries reason `non_canonical_checkpoint_id_format` plus provided/expected values.
- FR-003: Checkpoint-id-drift retries do not corrupt authoritative replay state.
- FR-004: Canonical commit after retry drift yields `areaStateVersion: 1` and canonical rehydration.
- FR-005: `checkpointObservationLaneTag` participates in the deterministic transition signature.
- FR-006: Growth-path integration round-trips the additive attribute.

## Implementation Outcomes

- API continuity unit + route suite: 43 pass / 0 fail.
- API continuity integration suite: 19 pass / 0 fail.
- Native scene-catalog coherence suites: 4/4 pass.
- Owning surfaces updated: `persistentWorld.ts`, `continuityPayloadService.ts` (+ test), `world.continuity.contract.test.ts`, continuity round-trip / contract-drift / growth-path integration tests.

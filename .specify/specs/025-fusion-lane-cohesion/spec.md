> **Superseded-by-031**: This spec is folded into [`031-unified-coherent-world`](../031-unified-coherent-world/spec.md) as of 2026-05-27. Do not extend this slice; extend 031 instead.

# Spec 025: Fusion Lane Cohesion

**Status**: Complete

**Input**: User description: "we have alot of slices, id like to combine them into one coherent game world"

## Summary

Enforce canonical (trim + lowercase) `checkpointFusionLaneTag`, prove replay determinism under fusion-lane-drift retries, and add additive `checkpointEchoLaneTag`.

## User Stories

- US1 (P1): Reject non-canonical `checkpointFusionLaneTag` with diagnostic `persistent_world_continuity_checkpoint_fusion_lane_mismatch`.
- US2 (P1): Fusion-lane-drift retries do not corrupt replay lineage; canonical commit lands at `areaStateVersion: 1`.
- US3 (P2): Additive `checkpointEchoLaneTag` default `'echo:lane-default'` included in deterministic signature.

## Functional Requirements

- FR-001: `validateAndNormalize` rejects non-canonical `checkpointFusionLaneTag`.
- FR-002: Diagnostic reason `non_canonical_checkpoint_fusion_lane_format`.
- FR-003: Drift retries do not corrupt replay state.
- FR-004: Canonical commit after drift yields `areaStateVersion: 1`.
- FR-005: `checkpointEchoLaneTag` participates in transition signature.
- FR-006: Growth-path integration round-trips the additive attribute.

## Implementation Outcomes

- API continuity unit + route suite: passing.
- API continuity integration suite: passing.
- Native scene-catalog coherence suites: passing.

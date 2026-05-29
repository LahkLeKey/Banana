> **Superseded-by-031**: This spec is folded into [`031-unified-coherent-world`](../031-unified-coherent-world/spec.md) as of 2026-05-27. Do not extend this slice; extend 031 instead.

# 030 — Observation-Lane Cohesion

## US1
Reject non-canonical `checkpointObservationLaneTag` (diagnostic `persistent_world_continuity_checkpoint_observation_lane_mismatch`, reason `non_canonical_checkpoint_observation_lane_format`).

## US2
Observation-lane-drift retries do not break authoritative replay once canonical commit succeeds.

## US3
Additive `checkpointStellarLaneTag` default `'stellar:lane-default'` mutates signature deterministically.

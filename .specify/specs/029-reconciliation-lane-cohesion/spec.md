> **Superseded-by-031**: This spec is folded into [`031-unified-coherent-world`](../031-unified-coherent-world/spec.md) as of 2026-05-27. Do not extend this slice; extend 031 instead.

# 029 — Reconciliation-Lane Cohesion

## US1
Reject non-canonical `checkpointReconciliationLaneTag` with diagnostic `persistent_world_continuity_checkpoint_reconciliation_lane_mismatch` (reason `non_canonical_checkpoint_reconciliation_lane_format`).

## US2
Reconciliation-lane-drift retries do not break authoritative replay once canonical commit succeeds.

## US3
Additive `checkpointAuroraLaneTag` default `'aurora:lane-default'` mutates signature deterministically.

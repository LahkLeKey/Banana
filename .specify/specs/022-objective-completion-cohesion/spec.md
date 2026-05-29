> **Superseded-by-031**: This spec is folded into [`031-unified-coherent-world`](../031-unified-coherent-world/spec.md) as of 2026-05-27. Do not extend this slice; extend 031 instead.

# Spec 022: Objective Completion Cohesion

**Status**: Complete

**Implementation Outcomes**:

- US1: Canonical objective-completion-ids enforcement (`persistent_world_continuity_objective_completion_ids_mismatch`) wired through service, route contract, and drift integration tests; rejects ordering drift and duplicates.
- US2: Deterministic ordering-drift retry diagnostics validated with authoritative replay lineage stability in the continuity roundtrip integration suite.
- US3: Additive `checkpointReconciliationLaneTag` introduced through owning contract path with deterministic signature mutation and growth-path persistence/rehydration coverage.
- Validation: API contract suite 39/39 pass, API integration suite 17/17 pass, native focused continuity/coherence suite 4/4 pass.

**Input**: User description: "we have alot of slices, id like to combine them into one coherent game world"

## Summary

Tighten coherence of the persistent-world continuity surface by enforcing canonical ordering of `objectiveCompletionIds`, proving replay determinism under ordering-drift retries, and growing one additive continuity attribute (`checkpointReconciliationLaneTag`) without destabilising owning contracts.

## User Stories

### US1 — Canonical Objective Completion Ordering (P1)

Reject continuity payloads whose `checkpoint.objectiveCompletionIds` is not in canonical (lexicographically sorted, de-duplicated) order with a deterministic diagnostic `persistent_world_continuity_objective_completion_ids_mismatch`.

### US2 — Replay Determinism Under Ordering Drift (P1)

Repeated ordering-drift commits must not corrupt authoritative replay lineage; a subsequent canonical commit lands at `areaStateVersion: 1` and rehydrates the canonical ordering.

### US3 — Additive Cohesion Attribute (P2)

Add an additive `checkpointReconciliationLaneTag` (default `'reconciliation:lane-default'`) to `ContinuityCheckpointStateSchema`, include it in the deterministic transition signature, and prove growth-path persistence/rehydration.

## Functional Requirements

- FR-001: `validateAndNormalize` rejects non-canonical `objectiveCompletionIds` ordering.
- FR-002: Diagnostic carries reason `non_canonical_objective_completion_ids_order` plus provided/expected arrays.
- FR-003: Ordering-drift retries do not corrupt authoritative replay state.
- FR-004: Canonical commit after retry drift yields `areaStateVersion: 1` and canonical rehydration.
- FR-005: `checkpointReconciliationLaneTag` participates in the deterministic transition signature.
- FR-006: Growth-path integration round-trips the additive attribute.

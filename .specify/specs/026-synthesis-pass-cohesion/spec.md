> **Superseded-by-031**: This spec is folded into [`031-unified-coherent-world`](../031-unified-coherent-world/spec.md) as of 2026-05-27. Do not extend this slice; extend 031 instead.

# Spec 026: Synthesis Pass Cohesion

**Status**: Complete

**Input**: User description: "we have alot of slices, id like to combine them into one coherent game world"

## Summary

Enforce canonical (trim + lowercase) `checkpointSynthesisPassTag`, prove replay determinism under synthesis-pass-drift retries, and add additive `checkpointResonanceLaneTag`.

## User Stories

- US1 (P1): Reject non-canonical `checkpointSynthesisPassTag` with diagnostic `persistent_world_continuity_checkpoint_synthesis_pass_mismatch`.
- US2 (P1): Synthesis-pass-drift retries do not corrupt replay lineage; canonical commit lands at `areaStateVersion: 1`.
- US3 (P2): Additive `checkpointResonanceLaneTag` default `'resonance:lane-default'` included in deterministic signature.

## Functional Requirements

- FR-001..FR-006 mirror 025 mapped to synthesis-pass + resonance lane.

## Implementation Outcomes

- API + native suites passing.

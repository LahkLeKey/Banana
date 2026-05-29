> **Superseded-by-031**: This spec is folded into [`031-unified-coherent-world`](../031-unified-coherent-world/spec.md) as of 2026-05-27. Do not extend this slice; extend 031 instead.

# Spec 027: Replay Phase Cohesion

**Status**: Complete

Canonical `checkpointReplayPhaseTag` enforcement, replay-phase-drift retry replay determinism, additive `checkpointHorizonLaneTag`.

- US1: reject non-canonical → `persistent_world_continuity_checkpoint_replay_phase_mismatch`.
- US2: drift retries do not corrupt lineage; canonical commit at `areaStateVersion: 1`.
- US3: additive `checkpointHorizonLaneTag` default `'horizon:lane-default'` in signature.

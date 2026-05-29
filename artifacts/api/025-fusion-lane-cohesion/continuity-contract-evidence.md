# 025 Fusion Lane Cohesion — API Continuity Evidence

## US1 — Canonical Fusion Lane Tag

- Unit reject: `rejects non-canonical checkpoint fusion lane tag formatting` (passing).
- Route reject: `rejects continuity payload with non-canonical checkpoint fusion lane diagnostics` (passing).
- Drift integration: `rejects non-canonical checkpoint fusion lane with deterministic diagnostics` (passing).
- Diagnostic code: `persistent_world_continuity_checkpoint_fusion_lane_mismatch`, reason `non_canonical_checkpoint_fusion_lane_format`.

## US2 — Replay Determinism Under Fusion-Lane Drift

- Round-trip integration: `invalid fusion-lane retries do not break authoritative replay once canonical commit succeeds` (passing). Canonical commit lands at `areaStateVersion: 1`.

## US3 — Additive Echo Lane Attribute

- Unit signature: `additive checkpoint echo lane field mutates signature deterministically` (passing).
- Growth path integration confirms `checkpointEchoLaneTag` persists and rehydrates with canonical value `echo:lane-growth-v2`.

## Final Validation Summary

- `cd src/typescript/api && bun run test:continuity` → 49 pass / 0 fail.
- `cd src/typescript/api && bun run test:integration:continuity` → 23 pass / 0 fail.

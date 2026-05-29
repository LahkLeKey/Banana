# API Continuity Contract Evidence — 022 Objective Completion Cohesion

## Baseline Commands

- `cd src/typescript/api && bun run test:continuity`
- `cd src/typescript/api && bun run test:integration:continuity`

## US1 Evidence

- Added `persistent_world_continuity_objective_completion_ids_mismatch` reject in `continuityPayloadService.ts` (`normalizeObjectiveCompletionIds` = sort + dedupe).
- Added unit tests `rejects non-canonical objective completion ids ordering` and `rejects duplicate objective completion ids as non-canonical`.
- Added route contract tests `rejects continuity payload with non-canonical objective completion ids diagnostics` and `rejects repeated non-canonical objective completion ids payloads deterministically`.
- Added drift integration test `rejects non-canonical objective completion ids ordering with deterministic diagnostics`.

## US2 Evidence

- Added roundtrip integration test `invalid objective-completion-ids ordering retries do not break authoritative replay once canonical commit succeeds` verifying deterministic diagnostics and stable authoritative replay lineage after ordering-drift retries.

## US3 Evidence

- Added additive `checkpointReconciliationLaneTag` (default `reconciliation:lane-default`) to `ContinuityCheckpointStateSchema`.
- Extended `makeExpectedTransitionSignature` to include reconciliation lane tag.
- Added unit mutation test `additive checkpoint reconciliation lane field mutates signature deterministically`.
- Extended growth-path integration to round-trip the additive attribute.

## Final Validation Summary

- `cd src/typescript/api && bun run test:continuity` -> 39 pass / 0 fail.
- `cd src/typescript/api && bun run test:integration:continuity` -> 17 pass / 0 fail.

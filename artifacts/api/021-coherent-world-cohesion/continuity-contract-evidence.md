# 021 Coherent World Cohesion - API Evidence

## Baseline Command Inventory

- API continuity contract suite:
  - `cd src/typescript/api && bun run test:continuity`
- API continuity integration suite:
  - `cd src/typescript/api && bun run test:integration:continuity`
- Focused as-needed suites:
  - `cd src/typescript/api && bun test src/domains/persistent-world-orchestration/services/replayMergeService.test.ts`
  - `cd src/typescript/api && bun test src/routes/world.continuity.contract.test.ts`

## US1 Evidence

- Added `persistent_world_continuity_profile_fingerprint_mismatch` reject in `continuityPayloadService.ts`.
- Added unit test `rejects non-canonical profile fingerprint formatting` (continuityPayloadService.test.ts).
- Added route contract tests `rejects continuity payload with non-canonical profile fingerprint diagnostics` and `rejects repeated non-canonical profile fingerprint payloads deterministically`.
- Added drift integration test `rejects non-canonical profile fingerprint with deterministic diagnostics`.

## US2 Evidence

- Added roundtrip integration test `invalid profile-fingerprint retries do not break authoritative replay once canonical commit succeeds` verifying deterministic diagnostics and stable authoritative replay lineage after fingerprint-drift retries.

## US3 Evidence

- Added additive `checkpointConvergenceLaneTag` field (default `'convergence:lane-default'`) to `ContinuityCheckpointStateSchema`.
- Extended `makeExpectedTransitionSignature` to include convergence lane tag for deterministic signature mutation.
- Added unit mutation test `additive checkpoint convergence lane field mutates signature deterministically`.
- Extended growth-path integration `makePayload` and rehydration expectations to assert the additive field roundtrips.

## Final Validation Summary

- `cd src/typescript/api && bun run test:continuity` -> 34 pass / 0 fail.
- `cd src/typescript/api && bun run test:integration:continuity` -> 15 pass / 0 fail.

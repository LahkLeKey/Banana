# 020 Coherent World Convergence - API Evidence

## Baseline Command Inventory

- API continuity contract suite:
  - `cd src/typescript/api && bun run test:continuity`
- API continuity integration suite:
  - `cd src/typescript/api && bun run test:integration:continuity`
- Focused as-needed suites:
  - `cd src/typescript/api && bun test src/domains/persistent-world-orchestration/services/replayMergeService.test.ts`
  - `cd src/typescript/api && bun test src/lib/contracts/v1/contracts.test.ts`
  - `cd src/typescript/api && bun test src/routes/world.continuity.contract.test.ts`

## US1 Evidence

- Audit finding (T004): checkpoint context identity canonicalization remained permissive, allowing whitespace/case drift at commit entry.
- Implementation change (T005): added canonical checkpoint-context enforcement in `continuityPayloadService.validateAndNormalize` with diagnostic:
  - `persistent_world_continuity_checkpoint_context_mismatch`
- Test updates (T006):
  - `continuityPayloadService.test.ts`: non-canonical checkpoint context reject coverage.
  - `world.continuity.contract.test.ts`: route-level checkpoint context mismatch reject coverage.
  - `persistent-world-continuity-contract-drift.integration.test.ts`: deterministic repeated mismatch diagnostics.
- Validation commands:
  - `cd src/typescript/api && bun run test:continuity`
  - `cd src/typescript/api && bun test src/integration/persistent-world-continuity-contract-drift.integration.test.ts`
- Validation results:
  - Contract suites: `28 pass`, `0 fail`
  - Drift integration suite: `7 pass`, `0 fail`

## US2 Evidence

- Audit findings (T008): replay lineage required explicit coverage for invalid context retries interleaved with canonical commits.
- Deterministic diagnostics expansion (T009-T011): route + integration retries now assert stable mismatch diagnostics for repeated non-canonical checkpoint context payloads.
- Replay lineage coverage (T010): added integration case proving invalid retries do not corrupt authoritative lineage once canonical commit succeeds.
- Validation command (T012):
  - `cd src/typescript/api && bun run test:integration:continuity`
- Validation results:
  - Integration suites: `13 pass`, `0 fail`

## US3 Evidence

- Additive attribute introduced (T014): `checkpointReplayPhaseTag` with default `replay:phase-default` in continuity checkpoint owning schema path.
- Deterministic behavior update (T015): transition signature derivation now includes `checkpointReplayPhaseTag`.
- Mutation/persistence coverage (T016):
  - `continuityPayloadService.test.ts`: deterministic signature mutation for replay phase tag.
  - `persistent-world-continuity-growth-path.integration.test.ts`: replay phase tag persistence/rehydration coverage.
- Validation commands:
  - `cd src/typescript/api && bun run test:continuity`
  - `cd src/typescript/api && bun run test:integration:continuity`
- Validation results:
  - Contract suites: `30 pass`, `0 fail`
  - Integration suites: `13 pass`, `0 fail`

## Final Validation Summary

- API continuity contract suite: `30 pass`, `0 fail`
- API continuity integration suite: `13 pass`, `0 fail`

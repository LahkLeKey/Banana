# 018 Coherent World Fusion - API Evidence

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

- Audit finding (T004): Continuity validation accepted non-canonical lane identity formatting (case/whitespace drift) and did not emit explicit diagnostics for this identity mismatch.
- Implementation change (T005): `continuityPayloadService.validateAndNormalize` now enforces canonical lane identity (`trim().toLowerCase()`) and rejects mismatches with:
  - `persistent_world_continuity_lane_identity_mismatch`
- Test updates (T006):
  - Service unit test added for lane identity mismatch diagnostics.
  - Route contract test added for non-canonical lane identity reject path.
  - Drift integration test added for deterministic repeated lane mismatch diagnostics.
- Validation commands:
  - `cd src/typescript/api && bun run test:continuity`
  - `cd src/typescript/api && bun test src/integration/persistent-world-continuity-contract-drift.integration.test.ts`
- Validation result:
  - Continuity contract suites: `20 pass`, `0 fail`
  - Drift integration suite: `4 pass`, `0 fail`

## US2 Evidence

- Audit findings (T008): Remaining legacy drift families included route keys using non-canonical legacy delimiters and mixed-entry variant ordering that still must resolve to one authoritative area lineage.
- Validation expansion (T009): Added explicit legacy route delimiter reject path in continuity payload validation:
  - `persistent_world_continuity_route_key_legacy_format`
- Deterministic diagnostics expansion (T010): Added repeated-request deterministic diagnostics coverage for legacy route delimiter drift.
- Route/integration tests (T011-T012):
  - `world.continuity.contract.test.ts`: route-level legacy delimiter reject coverage.
  - `persistent-world-continuity-contract-drift.integration.test.ts`: deterministic reject diagnostics for legacy delimiter.
  - `persistent-world-continuity-roundtrip.integration.test.ts`: mixed-entry commit path (`neo-musa -> metro-crescent` then reverse) preserves one authoritative area lineage.
- Validation commands:
  - `cd src/typescript/api && bun run test:continuity`
  - `cd src/typescript/api && bun run test:integration:continuity`
- Validation results:
  - Contract suites: `22 pass`, `0 fail`
  - Integration suites: `9 pass`, `0 fail`

## US3 Evidence

- Additive field introduced in owning schema path (T014):
  - `checkpointFusionLaneTag` with default `fusion:lane-default`.
- Deterministic signature/replay update (T015):
  - Signature derivation now includes `checkpointFusionLaneTag` (with compatibility default fallback).
- Mutation safety coverage (T016):
  - Unit test verifies deterministic signature mutation with `checkpointFusionLaneTag` changes.
  - Growth-path integration verifies commit + rehydrate persistence for `checkpointFusionLaneTag`.
- Validation commands:
  - `cd src/typescript/api && bun run test:continuity`
  - `cd src/typescript/api && bun run test:integration:continuity`
- Validation results:
  - Contract suites: `23 pass`, `0 fail`
  - Integration suites: `9 pass`, `0 fail`

## Final Validation Summary

- Contract suite closeout (T019):
  - `bun run test:continuity`
  - Result: `23 pass`, `0 fail`
- Integration suite closeout (T020):
  - `bun run test:integration:continuity`
  - Result: `9 pass`, `0 fail`
- Final API status: `32/32` continuity-focused checks passed in closeout run.

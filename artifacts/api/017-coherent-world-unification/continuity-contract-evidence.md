# 017 Coherent World Unification - API Evidence

## Baseline Command Inventory

- API continuity contract suite:
  - `cd src/typescript/api && bun run test:continuity`
- API integration continuity suite:
  - `cd src/typescript/api && bun run test:integration:continuity`
- Focused replay/contract/revisit suites (as needed):
  - `cd src/typescript/api && bun test src/domains/persistent-world-orchestration/services/replayMergeService.test.ts`
  - `cd src/typescript/api && bun test src/lib/contracts/v1/contracts.test.ts`
  - `cd src/typescript/api && bun test src/routes/world.revisit.contract.test.ts`

## US2 Evidence

- Audit finding (T008): Continuity payload validation enforced route/signature checks but variant identity canonicalization/support was implicit, allowing legacy/malformed variant IDs to reach route/signature paths.
- Implementation changes (T009-T010):
  - Added canonical variant normalization guard with explicit diagnostics:
    - `persistent_world_continuity_variant_identity_mismatch`
  - Added supported-variant allowlist guard with explicit diagnostics:
    - `persistent_world_continuity_variant_unsupported`
  - Canonical route derivation now always normalizes variant IDs before sorting.
- Test updates (T011-T012):
  - `continuityPayloadService.test.ts`
    - Added non-canonical variant formatting reject case.
    - Added unsupported variant reject case.
  - `world.continuity.contract.test.ts`
    - Added route-level unsupported variant diagnostics case.
  - `persistent-world-continuity-contract-drift.integration.test.ts`
    - Added deterministic unsupported variant diagnostics across repeated requests.
- Validation commands:
  - `cd src/typescript/api && bun run test:continuity`
  - `cd src/typescript/api && bun run test:integration:continuity`
- Validation results:
  - Contract suites: `17 pass`, `0 fail`
  - Integration continuity suites: `6 pass`, `0 fail`

## US3 Evidence

- Additive field introduced (T014): `checkpoint.checkpointRouteSignatureTag` in owning schema path (`persistentWorld.ts`) with default `route:default`.
- Signature and replay-path update (T015): deterministic transition signature derivation now includes `checkpointRouteSignatureTag` with compatibility defaulting for older builders.
- Growth safety tests (T016):
  - `continuityPayloadService.test.ts`
    - Added deterministic mutation test for `checkpointRouteSignatureTag`.
  - `persistent-world-continuity-growth-path.integration.test.ts`
    - Commit + rehydrate asserts for `checkpointRouteSignatureTag: route:growth-v2`.
- Validation commands:
  - `cd src/typescript/api && bun run test:continuity`
  - `cd src/typescript/api && bun run test:integration:continuity`
- Validation results:
  - Contract suites: `18 pass`, `0 fail`
  - Integration continuity suites: `6 pass`, `0 fail`

## Final Validation Summary

- Continuity contract suites:
  - `bun run test:continuity`
  - Final result: `18 pass`, `0 fail`
- Continuity integration suites:
  - `bun run test:integration:continuity`
  - Final result: `6 pass`, `0 fail`
- Final API status: `24/24` continuity-focused API checks passed in closeout run.

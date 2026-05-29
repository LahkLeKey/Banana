# Plan: Checkpoint Id Cohesion

**Spec**: `.specify/specs/023-checkpoint-id-cohesion/spec.md`

## Approach

Mirror proven 020/021/022 canonical-string + replay-determinism + additive-attribute pattern, applied to `checkpointId` and additive `checkpointObservationLaneTag`.

## Owning Surfaces

- `src/typescript/api/src/lib/contracts/v1/persistentWorld.ts`
- `src/typescript/api/src/domains/persistent-world-orchestration/services/continuityPayloadService.ts`
- `src/typescript/api/src/domains/persistent-world-orchestration/services/continuityPayloadService.test.ts`
- `src/typescript/api/src/routes/world.continuity.contract.test.ts`
- `src/typescript/api/src/integration/persistent-world-continuity-contract-drift.integration.test.ts`
- `src/typescript/api/src/integration/persistent-world-continuity-roundtrip.integration.test.ts`
- `src/typescript/api/src/integration/persistent-world-continuity-growth-path.integration.test.ts`

## Delivery Evidence

- `artifacts/native/023-checkpoint-id-cohesion/continuity-runtime-evidence.md`
- `artifacts/api/023-checkpoint-id-cohesion/continuity-contract-evidence.md`

## Final Command Set

- `cd src/typescript/api && bun run test:continuity` → 43 pass / 0 fail.
- `cd src/typescript/api && bun run test:integration:continuity` → 19 pass / 0 fail.
- `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)"` → 4/4 pass.

## Status

Complete.

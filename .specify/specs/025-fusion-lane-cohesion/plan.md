# Plan: Fusion Lane Cohesion

**Spec**: `.specify/specs/025-fusion-lane-cohesion/spec.md`

## Approach

Mirror 024 canonical-string + replay-determinism + additive-attribute pattern applied to `checkpointFusionLaneTag` and additive `checkpointEchoLaneTag`.

## Owning Surfaces

- `src/typescript/api/src/lib/contracts/v1/persistentWorld.ts`
- `src/typescript/api/src/domains/persistent-world-orchestration/services/continuityPayloadService.ts` (+ `.test.ts`)
- `src/typescript/api/src/routes/world.continuity.contract.test.ts`
- `src/typescript/api/src/integration/persistent-world-continuity-*.integration.test.ts`

## Delivery Evidence

- `artifacts/api/025-fusion-lane-cohesion/continuity-contract-evidence.md`
- `artifacts/native/025-fusion-lane-cohesion/continuity-runtime-evidence.md`

## Final Command Set

- `cd src/typescript/api && bun run test:continuity`
- `cd src/typescript/api && bun run test:integration:continuity`
- `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)"`

## Status

Complete.

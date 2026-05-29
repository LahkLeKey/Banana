# Plan: Synthesis Pass Cohesion

Mirrors 025 pattern applied to `checkpointSynthesisPassTag` and additive `checkpointResonanceLaneTag`.

## Final Command Set

- `cd src/typescript/api && bun run test:continuity`
- `cd src/typescript/api && bun run test:integration:continuity`
- `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)"`

## Status

Complete.

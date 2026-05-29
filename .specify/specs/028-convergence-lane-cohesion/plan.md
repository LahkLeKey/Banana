# Plan: Convergence Lane Cohesion

Mirrors 027. Status: Complete.

- `cd src/typescript/api && bun run test:continuity`
- `cd src/typescript/api && bun run test:integration:continuity`
- `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)"`

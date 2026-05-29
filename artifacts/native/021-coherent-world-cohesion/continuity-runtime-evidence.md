# 021 Coherent World Cohesion - Runtime Evidence

## Baseline Command Inventory

- Configure native build tree:
  - `cmake -S src/native -B out/v3-native`
- Focused serial native build targets:
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_profile_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_transition_continuity_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_transition_drift_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_continuity_roundtrip_test -- -m:1`
- Run focused suite:
  - `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)"`

## US1 Evidence

- API-owned canonical fingerprint enforcement; no native-side mutation required to preserve coherent profile guard.
- Verified native coherent profile suite still passes after API-side schema additive change.

## US2 Evidence

- Native transition continuity + drift suites continue to pass under additive schema; replay determinism unaffected by API-only fingerprint retry coverage.

## US3 Evidence

- Native continuity roundtrip suite continues to pass against additive schema (additive field is API-contract-only).

## Final Validation Summary

- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_profile_test banana_runtime_demo_scene_catalog_transition_continuity_test banana_runtime_demo_scene_catalog_transition_drift_test banana_runtime_demo_scene_catalog_continuity_roundtrip_test -- -m:1` -> build succeeded.
- `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)"` -> 4 / 4 passed.

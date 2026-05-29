# 018 Coherent World Fusion - Runtime Evidence

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

- Audit finding (T004): Native coherence consistency did not bind `bootstrap_signature` to the authoritative per-variant signature, allowing profile-level signature tampering.
- Implementation change (T005): `banana_poc_demo_scene_catalog_coherent_profile_consistent` now requires:
  - non-zero `bootstrap_signature`
  - exact equality with `banana_poc_demo_scene_catalog_bootstrap_signature_for_variant(profile->browser_variant)`
- Test update (T006): `runtime_demo_scene_catalog_coherent_failure_path_test.c` now includes bootstrap signature tamper rejection coverage.
- Validation commands:
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_failure_path_test -- -m:1`
  - `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|coherent_failure_path_test|transition_continuity_test)"`
- Validation result: `3/3` tests passed.

## US2 Evidence

- Native US2 contract seams required no additional scene-catalog logic changes in this slice.
- Cross-domain verification: API legacy route drift rejects did not alter native coherent profile/transition behavior.

## US3 Evidence

- Additive continuity growth field implemented in API contract/signature path only (`checkpointFusionLaneTag`).
- Native contract impact: none required for this additive API continuity evolution lane.

## Final Validation Summary

- US1 focused native validation:
  - `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|coherent_failure_path_test|transition_continuity_test)"`
  - Result: `3/3` passed.
- Hardening native validation (T018):
  - `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)"`
  - Result: `4/4` passed.
- Final native status: `7/7` focused coherence/continuity tests passed across implementation + hardening runs.

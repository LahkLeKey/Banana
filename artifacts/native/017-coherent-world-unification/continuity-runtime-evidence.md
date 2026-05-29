# 017 Coherent World Unification - Runtime Evidence

## Baseline Command Inventory

- Configure native build tree:
  - `cmake -S src/native -B out/v3-native`
- Focused serial native build targets (Windows lock-safe):
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_profile_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_launch_signature_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_failure_path_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_transition_continuity_test -- -m:1`
- Run focused continuity/drift suites:
  - `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|coherent_launch_signature_test|coherent_failure_path_test|transition_continuity_test)"`

## US1 Evidence

- Audit finding (T004): `banana_poc_demo_scene_catalog_coherent_profile_consistent` validated structure but did not assert catalog-authoritative identity fields remained aligned to `browser_variant`.
- Implementation change (T005): Added `banana_poc_demo_scene_profile_matches_catalog_identity` and required it in coherence consistency checks.
  - Enforced invariant fields: `kind`, primary/secondary region IDs, `route_id`, `scene_key`, `asset_pack_id`, `anchor_county_id`, and gameplay theme derived from the canonical pack.
- Test change (T006): Extended `runtime_demo_scene_catalog_coherent_failure_path_test.c` with tamper coverage:
  - `browser_variant` tamper now fails coherence.
  - `scene_key` tamper now fails coherence.
- Validation commands:
  - `cmake -S src/native -B out/v3-native`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_profile_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_launch_signature_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_failure_path_test -- -m:1`
  - `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_transition_continuity_test -- -m:1`
  - `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|coherent_launch_signature_test|coherent_failure_path_test|transition_continuity_test)"`
- Result: `4/4` passed, `0` failed.

## US3 Evidence

- No native scene-catalog structure change required for this additive API continuity field.
- Runtime contract impact reviewed: coherent scene identity/transition signatures remain unchanged in native layer for this US3 increment.
- Cross-domain note: API additive checkpoint field is isolated to owning API continuity contract/signature path and does not alter native launch profile invariants.

## Final Validation Summary

- Native US1 implementation validation:
  - `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|coherent_launch_signature_test|coherent_failure_path_test|transition_continuity_test)"`
  - Result: `4/4` passed.
- Native hardening closeout validation:
  - `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)"`
  - Result: `4/4` passed.
- Final native status: `8/8` focused continuity/coherence checks passed across implementation and closeout runs.

# 015 Coherent World Diagnostics

Date: 2026-05-26

## Scope

Combine existing world slices into one coherent-world launch identity using the native scene catalog, route/county contracts, objective policy, and gameplay model placements.

## Cross-Slice Contract Inventory

Primary source slices to consolidate:

- `.specify/specs/009-persistent-world-orchestration/`
- `.specify/specs/010-client-static-mesh-banana-line/`
- `.specify/specs/011-client-world-assets-demo-scenes/`
- `.specify/specs/014-add-gameplay-assets/`

Owning runtime surfaces:

- `src/native/engine/win32_dx12_poc/scene/`
- `src/native/engine/win32_dx12_poc/overlay/`
- `src/native/engine/win32_dx12_poc/app/`
- `src/native/engine/runtime/terrain/static_mesh/`
- `tests/native/runtime/engine/`

## Baseline Coherent-World Signature Capture

Captured command:

- `BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=2 ./out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe`

Key output:

- `[dx12-poc] startup scene override variant=2 key=banana-mainline-neo-musa kind=1 asset-pack=continent/metro-banana-v1 gameplay-theme=theme/urban-industrial placements=3`
- `[dx12-poc] coherent-world variant=2 scene=banana-mainline-neo-musa kind=1 primary=2 secondary=-1 route=0 anchor=neo-musa theme=theme/urban-industrial placements=3 signature=1167355952 status=ok consistent=1`
- `[dx12-poc] gameplay-placement variant=2 index=1 role=landmark model=gameplay/urban-industrial/rail-hub-v1 fallback=gameplay/reference/banana-basic-v1 tag=station-landmark pos=(3.0,0.0,-2.0)`
- `[dx12-poc] gameplay-placement variant=2 index=2 role=traversal model=gameplay/urban-industrial/platform-barrier-v1 fallback=gameplay/reference/banana-basic-v1 tag=station-traversal pos=(0.5,0.0,4.5)`
- `[dx12-poc] startup smoke passed after 1 second(s)`

Expected signature dimensions:

- scene key
- primary/secondary region ids
- route id and adjacency validation
- county anchor validation
- gameplay theme id
- gameplay placement count + deterministic placement signature
- objective policy variant binding

## Planned Validation Commands

- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_bootstrap_test`
- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_coherent_profile_test banana_runtime_demo_scene_catalog_bootstrap_test banana_engine_win32_dx12_poc`
- `ctest -C Debug -R "runtime_demo_scene_catalog_(bootstrap|modularity|gameplay_validation|asset_resolution|theme_coverage|theme_determinism|mutation|disabled_diagnostics)_test" --test-dir out/v3-native --output-on-failure`
- `ctest -C Debug -R "runtime_demo_scene_catalog_(bootstrap|coherent_profile)_test" --test-dir out/v3-native --output-on-failure`
- `BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=<variant> ./out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe`

## Foundational Validation Result (T004-T007)

- Build targets compiled: `banana_runtime_demo_scene_catalog_coherent_profile_test`, `banana_runtime_demo_scene_catalog_bootstrap_test`, `banana_engine_win32_dx12_poc`
- Focused ctest result: 2/2 passed (`bootstrap`, `coherent_profile`), 0 failed
- Runtime coherent-world signature line emitted for variant `2` with `status=ok` and `consistent=1`

## US1 Implementation Notes (T010-T012)

- Coherent-world profile contract now lives in `src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.{h,c}` via:
	- `banana_poc_demo_scene_catalog_build_coherent_profile(...)`
	- `banana_poc_demo_scene_catalog_coherent_profile_consistent(...)`
- Launch diagnostics in `src/native/engine/win32_dx12_poc/app/main.c` now emit a dedicated coherent-world identity line with signature + consistency.
- Scene browser and in-world HUD in `src/native/engine/win32_dx12_poc/overlay/scene_overlay.c` now surface coherent status and signature context for intentional playtests.

## US1 Test Validation Result (T008-T009)

- Added tests:
	- `tests/native/runtime/engine/runtime_demo_scene_catalog_coherent_launch_signature_test.c`
	- `tests/native/runtime/engine/runtime_demo_scene_catalog_coherent_failure_path_test.c`
- Focused ctest command:
	- `ctest -C Debug -R "runtime_demo_scene_catalog_(coherent_profile|coherent_launch_signature|coherent_failure_path|bootstrap)_test" --test-dir out/v3-native --output-on-failure`
- Result: 4/4 passed (`bootstrap`, `coherent_profile`, `coherent_launch_signature`, `coherent_failure_path`), 0 failed

## US2/US3 Continuity and Growth Validation

- Focused ctest command:
	- `ctest -C Debug -R "runtime_demo_scene_catalog_(coherent_mutation_safety|coherent_disabled_profile|transition_continuity|transition_drift|coherent_profile)_test" --test-dir out/v3-native --output-on-failure`
- Result: 5/5 passed (`coherent_profile`, `transition_continuity`, `transition_drift`, `coherent_mutation_safety`, `coherent_disabled_profile`), 0 failed

## Minimal Add-A-Model Workflow (Coherent World)

1. Register or adjust a model key in `k_demo_scene_gameplay_model_catalog` in `src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c`.
2. Reference that key from `k_demo_scene_gameplay_placement_templates` for the targeted coherent-world variants.
3. Keep fallback keys anchored to `reference-banana` unless stricter failure behavior is explicitly required.
4. Validate with focused suite:
	 - `ctest -C Debug -R "runtime_demo_scene_catalog_(coherent_mutation_safety|coherent_disabled_profile|coherent_profile)_test" --test-dir out/v3-native --output-on-failure`
5. Confirm coherent identity still reports `consistent=1` for enabled target variants and explicit diagnostics for disabled/mismatch paths.

## Final Focused Validation Summary

- Command:
	- `ctest -C Debug -R "runtime_demo_scene_catalog_(bootstrap|coherent_profile|coherent_launch_signature|coherent_failure_path|transition_continuity|transition_drift|coherent_mutation_safety|coherent_disabled_profile)_test" --test-dir out/v3-native --output-on-failure`
- Result:
	- 8/8 tests passed, 0 failed.

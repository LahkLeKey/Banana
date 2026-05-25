# 011 Setup Evidence (T001-T003)

Date: 2026-05-25

## Implemented

- Continent asset-pack registry scaffold:
  - src/native/engine/win32_dx12_poc/scene/continent_asset_registry.h
  - src/native/engine/win32_dx12_poc/scene/continent_asset_registry.c
- Modular demo-scene catalog contract:
  - src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.h
  - src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c
- Scene-flow and overlay integration for catalog-driven scene browser entries.
- Scene-browser index/variant clamping via catalog helper contracts in DX12 POC bootstrap path.
- New native determinism test target:
  - tests/native/runtime/engine/runtime_demo_scene_catalog_bootstrap_test.c

## Validation Commands

- cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_bootstrap_test banana_engine_win32_dx12_poc
- ctest -C Debug -R "runtime_demo_scene_catalog_bootstrap_test|engine_win32_dx12_poc_startup_smoke" --output-on-failure

## Validation Result

- banana_runtime_demo_scene_catalog_bootstrap_test: pass
- banana_engine_win32_dx12_poc_startup_smoke: pass
- 2/2 tests passed, 0 failed

## Foundational Contracts (T004-T006)

- Canonical continent mappings are enforced by requiring one non-legacy asset pack per canonical static-mesh region profile.
- Banana Line demo slices now declare explicit route/region anchor contracts (station/corridor) in the scene catalog.
- Scene launch diagnostics now return machine-readable validation reasons:
  - unknown-scene
  - scene-disabled
  - missing-asset-pack
  - canonical-region-pack-mismatch
  - route-anchor-mismatch
  - county-anchor-mismatch

Focused re-validation commands:

- cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_bootstrap_test banana_engine_win32_dx12_poc
- ctest -C Debug -R "runtime_demo_scene_catalog_bootstrap_test|engine_win32_dx12_poc_startup_smoke" --output-on-failure

Result:

- 2/2 tests passed, 0 failed

## Focused Slice Playtests (T009, T012)

Continent slice (variant 0):

- Command:
  BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=0 ./banana_engine_win32_dx12_poc.exe
- Key output:
  - [dx12-poc] startup scene override variant=0 key=continent-stem-territories kind=0 asset-pack=continent/stem-territories-v1
  - [dx12-poc] startup smoke passed after 1 second(s)

Banana Line corridor slice (variant 3):

- Command:
  BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=3 ./banana_engine_win32_dx12_poc.exe
- Key output:
  - [dx12-poc] startup scene override variant=3 key=banana-mainline-sun-metro kind=2 asset-pack=continent/sun-coast-v1
  - [dx12-poc] startup smoke passed after 1 second(s)

Banana Line station slice (variant 2):

- Command:
  BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=2 ./banana_engine_win32_dx12_poc.exe
- Key output:
  - [dx12-poc] startup scene override variant=2 key=banana-mainline-port-koba kind=1 asset-pack=continent/stem-territories-v1
  - [dx12-poc] startup smoke passed after 1 second(s)

## Modularity Validation (T014)

- Added test target: banana_runtime_demo_scene_catalog_modularity_test
- Validation command:
  ctest -C Debug -R "runtime_demo_scene_catalog_modularity_test|runtime_demo_scene_catalog_bootstrap_test|engine_win32_dx12_poc_startup_smoke" --output-on-failure
- Result:
  - banana_runtime_demo_scene_catalog_modularity_test: pass
  - banana_runtime_demo_scene_catalog_bootstrap_test: pass
  - banana_engine_win32_dx12_poc_startup_smoke: pass
  - 3/3 tests passed, 0 failed

Notes:

- Startup overrides now reject invalid or disabled scene variants with explicit validation diagnostics.
- Enabled continent and Banana Line slices launch with non-legacy regional asset pack IDs.

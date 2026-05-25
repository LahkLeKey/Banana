# Quickstart: Client World Assets and Modular Demo Scene Playtests

## Native build and scene-playtest loop

1. Configure once:
   cmake -S src/native -B out/v3-native

2. Build the DX12 POC and targeted scene validation tests:
   cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_bootstrap_test banana_runtime_demo_scene_catalog_modularity_test banana_engine_win32_dx12_poc

3. Run focused deterministic validation:
   ctest -C Debug -R "runtime_demo_scene_catalog_bootstrap_test|runtime_demo_scene_catalog_modularity_test|engine_win32_dx12_poc_startup_smoke" --test-dir out/v3-native --output-on-failure

4. Launch DX12 POC scene browser for modular slice validation:
   C:/Github/Banana/out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe

5. Launch focused scene variants directly for playtests:
   Continent (variant 0):
   BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=0 C:/Github/Banana/out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe
   Continent (variant 1):
   BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=1 C:/Github/Banana/out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe
   Station (variant 2):
   BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=2 C:/Github/Banana/out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe
   Corridor (variant 3):
   BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=3 C:/Github/Banana/out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe

6. Record continent/Banana Line playtest outputs under:
   artifacts/native/011-client-world-assets-demo-scenes/

7. Apply onboarding diagnostics before enabling a new slice:
   artifacts/native/011-client-world-assets-demo-scenes/modular-slice-diagnostics-checklist.md

## Incremental continent asset-pack workflow

1. Update continent asset-pack mapping contracts for a single region slice.
2. Rebuild targeted scene/bootstrap tests.
3. Launch the corresponding demo scene and confirm non-legacy asset pack bindings via startup logs.
4. Repeat for next continent slice.

## Incremental Banana Line scene-slice workflow

1. Register one station or corridor scene entry in the demo-scene catalog.
2. Bind route ID + stop anchors using canonical static-mesh route metadata.
3. Launch and verify deterministic scene bootstrap signature parity.
4. Use scene override values for smoke runs: station variant 2, corridor variant 3.
5. Capture playtest evidence before enabling additional slices.

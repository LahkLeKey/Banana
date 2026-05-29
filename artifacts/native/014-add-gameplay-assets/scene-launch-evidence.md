# 014 Scene Launch Evidence

Date: 2026-05-26

## Target Validation Scenes

Initial focused validation surfaces inherited from the modular demo-scene slice:

- Variant `0`: `continent-stem-territories`
- Variant `1`: `continent-north-crown`
- Variant `2`: `banana-mainline-neo-musa`
- Variant `3`: `banana-mainline-metro-crescent`

## Purpose

- Variant `0` covers tropical/coastal gameplay theme wiring.
- Variant `1` covers rugged/wild gameplay theme wiring.
- Variant `2` covers urban/industrial station theming.
- Variant `3` covers urban/industrial corridor theming with distinct landmark/traversal models.

## Current Status

- Foundational gameplay model metadata has been added to the owning continent asset-pack contract.
- Deterministic bootstrap validation now includes gameplay theme and model identifiers.
- Runtime scene-launch evidence now confirms tropical, rugged, and urban themed variant launches.

## Captured Runtime Evidence

Command:

- `BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=0 ./out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe`

Key output:

- `[dx12-poc] startup scene override variant=0 key=continent-stem-territories kind=0 asset-pack=continent/stem-territories-v1 gameplay-theme=theme/tropical-coastal placements=3`
- `[dx12-poc] gameplay-placement variant=0 index=0 role=reference model=gameplay/reference/banana-basic-v1 fallback=none tag=reference-anchor pos=(-2.0,0.0,1.0)`
- `[dx12-poc] gameplay-placement variant=0 index=1 role=landmark model=gameplay/tropical-coastal/volcanic-arch-v1 fallback=gameplay/reference/banana-basic-v1 tag=landmark-cluster pos=(4.0,0.0,-3.5)`
- `[dx12-poc] gameplay-placement variant=0 index=2 role=traversal model=gameplay/tropical-coastal/palm-cluster-v1 fallback=gameplay/reference/banana-basic-v1 tag=traversal-route pos=(1.5,0.0,6.0)`
- `[dx12-poc] startup smoke passed after 1 second(s)`

Comparative key output (variants `1`, `2`, `3`):

- `[dx12-poc] startup scene override variant=1 key=continent-north-crown kind=0 asset-pack=continent/north-crown-v1 gameplay-theme=theme/rugged-wild placements=3`
- `[dx12-poc] gameplay-placement variant=1 index=1 role=landmark model=gameplay/rugged-wild/ice-gate-v1 fallback=gameplay/reference/banana-basic-v1 tag=landmark-gate pos=(6.0,0.0,-4.0)`
- `[dx12-poc] gameplay-placement variant=1 index=2 role=traversal model=gameplay/rugged-wild/pine-cluster-v1 fallback=gameplay/reference/banana-basic-v1 tag=traversal-route pos=(2.0,0.0,5.0)`
- `[dx12-poc] startup scene override variant=2 key=banana-mainline-neo-musa kind=1 asset-pack=continent/metro-banana-v1 gameplay-theme=theme/urban-industrial placements=3`
- `[dx12-poc] gameplay-placement variant=2 index=1 role=landmark model=gameplay/urban-industrial/rail-hub-v1 fallback=gameplay/reference/banana-basic-v1 tag=station-landmark pos=(3.0,0.0,-2.0)`
- `[dx12-poc] gameplay-placement variant=2 index=2 role=traversal model=gameplay/urban-industrial/platform-barrier-v1 fallback=gameplay/reference/banana-basic-v1 tag=station-traversal pos=(0.5,0.0,4.5)`
- `[dx12-poc] startup scene override variant=3 key=banana-mainline-metro-crescent kind=2 asset-pack=continent/metro-banana-v1 gameplay-theme=theme/urban-industrial placements=3`
- `[dx12-poc] gameplay-placement variant=3 index=1 role=landmark model=gameplay/urban-industrial/archive-gate-v1 fallback=gameplay/reference/banana-basic-v1 tag=corridor-landmark pos=(5.0,0.0,-3.0)`
- `[dx12-poc] gameplay-placement variant=3 index=2 role=traversal model=gameplay/urban-industrial/market-stall-v1 fallback=gameplay/reference/banana-basic-v1 tag=corridor-traversal pos=(1.0,0.0,5.5)`

## Planned Runtime Commands

- `BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=0 C:/Github/Banana/out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe`
- `BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=1 C:/Github/Banana/out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe`
- `BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=2 C:/Github/Banana/out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe`
- `BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=3 C:/Github/Banana/out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe`

## Focused Validation Path Result

Command:

- `ctest -C Debug -R "runtime_demo_scene_catalog_(bootstrap|modularity|gameplay_validation|asset_resolution|theme_coverage|theme_determinism|mutation|disabled_diagnostics)_test" --test-dir out/v3-native --output-on-failure`

Result:

- 8/8 demo-scene catalog tests passed, 0 failed.
---

## US3 Closure Run

Targeted ctest filter executed from `out/v3-native` (Debug):
- banana_runtime_demo_scene_catalog_modularity_test
- banana_runtime_demo_scene_catalog_gameplay_validation_test
- banana_runtime_demo_scene_catalog_asset_resolution_test
- banana_runtime_demo_scene_catalog_theme_coverage_test
- banana_runtime_demo_scene_catalog_theme_determinism_test
- banana_runtime_demo_scene_catalog_mutation_test
- banana_runtime_demo_scene_catalog_disabled_diagnostics_test
- banana_runtime_demo_scene_catalog_bootstrap_test

Result: 8/8 passed (raw ctest output in `scene-launch-evidence-run.txt`).
US3 catalog-mutation and disabled-diagnostics coverage are satisfied by the existing test binaries; no scene-bootstrap rewrites were needed because catalog growth flows through the single owning path documented in `catalog-diagnostics.md`.

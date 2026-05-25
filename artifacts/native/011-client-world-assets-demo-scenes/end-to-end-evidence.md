# End-to-End Evidence Bundle: Spec 011

Date: 2026-05-25
Feature: 011-client-world-assets-demo-scenes

## Scope covered

- Continent asset packs are mapped to canonical regions (non-legacy default path for enabled slices).
- Banana Line station and corridor slices are launchable with deterministic scene bootstrap contracts.
- Scene catalog supports modular append-only growth without unrelated baseline rewrites.

## Build and deterministic test gate

Command:

- cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_bootstrap_test banana_runtime_demo_scene_catalog_modularity_test banana_engine_win32_dx12_poc

Result:

- Build succeeded for all targets.

Command:

- ctest -C Debug -R "runtime_demo_scene_catalog_modularity_test|runtime_demo_scene_catalog_bootstrap_test|engine_win32_dx12_poc_startup_smoke" --output-on-failure

Result:

- 3/3 tests passed, 0 failed.
- runtime_demo_scene_catalog_bootstrap_test: pass
- runtime_demo_scene_catalog_modularity_test: pass
- banana_engine_win32_dx12_poc_startup_smoke: pass

## Focused playtest launch evidence

Continent slice (variant 0):

- startup scene override variant=0 key=continent-stem-territories kind=0 asset-pack=continent/stem-territories-v1
- startup smoke passed after 1 second(s)

Banana Line station slice (variant 2):

- startup scene override variant=2 key=banana-mainline-port-koba kind=1 asset-pack=continent/stem-territories-v1
- startup smoke passed after 1 second(s)

Banana Line corridor slice (variant 3):

- startup scene override variant=3 key=banana-mainline-sun-metro kind=2 asset-pack=continent/sun-coast-v1
- startup smoke passed after 1 second(s)

## Modularity proof points

- Baseline scene catalog entries remain index-stable with explicit key+variant assertions.
- Browser variants are unique and round-trip through index lookup.
- Additive scene growth is allowed by requiring baseline preservation while tolerating additional entries.

## Diagnostic onboarding artifact

- See modular-slice-diagnostics-checklist.md for repeatable onboarding validation.

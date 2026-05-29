# 014 Gameplay Asset Catalog Diagnostics

Date: 2026-05-26

## Current Owning Paths

- Continent asset-pack registry and per-region asset metadata:
  - `src/native/engine/win32_dx12_poc/scene/continent_asset_registry.h`
  - `src/native/engine/win32_dx12_poc/scene/continent_asset_registry.c`
- Demo-scene bootstrap signature and scene-level validation path:
  - `src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.h`
  - `src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c`
- Focused native validation:
  - `tests/native/runtime/engine/runtime_demo_scene_catalog_bootstrap_test.c`
  - `tests/native/runtime/engine/runtime_demo_scene_catalog_gameplay_validation_test.c`
  - `tests/native/runtime/engine/runtime_demo_scene_catalog_asset_resolution_test.c`

## Baseline Inventory

- Current continent asset packs already own per-region `asset_pack_id` and `fallback_tag` metadata.
- The shared terrain fallback path is still `terrain/basic-overworld` for all canonical regions.
- The demo-scene catalog still drives deterministic scene bootstrap through `primary_region_id`, `asset_pack_id`, and `diagnostics_tag`.

## Gameplay Asset Contract Added In This Slice

- Added per-region gameplay metadata on `BananaPocContinentAssetPack`:
  - `gameplay_theme_id`
  - `reference_model_id`
  - `landmark_model_id`
  - `traversal_model_id`
- Added helper validation contract:
  - `banana_poc_continent_asset_pack_has_gameplay_models(...)`
- Extended scene bootstrap signatures so deterministic scene identity now incorporates gameplay theme/model selections from the primary region pack.

## Scene Placement Contract Added In This Slice

- Added explicit per-scene gameplay placement records in `src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c`.
- Each placement now carries:
  - `role`
  - `model_id`
  - `fallback_model_id`
  - `diagnostics_tag`
  - deterministic `x/y/z` coordinates
- Added placement query helpers:
  - `banana_poc_demo_scene_gameplay_placement_count_for_variant(...)`
  - `banana_poc_demo_scene_gameplay_placement_at(...)`
- Enabled-scene validation now fails with `gameplay-model-mismatch` when a scene lacks placements, lacks landmark/traversal roles, or lacks both a primary and fallback model reference.

## Theme Coverage

- `theme/tropical-coastal`
- `theme/urban-industrial`
- `theme/rugged-wild`

## Validation Commands

- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_bootstrap_test`
- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_gameplay_validation_test`
- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_asset_resolution_test`
- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_theme_coverage_test`
- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_theme_determinism_test`
- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_modularity_test`
- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_mutation_test`
- `cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_disabled_diagnostics_test`
- `ctest -C Debug -R "runtime_demo_scene_catalog_bootstrap_test" --test-dir out/v3-native --output-on-failure`
- `ctest -C Debug -R "runtime_demo_scene_catalog_gameplay_validation_test" --test-dir out/v3-native --output-on-failure`
- `ctest -C Debug -R "runtime_demo_scene_catalog_asset_resolution_test|runtime_demo_scene_catalog_gameplay_validation_test" --test-dir out/v3-native --output-on-failure`
- `ctest -C Debug -R "runtime_demo_scene_catalog_(bootstrap|modularity|gameplay_validation|asset_resolution|theme_coverage|theme_determinism|mutation|disabled_diagnostics)_test" --test-dir out/v3-native --output-on-failure`

## Validation Result

- `banana_runtime_demo_scene_catalog_bootstrap_test`: pass
- `banana_runtime_demo_scene_catalog_modularity_test`: pass
- `banana_runtime_demo_scene_catalog_gameplay_validation_test`: pass
- `banana_runtime_demo_scene_catalog_asset_resolution_test`: pass
- `banana_runtime_demo_scene_catalog_theme_coverage_test`: pass
- `banana_runtime_demo_scene_catalog_theme_determinism_test`: pass
- `banana_runtime_demo_scene_catalog_mutation_test`: pass
- `banana_runtime_demo_scene_catalog_disabled_diagnostics_test`: pass
- 8/8 focused tests passed, 0 failed

## Minimal Add-A-Model Workflow

1. Add one model key entry in the owning catalog table in `src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c` under `k_demo_scene_gameplay_model_catalog`.
2. Reference that key from targeted variant placements in `k_demo_scene_gameplay_placement_templates` rather than inlining a model ID.
3. Keep fallback paths keyed through `reference-banana` unless a stricter fallback contract is intended.
4. Rebuild and run the focused suite with:
  - `ctest -C Debug -R "runtime_demo_scene_catalog_(mutation|disabled_diagnostics|theme_determinism)_test" --test-dir out/v3-native --output-on-failure`
5. Capture one runtime launch line proving the key resolves to the expected model ID.

## Diagnostics Expectations

- Disabled scene entries must return `scene-disabled` from `banana_poc_demo_scene_catalog_validate_index(...)`.
- Gameplay validator mismatch is acceptable for disabled scenes that intentionally omit placements.
- Missing or disabled model-key resolution should preserve fallback behavior where a fallback key is configured.
- Model-key mutation via `banana_poc_demo_scene_gameplay_model_override(...)` must not require rewriting unrelated scene metadata.

## Remaining Work In This Track

- Integrate gameplay placements into render-path actor spawning once mesh asset IDs are bound to runtime mesh resources.
---

## Add-a-Model Workflow (US3 closure)

Single owning paths:
- Catalog entries: `src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c` (entries + placements arrays).
- Gameplay placement registration: `BananaPocDemoSceneGameplayPlacement` rows keyed by `browser_variant` and `BananaPocGameplayPlacementRole`.
- Continent + asset-pack binding: `continent_asset_registry.{c,h}` neighboring file.

To add a new gameplay model:
1. Add the model key + asset metadata in the continent registry path.
2. Append a placement row in `demo_scene_catalog.c` referencing the model key and the target `browser_variant`.
3. No scene-bootstrap rewrites required; validation surfaces via
   `BANANA_POC_DEMO_SCENE_VALIDATION_GAMEPLAY_MODEL_MISMATCH` if the key is unknown or
   `BANANA_POC_DEMO_SCENE_VALIDATION_DISABLED` if the owning entry is disabled.

Diagnostics expectations:
- Mutation safety guarded by `banana_runtime_demo_scene_catalog_mutation_test` and `banana_runtime_demo_scene_catalog_coherent_mutation_safety_test`.
- Disabled-entry references guarded by `banana_runtime_demo_scene_catalog_disabled_diagnostics_test` and `banana_runtime_demo_scene_catalog_coherent_disabled_profile_test`.
- Determinism + theme parity guarded by `banana_runtime_demo_scene_catalog_theme_determinism_test` and `banana_runtime_demo_scene_catalog_theme_coverage_test`.

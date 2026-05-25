# Modular Slice Onboarding Diagnostics Checklist

Use this checklist each time a new continent, station, corridor, or lab demo scene is added.

## 1) Catalog registration

- [ ] Add a single catalog entry with a unique browser variant and scene key.
- [ ] Keep existing baseline entries in the original order; append new slices after them.
- [ ] Set enabled=0 for slices that are visible-but-not-launchable during staging.

## 2) Canonical region and asset pack contract

- [ ] primary_region_id maps to a registered continent asset pack.
- [ ] asset_pack_id matches the canonical pack for primary_region_id.
- [ ] anchor_county_id resolves to a known county in primary_region_id.

## 3) Banana Line route-anchor contract (when applicable)

- [ ] Station slices bind to a valid route_id and include a stop region present on that route.
- [ ] Corridor slices bind to a valid route_id with adjacent primary/secondary region stops.
- [ ] secondary_region_id is INVALID for station slices and set for corridor slices.

## 4) Determinism and validation checks

- [ ] Scene bootstrap signature is stable across repeated launches.
- [ ] Enabled entries validate as OK; disabled entries validate as scene-disabled.
- [ ] Missing/mismatched bindings surface explicit diagnostics tags and messages.

## 5) Required validation commands

- [ ] cmake --build out/v3-native --target banana_runtime_demo_scene_catalog_bootstrap_test banana_runtime_demo_scene_catalog_modularity_test banana_engine_win32_dx12_poc
- [ ] ctest -C Debug -R "runtime_demo_scene_catalog_bootstrap_test|runtime_demo_scene_catalog_modularity_test|engine_win32_dx12_poc_startup_smoke" --test-dir out/v3-native --output-on-failure
- [ ] BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=<variant> ./out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe

## 6) Evidence updates

- [ ] Append key output lines for continent, station, and corridor startup overrides.
- [ ] Record modularity test results proving baseline entries remain stable with append-only growth.
- [ ] Update execution dashboard and task status in .specify/specs/011-client-world-assets-demo-scenes/.

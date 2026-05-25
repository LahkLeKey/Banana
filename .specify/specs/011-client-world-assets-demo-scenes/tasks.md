# Tasks: Client World Assets, Continents Playtests, and Modular Demo Scenes

**Input**: Design documents from .specify/specs/011-client-world-assets-demo-scenes/

**Prerequisites**: plan.md, spec.md

**Tests**: Native deterministic launch/bootstrap and route-anchor checks are required for each user story increment.

## Phase 1: Setup (Shared Infrastructure)

Path targets:
- src/native/engine/win32_dx12_poc/scene/continent_asset_registry.h
- src/native/engine/win32_dx12_poc/scene/continent_asset_registry.c
- src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.h
- src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c
- src/native/engine/win32_dx12_poc/scene/scene_flow.c
- src/native/engine/win32_dx12_poc/scene_flow.h
- src/native/engine/win32_dx12_poc/overlay/scene_overlay.c
- src/native/engine/CMakeLists.txt
- tests/native/runtime/engine/runtime_demo_scene_catalog_bootstrap_test.c

- [x] T001 Create continent asset-pack registry scaffold in native scene runtime contracts
- [x] T002 Add modular demo-scene catalog contract and registration entry points
- [x] T003 Add baseline native playtest executable/target for scene-catalog bootstrap determinism

## Phase 2: Foundational (Blocking Prerequisites)

Path targets:
- docs/banana-archipelago-worldbuilding.md
- artifacts/generated-assets/worldbuilding/banana-archipelago-region-baseline.json
- src/native/engine/runtime/terrain/static_mesh/terrain_static_mesh_domain.h
- src/native/engine/runtime/terrain/static_mesh/terrain_static_mesh_domain.c
- src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.h
- src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c
- src/native/engine/win32_dx12_poc/scene/scene_flow.c
- src/native/engine/win32_dx12_poc/scene_flow.h
- src/native/engine/win32_dx12_poc/app/main.c

- [x] T004 Define canonical continent-to-asset-pack mappings from docs/banana-archipelago-worldbuilding.md
- [x] T005 Add Banana Line stop-to-scene anchor mapping contract based on static_mesh route IDs
- [x] T006 Add diagnostics surface for missing asset packs and route-anchor mismatches

## Phase 3: User Story 1 - Client Asset Packs for New Continents (Priority: P1)

Path targets:
- src/native/engine/win32_dx12_poc/scene/continent_asset_registry.h
- src/native/engine/win32_dx12_poc/scene/continent_asset_registry.c
- src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c
- tests/native/runtime/engine/runtime_demo_scene_catalog_bootstrap_test.c
- artifacts/native/011-client-world-assets-demo-scenes/setup-evidence.md

- [x] T007 [US1] Implement continent-specific non-legacy client asset pack bindings for targeted regions
- [x] T008 [US1] Add deterministic scene bootstrap signature checks for continent demo slices
- [x] T009 [US1] Capture focused playtest evidence proving old basic placeholder set is no longer default in covered regions

## Phase 4: User Story 2 - Banana Line Station and Corridor Slices (Priority: P1)

Path targets:
- src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.h
- src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c
- tests/native/runtime/engine/runtime_demo_scene_catalog_bootstrap_test.c
- src/native/engine/win32_dx12_poc/app/main.c
- artifacts/native/011-client-world-assets-demo-scenes/setup-evidence.md

- [x] T010 [US2] Implement Banana Line station/corridor demo-scene entries with deterministic anchor bindings
- [x] T011 [US2] Add route-anchor parity tests for each enabled Banana Line demo slice
- [x] T012 [US2] Capture traversal launch evidence for at least one station slice and one corridor slice

## Phase 5: User Story 3 - Modular Demo Scene Catalog (Priority: P2)

Path targets:
- src/native/engine/win32_dx12_poc/overlay/scene_overlay.c
- src/native/engine/win32_dx12_poc/scene/scene_flow.c
- src/native/engine/win32_dx12_poc/scene_flow.h
- tests/native/runtime/engine/runtime_demo_scene_catalog_modularity_test.c
- src/native/engine/CMakeLists.txt
- artifacts/native/011-client-world-assets-demo-scenes/modular-slice-diagnostics-checklist.md

- [x] T013 [US3] Implement scene-catalog metadata toggles (enabled/disabled) and scene-browser rendering hooks
- [x] T014 [US3] Add catalog mutation test proving new scene entries can be added without unrelated scene rewrites
- [x] T015 [US3] Publish quick diagnostics checklist for modular slice onboarding

## Phase 6: Polish and Cross-Cutting

Path targets:
- .specify/specs/execution-dashboard.md
- .specify/specs/011-client-world-assets-demo-scenes/quickstart.md
- artifacts/native/011-client-world-assets-demo-scenes/end-to-end-evidence.md

- [x] T016 Update .specify/specs/execution-dashboard.md with 011 status progress
- [x] T017 Refresh quickstart with continent and Banana Line modular scene playtest commands
- [x] T018 Capture end-to-end evidence bundle for continent assets, Banana Line scenes, and scene-catalog modularity

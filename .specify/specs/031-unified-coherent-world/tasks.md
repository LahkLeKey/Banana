# Tasks: Unified Coherent Game World

## Phase 1: Unification

- [x] T001 Draft unified spec collapsing 015–030 (`spec.md`).
- [x] T002 Identify canonical umbrella ctest filter covering all folded slice contracts.
- [x] T003 Run umbrella baseline and capture evidence at `artifacts/native/031-unified-coherent-world/umbrella-baseline.txt`.
- [x] T004 Mark 015–030 as superseded in `.specify/specs/execution-dashboard.md` and add 031 row.

## Phase 2: Guard Rails

- [x] T005 Document growth rule: extend 031 + canonical owning paths instead of creating new cohesion-lane specs.
- [x] T006 Cross-link this spec from `014-add-gameplay-assets/catalog-diagnostics.md` (growth workflow) — link recorded in spec FR / US3.

## Phase 3: Full-MMO Demo

- [x] T007 Author `tests/native/runtime/engine/runtime_full_mmo_demo_test.c` and CMake target `banana_runtime_full_mmo_demo_test`, exercising real ABIs in 7 stages end-to-end (engine_init via launch-gate env, multi-player upsert, click-to-move via `runtime_player_motion_tick`, real merchant ABI economy, pickup, sync/staleness/rejoin, coherent transition + self-loop signature). Launch-gate env (`BANANA_LAUNCH_GATE_VERIFICATION_AVAILABLE=1;BANANA_LAUNCH_GATE_ACCOUNT_IN_GOOD_STANDING=1`) supplied via `set_tests_properties` on both this test and the pre-existing `banana_engine_merchant_trade_test` (which was previously red and is now green). Evidence: `artifacts/native/031-unified-coherent-world/full-mmo-demo.txt`; umbrella 29/29.

## Phase 4: Bulk TODO Consolidation

- [x] T008 Re-verify canonical unified-world baselines on 2026-05-27.
  - Umbrella subset: `ctest --test-dir out/v3-native -C Debug --output-on-failure -R "banana_(runtime_full_mmo_demo|runtime_demo_scene_catalog_.+|dx12_(objective_policy|scene_flow|scene_overlay_frame|projection_policy|gameplay_lane|gameplay_click_lane)|runtime_tick_(input_phase|post_phase|budget_policy)|runtime_engine_tick|runtime_camera_follow_policy|runtime_move_target_domain)_test"` -> 29 / 29 passed.
  - Full native suite: `ctest --test-dir out/v3-native -C Debug --output-on-failure` -> 107 / 107 passed.
- [x] T009 Add CI guard that blocks creation/extension of new cohesion-lane numbered specs for this surface (must extend `031-unified-coherent-world` instead) via `scripts/validate-coherent-world-unified-slice.sh` and wired CI step in `.github/workflows/banana.yml`.
- [x] T010 Create a canonical migration checklist that maps each superseded spec (015-030) to the owning section(s) in 031 for fast onboarding (`migration-checklist.md`).
- [x] T011 Add a single command/runbook step that refreshes 031 evidence artifacts (`full-engine-baseline.txt`, `umbrella-baseline.txt`, `full-mmo-demo.txt`) in one pass via `scripts/refresh-coherent-world-evidence.sh`.
- [x] T012 Add an API baseline refresh task under 031 and capture updated API unit/integration evidence alongside native baselines via `scripts/refresh-coherent-world-api-baselines.sh` (`coherent-world-units.log` refreshed with stable unit subset; `coherent-world-integration.log` refreshed at 45/0).
- [x] T013 Add a "done definition" checklist for future coherent-world changes (spec update + owning tests + evidence + dashboard update) and link it from `execution-dashboard.md` (`done-definition.md`).

## Validation

```bash
ctest --test-dir out/v3-native -C Debug --output-on-failure -R \
  "banana_(runtime_full_mmo_demo|runtime_demo_scene_catalog_.+|dx12_(objective_policy|scene_flow|scene_overlay_frame|projection_policy|gameplay_lane|gameplay_click_lane)|runtime_tick_(input_phase|post_phase|budget_policy)|runtime_engine_tick|runtime_camera_follow_policy|runtime_move_target_domain)_test"
```

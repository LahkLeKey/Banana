#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BANANA_NATIVE_BUILD_DIR:-out/v3-native}"
ARTIFACT_DIR="$ROOT_DIR/artifacts/native/031-unified-coherent-world"

mkdir -p "$ARTIFACT_DIR"

cd "$ROOT_DIR"

echo "[031-refresh] writing umbrella baseline -> $ARTIFACT_DIR/umbrella-baseline.txt"
ctest --test-dir "$BUILD_DIR" -C Debug --output-on-failure -R \
  "banana_(runtime_full_mmo_demo|runtime_demo_scene_catalog_.+|dx12_(objective_policy|scene_flow|scene_overlay_frame|projection_policy|gameplay_lane|gameplay_click_lane)|runtime_tick_(input_phase|post_phase|budget_policy)|runtime_engine_tick|runtime_camera_follow_policy|runtime_move_target_domain)_test" \
  | tee "$ARTIFACT_DIR/umbrella-baseline.txt"

echo "[031-refresh] writing full engine baseline -> $ARTIFACT_DIR/full-engine-baseline.txt"
ctest --test-dir "$BUILD_DIR" -C Debug --output-on-failure \
  | tee "$ARTIFACT_DIR/full-engine-baseline.txt"

echo "[031-refresh] writing full MMO demo baseline -> $ARTIFACT_DIR/full-mmo-demo.txt"
ctest --test-dir "$BUILD_DIR" -C Debug --output-on-failure -R "banana_runtime_full_mmo_demo_test" \
  | tee "$ARTIFACT_DIR/full-mmo-demo.txt"

echo "[031-refresh] writing combat-vs-wildlife war evidence -> $ARTIFACT_DIR/combat-wildlife-war-evidence.txt"
ctest --test-dir "$BUILD_DIR" -C Debug --output-on-failure -R \
  "banana_(combat_controller_signal|wildlife_controller_war_state|runtime_controller_team_war|runtime_tick_post_phase|runtime_gameplay_warfront_expansion|runtime_gameplay_warfront_comeback_bonus|runtime_gameplay_warfront_negotiate_deescalation|runtime_gameplay_warfront_negotiate_factory|runtime_gameplay_warfront_intelligence_feature_unlock|runtime_tick_orchestration_war_terrain_expansion|runtime_tick_orchestration_war_escalation|runtime_tick_orchestration_war_intelligence_growth|runtime_tick_orchestration_war_game_of_life_sentience|runtime_tick_orchestration_war_overcrowd_expansion|runtime_tick_orchestration_war_long_run_soak|runtime_demo_scene_catalog_vector_profile|runtime_demo_scene_catalog_war_policy)_test" \
  | tee "$ARTIFACT_DIR/combat-wildlife-war-evidence.txt"

echo "[031-refresh] complete"

#!/usr/bin/env bash
# Temporary wrapper for building WASM with Emscripten from /tmp/emsdk

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$SCRIPT_DIR/.."
ENGINE_DIR="$ROOT/src/native/engine"
OUT_DIR="$ROOT/out/wasm"
ARTIFACT_DIR="$ROOT/artifacts/wasm"
REACT_PUBLIC="$ROOT/src/typescript/react/public/wasm"

# Source Emscripten
source /tmp/emsdk/emsdk_env.sh
EMCC="python /tmp/emsdk/upstream/emscripten/emcc.py"

OPT="-O0 -g"
[[ "${1:-}" == "--release" ]] && OPT="-O2" && echo "[wasm] Release build" || echo "[wasm] Debug build"

mkdir -p "$OUT_DIR"

SOURCES=(
    "$ENGINE_DIR/engine.c"
    "$ENGINE_DIR/render/window.c"
    "$ENGINE_DIR/render/shader.c"
    "$ENGINE_DIR/render/mesh.c"
    "$ENGINE_DIR/render/material.c"
    "$ENGINE_DIR/render/camera.c"
    "$ENGINE_DIR/render/renderer.c"
    "$ENGINE_DIR/physics/body.c"
    "$ENGINE_DIR/physics/collider.c"
    "$ENGINE_DIR/physics/dynamics.c"
    "$ENGINE_DIR/physics/world.c"
    "$ENGINE_DIR/ai/controller.c"
    "$ENGINE_DIR/ai/controller_system.c"
    "$ENGINE_DIR/ai/state_machine.c"
    "$ENGINE_DIR/ai/navigation.c"
    "$ENGINE_DIR/ai/perception.c"
    "$ENGINE_DIR/ai/wildlife_controller.c"
    "$ENGINE_DIR/world/entity.c"
    "$ENGINE_DIR/world/world.c"
    "$ENGINE_DIR/world/signals.c"
    "$ENGINE_DIR/wasm_main.c"
)

EXPORTS='["_main","_engine_init","_engine_tick","_engine_shutdown","_engine_render_frame","_engine_get_frame_buffer","_engine_world_spawn","_engine_world_tick","_engine_controller_create","_engine_controller_attach","_engine_controller_update","_engine_controller_signal","_engine_get_entity_count","_engine_get_entity_x","_engine_get_entity_z","_engine_get_entity_state","_engine_set_move_input","_engine_get_click_count","_engine_get_target_reached_count","_engine_get_has_move_target","_physics_step","_physics_add_body","_physics_update_body"]'

echo "[wasm] Compiling ${#SOURCES[@]} sources…"

$EMCC \
    "${SOURCES[@]}" \
    -I "$ENGINE_DIR" -I "$ENGINE_DIR/render" -I "$ENGINE_DIR/physics" -I "$ENGINE_DIR/ai" -I "$ENGINE_DIR/world" \
    -std=c11 $OPT \
    -s WASM=1 -s USE_WEBGL2=1 -s FULL_ES3=1 \
    -s EXPORTED_FUNCTIONS="$EXPORTS" \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    -s MODULARIZE=1 -s EXPORT_NAME=BananaEngine \
    -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=33554432 \
    -s NO_EXIT_RUNTIME=1 \
    -s ENVIRONMENT=web \
    -o "$OUT_DIR/engine.js"

echo "[wasm] Build complete: $OUT_DIR/"
mkdir -p "$REACT_PUBLIC"
cp "$OUT_DIR/engine.js" "$REACT_PUBLIC/" && cp "$OUT_DIR/engine.wasm" "$REACT_PUBLIC/" 2>/dev/null || true
mkdir -p "$ARTIFACT_DIR"
cp "$OUT_DIR/engine.js" "$ARTIFACT_DIR/banana-wasm-engine.js" && cp "$OUT_DIR/engine.wasm" "$ARTIFACT_DIR/banana-wasm-engine.wasm" 2>/dev/null || true
echo "[wasm] Copied to $REACT_PUBLIC/"

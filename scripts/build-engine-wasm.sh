#!/usr/bin/env bash
# build-engine-wasm.sh — Compile the Banana game engine to WebAssembly.
#
# Prerequisites:
#   emsdk installed and activated:
#     cd /path/to/emsdk && source ./emsdk_env.sh
#
# Usage:
#   bash scripts/build-engine-wasm.sh [--release]
#
# Outputs:
#   out/wasm/engine.js   — JS loader (auto-loads engine.wasm)
#   out/wasm/engine.wasm — WebAssembly module
#   src/typescript/react/public/wasm/ — copied for Vite/React to serve

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$SCRIPT_DIR/.."
ENGINE_DIR="$ROOT/src/native/engine"
OUT_DIR="$ROOT/out/wasm"
REACT_PUBLIC="$ROOT/src/typescript/react/public/wasm"

# ── Flags ────────────────────────────────────────────────────────────────────
OPT="-O0 -g"
if [[ "${1:-}" == "--release" ]]; then
    OPT="-O2"
    echo "[build-engine-wasm] Release build"
else
    echo "[build-engine-wasm] Debug build (use --release for optimized)"
fi

# ── Verify emcc is available ─────────────────────────────────────────────────
if ! command -v emcc &>/dev/null; then
    echo ""
    echo "ERROR: emcc not found. Activate the Emscripten SDK first:"
    echo "  source /path/to/emsdk/emsdk_env.sh"
    echo ""
    exit 1
fi
echo "[build-engine-wasm] Using emcc $(emcc --version | head -1)"

mkdir -p "$OUT_DIR"

# ── Source files ─────────────────────────────────────────────────────────────
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
    "$ENGINE_DIR/ai/state_machine.c"
    "$ENGINE_DIR/ai/navigation.c"
    "$ENGINE_DIR/ai/perception.c"
    "$ENGINE_DIR/ai/wildlife_controller.c"
    "$ENGINE_DIR/world/entity.c"
    "$ENGINE_DIR/world/world.c"
    "$ENGINE_DIR/world/signals.c"
    "$ENGINE_DIR/wasm_main.c"
)

# ── Exported WASM functions (callable from JS via Module.ccall / cwrap) ──────
EXPORTS='["_main","_engine_world_spawn","_engine_controller_attach","_engine_controller_signal","_engine_tick","_engine_set_move_input","_engine_get_entity_count","_engine_get_entity_x","_engine_get_entity_z","_engine_get_entity_state"]'

# ── Build ─────────────────────────────────────────────────────────────────────
echo "[build-engine-wasm] Compiling ${#SOURCES[@]} source files…"

emcc \
    "${SOURCES[@]}" \
    -I "$ENGINE_DIR" \
    -I "$ENGINE_DIR/render" \
    -I "$ENGINE_DIR/physics" \
    -I "$ENGINE_DIR/ai" \
    -I "$ENGINE_DIR/world" \
    -std=c11 \
    $OPT \
    -s WASM=1 \
    -s USE_WEBGL2=1 \
    -s FULL_ES3=1 \
    -s EXPORTED_FUNCTIONS="$EXPORTS" \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap"]' \
    -s MODULARIZE=1 \
    -s EXPORT_NAME=BananaEngine \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=33554432 \
    -s ENVIRONMENT=web \
    -o "$OUT_DIR/engine.js"

echo "[build-engine-wasm] Build complete: $OUT_DIR/engine.js + engine.wasm"

# ── Copy to React public directory so Vite serves it ─────────────────────────
mkdir -p "$REACT_PUBLIC"
cp "$OUT_DIR/engine.js"   "$REACT_PUBLIC/engine.js"
cp "$OUT_DIR/engine.wasm" "$REACT_PUBLIC/engine.wasm"

echo "[build-engine-wasm] Copied to $REACT_PUBLIC"
echo "[build-engine-wasm] Done. Start React dev server and open /game-engine"

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
ARTIFACTS_DIR="$ROOT/artifacts/wasm"

# ── Flags ────────────────────────────────────────────────────────────────────
OPT="-O0 -g"
if [[ "${1:-}" == "--release" ]]; then
    OPT="-O2"
    echo "[build-engine-wasm] Release build"
else
    echo "[build-engine-wasm] Debug build (use --release for optimized)"
fi

# ── Verify emcc is available ─────────────────────────────────────────────────
EMCC_CMD=(emcc)
if [[ -n "${EMCC_SCRIPT:-}" && -f "${EMCC_SCRIPT}" ]]; then
    case "$EMCC_SCRIPT" in
        *.py)
            EMCC_CMD=(python "${EMCC_SCRIPT}")
            ;;
        *.sh)
            EMCC_CMD=(bash "${EMCC_SCRIPT}")
            ;;
        *)
            EMCC_CMD=("${EMCC_SCRIPT}")
            ;;
    esac
    echo "[build-engine-wasm] Using emcc script from EMCC_SCRIPT=${EMCC_SCRIPT}"
elif command -v emcc &>/dev/null; then
    EMCC_CMD=(emcc)
    echo "[build-engine-wasm] Using emcc $(emcc --version | head -1)"
elif [[ -n "${EMSDK_PATH:-}" && -f "${EMSDK_PATH}/upstream/emscripten/emcc.sh" ]]; then
    EMCC_CMD=(bash "${EMSDK_PATH}/upstream/emscripten/emcc.sh")
    echo "[build-engine-wasm] Using emcc.sh from EMSDK_PATH=${EMSDK_PATH}"
elif [[ -n "${EMSDK_PATH:-}" && -f "${EMSDK_PATH}/upstream/emscripten/emcc.py" ]]; then
    EMCC_CMD=(python "${EMSDK_PATH}/upstream/emscripten/emcc.py")
    echo "[build-engine-wasm] Using emcc.py from EMSDK_PATH=${EMSDK_PATH}"
else
    echo ""
    echo "ERROR: emcc not found. Activate the Emscripten SDK first:"
    echo "  source /path/to/emsdk/emsdk_env.sh"
    echo ""
    exit 1
fi

echo "[build-engine-wasm] Using emcc $("${EMCC_CMD[@]}" --version | head -1)"

mkdir -p "$OUT_DIR"

# ── Source files ─────────────────────────────────────────────────────────────
SOURCES=(
    "$ENGINE_DIR/engine.c"
    "$ENGINE_DIR/engine_serialize.c"
    "$ENGINE_DIR/wasm_main.c"
    "$ENGINE_DIR/tools/domain/banana_asset_domain.c"
)

# Keep this list aligned with the CMake wasm target surfaces.
while IFS= read -r -d '' source_file; do
    SOURCES+=("$source_file")
done < <(find "$ENGINE_DIR/render" "$ENGINE_DIR/physics" "$ENGINE_DIR/ai" "$ENGINE_DIR/world" "$ENGINE_DIR/runtime" -type f -name '*.c' -print0 | sort -z)

# ── Exported WASM functions (callable from JS via Module.ccall / cwrap) ──────
EXPORTS='["_main","_engine_world_spawn","_engine_controller_attach","_engine_controller_signal","_engine_tick","_engine_set_move_input","_engine_handle_right_click","_engine_handle_right_click_normalized","_engine_get_entity_count","_engine_get_entity_x","_engine_get_entity_z","_engine_get_entity_state"]'

# ── Build ─────────────────────────────────────────────────────────────────────
echo "[build-engine-wasm] Compiling ${#SOURCES[@]} source files…"

"${EMCC_CMD[@]}" \
    "${SOURCES[@]}" \
    -I "$ENGINE_DIR" \
    -I "$ENGINE_DIR/render" \
    -I "$ENGINE_DIR/physics" \
    -I "$ENGINE_DIR/ai" \
    -I "$ENGINE_DIR/world" \
    -std=c11 \
    -DBANANA_ENGINE_HAS_OPENMP=1 \
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

# ── Emit engine version metadata for runtime cache-busting + diagnostics ────
if command -v sha256sum &>/dev/null; then
    HASH="$(sha256sum "$OUT_DIR/engine.wasm" | awk '{print $1}' | cut -c1-12)"
elif command -v shasum &>/dev/null; then
    HASH="$(shasum -a 256 "$OUT_DIR/engine.wasm" | awk '{print $1}' | cut -c1-12)"
else
    HASH="$(wc -c < "$OUT_DIR/engine.wasm" | tr -d ' ')-bytes"
fi
BUILD_UTC="$(date -u +"%Y%m%d-%H%M%S")"
ENGINE_VERSION="${BUILD_UTC}-${HASH}"
printf '{\n  "engineAssetVersion": "%s"\n}\n' "$ENGINE_VERSION" > "$OUT_DIR/engine.version.json"

# ── Copy to React public directory so Vite serves it ─────────────────────────
mkdir -p "$REACT_PUBLIC"
cp "$OUT_DIR/engine.js"   "$REACT_PUBLIC/engine.js"
cp "$OUT_DIR/engine.wasm" "$REACT_PUBLIC/engine.wasm"
cp "$OUT_DIR/engine.version.json" "$REACT_PUBLIC/engine.version.json"

# ── Copy to canonical artifact directory for CI / reproducible app builds ───
mkdir -p "$ARTIFACTS_DIR"
cp "$OUT_DIR/engine.js"   "$ARTIFACTS_DIR/banana-wasm-engine.js"
cp "$OUT_DIR/engine.wasm" "$ARTIFACTS_DIR/banana-wasm-engine.wasm"
cp "$OUT_DIR/engine.version.json" "$ARTIFACTS_DIR/banana-wasm-engine.version.json"

echo "[build-engine-wasm] Copied to $REACT_PUBLIC"
echo "[build-engine-wasm] Copied to $ARTIFACTS_DIR"
echo "[build-engine-wasm] Engine asset version: $ENGINE_VERSION"
echo "[build-engine-wasm] Done. Start React dev server and open /game-engine"

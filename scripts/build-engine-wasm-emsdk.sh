#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$SCRIPT_DIR/.."
BUILD_SCRIPT="$SCRIPT_DIR/build-engine-wasm.sh"
DEFAULT_EMSDK_PATH="/tmp/emsdk"
DOCKER_IMAGE="emscripten/emsdk:3.1.57"

if command -v emcc >/dev/null 2>&1; then
    echo "[build-engine-wasm-emsdk] Using active emcc from PATH"
    exec bash "$BUILD_SCRIPT" "$@"
fi

EMSDK_ROOT="${EMSDK_PATH:-$DEFAULT_EMSDK_PATH}"
if [[ -f "$EMSDK_ROOT/emsdk_env.sh" ]]; then
    echo "[build-engine-wasm-emsdk] Activating emsdk from $EMSDK_ROOT"
    # shellcheck disable=SC1091
    source "$EMSDK_ROOT/emsdk_env.sh" >/dev/null 2>&1
  export EMSDK_PATH="$EMSDK_ROOT"
  if [[ -f "$EMSDK_ROOT/upstream/emscripten/emcc.sh" ]]; then
    export EMCC_SCRIPT="$EMSDK_ROOT/upstream/emscripten/emcc.sh"
  elif [[ -f "$EMSDK_ROOT/upstream/emscripten/emcc.py" ]]; then
    export EMCC_SCRIPT="$EMSDK_ROOT/upstream/emscripten/emcc.py"
  fi
    exec bash "$BUILD_SCRIPT" "$@"
fi

if command -v docker >/dev/null 2>&1 && docker info >/dev/null 2>&1; then
    echo "[build-engine-wasm-emsdk] Using Docker image $DOCKER_IMAGE"
    exec docker run --rm \
        -v "$ROOT:/workspace" \
        -w /workspace \
        "$DOCKER_IMAGE" \
  bash scripts/build-engine-wasm.sh "$@"
fi

cat >&2 <<EOF
[build-engine-wasm-emsdk] ERROR: No usable Emscripten toolchain found.

Tried, in order:
  1. emcc on PATH
  2. EMSDK_PATH/emsdk_env.sh
  3. $DEFAULT_EMSDK_PATH/emsdk_env.sh
  4. Docker daemon with image $DOCKER_IMAGE

To build locally, either:
  - activate emsdk first, or
  - set EMSDK_PATH to your emsdk root, or
  - start Docker Desktop so the CI-style container build can run.
EOF
exit 1

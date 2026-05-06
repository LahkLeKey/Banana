#!/usr/bin/env bash
# scripts/build-wasm.sh — Build banana WASM artifacts using Emscripten.
#
# Spec 257 T003 canonical WASM build flow.
# Spec 258 T003 SIMD/scalar kernel pair strategy.
#
# Usage:
#   bash scripts/build-wasm.sh [--simd] [--out-dir <path>]
#
# Options:
#   --simd        Build SIMD variant alongside scalar (requires emcc with -msimd128 support).
#   --out-dir     Output directory (default: artifacts/wasm).
#
# Outputs (scalar):
#   <out-dir>/banana-wasm.js
#   <out-dir>/banana-wasm.wasm
#   <out-dir>/banana-wasm.manifest.json
#
# Outputs (with --simd):
#   <out-dir>/banana-wasm-simd.js
#   <out-dir>/banana-wasm-simd.wasm
#   <out-dir>/banana-wasm-simd.manifest.json
#
# Requirements:
#   - emcc (Emscripten) on PATH, or EMSDK_PATH env pointing to emsdk root.
#   - Run from workspace root (c:\Github\Banana or /c/Github/Banana).
set -euo pipefail

# ── Arguments ────────────────────────────────────────────────────────────────
BUILD_SIMD=0
OUT_DIR="artifacts/wasm"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --simd)      BUILD_SIMD=1; shift ;;
    --out-dir)   OUT_DIR="$2"; shift 2 ;;
    *) echo "[build-wasm] Unknown option: $1" >&2; exit 1 ;;
  esac
done

# ── Emscripten resolution ─────────────────────────────────────────────────────
if ! command -v emcc &>/dev/null; then
  if [[ -n "${EMSDK_PATH:-}" ]]; then
    # shellcheck disable=SC1091
    source "${EMSDK_PATH}/emsdk_env.sh" >/dev/null 2>&1
  else
    echo "[build-wasm] ERROR: emcc not found. Install Emscripten or set EMSDK_PATH." >&2
    exit 1
  fi
fi

EMCC_VERSION=$(emcc --version 2>&1 | head -1)
echo "[build-wasm] Using: ${EMCC_VERSION}"

# ── Source list (DAL-free, spec 257 T003) ────────────────────────────────────
CORE_SOURCES=(
  src/native/core/banana_status.c
  src/native/core/banana_calc.c
  src/native/core/banana_classify.c
  src/native/core/banana_ripeness.c
  src/native/core/banana_batch.c
  src/native/core/banana_truck.c
  src/native/core/domain/banana_ml_models.c
  src/native/core/domain/banana_not_banana.c
  src/native/core/domain/ml/shared/banana_ml_shared.c
  src/native/core/domain/ml/regression/banana_ml_regression.c
  src/native/core/domain/ml/binary/banana_ml_binary.c
  src/native/core/domain/ml/transformer/banana_ml_transformer.c
  src/native/wrapper/banana_wrapper.c
  src/native/wrapper/domain/ml/shared/banana_wrapper_ml_json.c
  src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.c
  src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.c
  src/native/wrapper/domain/ml/transformer/banana_wrapper_ml_transformer.c
)

# SIMD-variant replacements: simd .c files are compiled in addition to core sources;
# both scalar and simd symbols are exported so the JS dispatch layer can select at runtime.
SIMD_EXTRA_SOURCES=(
  src/native/core/domain/ml/regression/banana_ml_regression_simd.c
  src/native/core/domain/ml/binary/banana_ml_binary_simd.c
)

INCLUDE_DIRS=(
  -I src/native/core
  -I src/native/core/dal
  -I src/native/wrapper
)

# ── Exported functions ────────────────────────────────────────────────────────
# Tier 1 (spec 257): pure-compute, no heap-string output, no DB.
# SIMD variants also exported when building with --simd.
EXPORTED_SCALAR=(
  _banana_calculate_banana
  _banana_calculate_banana_with_breakdown
  _banana_predict_banana_regression_score
  _banana_classify_banana_binary
  _banana_classify_banana_binary_with_threshold
  _banana_predict_banana_ripeness
  _banana_create_banana_message
  _banana_native_version
  _banana_free
  _malloc
  _free
)

EXPORTED_SIMD_EXTRA=(
  _banana_ml_regression_predict_simd
  _banana_ml_binary_classify_simd
)

join_exports() {
  local arr=("$@")
  local joined
  joined=$(printf '"%s",' "${arr[@]}")
  echo "[${joined%,}]"
}

SCALAR_EXPORTS_STR=$(join_exports "${EXPORTED_SCALAR[@]}")
SIMD_EXPORTS_STR=$(join_exports "${EXPORTED_SCALAR[@]}" "${EXPORTED_SIMD_EXTRA[@]}")

RUNTIME_METHODS='["UTF8ToString","stringToUTF8","lengthBytesUTF8","setValue","getValue","AsciiToString"]'

# ── Common emcc flags ─────────────────────────────────────────────────────────
COMMON_FLAGS=(
  -O3
  -s MODULARIZE=1
  -s EXPORT_ES6=1
  -s NO_EXIT_RUNTIME=1
  -s ALLOW_MEMORY_GROWTH=1
  -s "INITIAL_MEMORY=8mb"
  -s "MAXIMUM_MEMORY=64mb"
  -s ASSERTIONS=0
  -s "EXPORTED_RUNTIME_METHODS=${RUNTIME_METHODS}"
  --no-entry
  -DBANANA_BUILDING_DLL=0
  -DBANANA_ENABLE_DAL=0
  -lm
  "${INCLUDE_DIRS[@]}"
)

mkdir -p "${OUT_DIR}"

# ── ABI version (read from header) ───────────────────────────────────────────
ABI_MAJOR=$(grep 'BANANA_WRAPPER_ABI_VERSION_MAJOR' src/native/wrapper/banana_wrapper.h | awk '{print $3}')
ABI_MINOR=$(grep 'BANANA_WRAPPER_ABI_VERSION_MINOR' src/native/wrapper/banana_wrapper.h | awk '{print $3}')
BUILD_DATE=$(date -u +%Y-%m-%dT%H:%M:%SZ)

write_manifest() {
  local out_wasm="$1"
  local out_manifest="$2"
  local variant="$3"
  local sha256
  sha256=$(sha256sum "${out_wasm}" | awk '{print $1}')
  cat > "${out_manifest}" <<MANIFEST
{
  "version": "${ABI_MAJOR}.${ABI_MINOR}",
  "abiMajor": ${ABI_MAJOR},
  "abiMinor": ${ABI_MINOR},
  "variant": "${variant}",
  "buildDate": "${BUILD_DATE}",
  "sha256": "${sha256}"
}
MANIFEST
  echo "[build-wasm] Manifest: ${out_manifest}"
}

# ── Scalar build ──────────────────────────────────────────────────────────────
echo "[build-wasm] Building scalar variant..."
emcc \
  "${COMMON_FLAGS[@]}" \
  -s "EXPORT_NAME=BananaWasm" \
  -s "EXPORTED_FUNCTIONS=${SCALAR_EXPORTS_STR}" \
  "${CORE_SOURCES[@]}" \
  -o "${OUT_DIR}/banana-wasm.js"

write_manifest "${OUT_DIR}/banana-wasm.wasm" "${OUT_DIR}/banana-wasm.manifest.json" "scalar"
echo "[build-wasm] Scalar build complete: ${OUT_DIR}/banana-wasm.wasm"

# ── SIMD build ────────────────────────────────────────────────────────────────
if [[ "${BUILD_SIMD}" == "1" ]]; then
  echo "[build-wasm] Building SIMD variant (-msimd128)..."
  emcc \
    "${COMMON_FLAGS[@]}" \
    -msimd128 \
    -s "EXPORT_NAME=BananaWasmSimd" \
    -s "EXPORTED_FUNCTIONS=${SIMD_EXPORTS_STR}" \
    "${CORE_SOURCES[@]}" \
    "${SIMD_EXTRA_SOURCES[@]}" \
    -o "${OUT_DIR}/banana-wasm-simd.js"

  write_manifest "${OUT_DIR}/banana-wasm-simd.wasm" "${OUT_DIR}/banana-wasm-simd.manifest.json" "simd"
  echo "[build-wasm] SIMD build complete: ${OUT_DIR}/banana-wasm-simd.wasm"
fi

echo "[build-wasm] Done. Artifacts in ${OUT_DIR}/"

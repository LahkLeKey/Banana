#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BANANA_NATIVE_NEON_BUILD_DIR:-${ROOT_DIR}/out/native-neon-integration}"
API_DIR="${ROOT_DIR}/src/typescript/api"

source "${ROOT_DIR}/scripts/lib/database-env-aliases.sh"

if ! command -v cmake >/dev/null 2>&1; then
  echo "[native-neon-integration] ERROR: cmake is required but not on PATH." >&2
  exit 1
fi

if ! command -v bun >/dev/null 2>&1; then
  echo "[native-neon-integration] ERROR: bun is required but not on PATH." >&2
  exit 1
fi

if ! banana_require_database_aliases "native-neon-integration"; then
  exit 1
fi

echo "[native-neon-integration] configuring native build with BANANA_ENABLE_POSTGRES=ON"
rm -rf "${BUILD_DIR}"
cmake -S "${ROOT_DIR}/src/native" -B "${BUILD_DIR}" -G Ninja -DBANANA_ENABLE_POSTGRES=ON -DCMAKE_BUILD_TYPE=Release

echo "[native-neon-integration] building banana_native"
cmake --build "${BUILD_DIR}" --config Release --target banana_native

if [[ -d "${BUILD_DIR}/Release" ]]; then
  export BANANA_NATIVE_PATH="${BUILD_DIR}/Release"
else
  export BANANA_NATIVE_PATH="${BUILD_DIR}"
fi

export BANANA_NEON_STRICT="true"
export BANANA_NATIVE_PGBOUNCER_ENABLED="true"

echo "[native-neon-integration] running API-native Neon integration test"
cd "${API_DIR}"
echo "[native-neon-integration] installing API dependencies"
bun install --frozen-lockfile
bun test src/services/databaseRuntime.neon.integration.test.ts

echo "[native-neon-integration] success"

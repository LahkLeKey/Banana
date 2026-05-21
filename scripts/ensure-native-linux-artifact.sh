#!/usr/bin/env sh

set -eu

WORKSPACE_DIR="${WORKSPACE_DIR:-/workspace}"
BUILD_DIR="${WORKSPACE_DIR}/out/native"
TARGET_DIR="${WORKSPACE_DIR}/out/native/bin"
TARGET_SO="${TARGET_DIR}/libbanana_native.so"

echo "[native-bootstrap] ensuring Linux native artifact at ${TARGET_SO}"

if [ -f "${TARGET_SO}" ]; then
  echo "[native-bootstrap] found existing ${TARGET_SO}"
  exit 0
fi

if [ "$(uname -s 2>/dev/null || echo unknown)" != "Linux" ]; then
  echo "[native-bootstrap] non-Linux runtime detected; skipping Linux artifact build"
  exit 0
fi

if ! command -v cmake >/dev/null 2>&1; then
  echo "[native-bootstrap] cmake missing; attempting install via apt-get"
  if command -v apt-get >/dev/null 2>&1; then
    DEBIAN_FRONTEND=noninteractive apt-get update >/dev/null 2>&1 || true
    DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      cmake build-essential pkg-config libomp-dev >/dev/null 2>&1 || true
  fi
fi

if ! command -v cmake >/dev/null 2>&1; then
  echo "[native-bootstrap] cmake still unavailable; continuing with API fallback world service"
  exit 0
fi

mkdir -p "${TARGET_DIR}"

echo "[native-bootstrap] configuring native build"
if ! cmake -S "${WORKSPACE_DIR}/src/native" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release >/tmp/banana-native-configure.log 2>&1; then
  echo "[native-bootstrap] configure failed; continuing with API fallback world service"
  exit 0
fi

echo "[native-bootstrap] building native library"
if ! cmake --build "${BUILD_DIR}" --config Release >/tmp/banana-native-build.log 2>&1; then
  echo "[native-bootstrap] build failed; continuing with API fallback world service"
  exit 0
fi

FOUND_SO="$(find "${BUILD_DIR}" -type f -name 'libbanana_native.so' | head -n 1 || true)"
if [ -z "${FOUND_SO}" ]; then
  echo "[native-bootstrap] build completed but libbanana_native.so was not found; continuing with API fallback world service"
  exit 0
fi

cp "${FOUND_SO}" "${TARGET_SO}"
echo "[native-bootstrap] native artifact ready: ${TARGET_SO}"

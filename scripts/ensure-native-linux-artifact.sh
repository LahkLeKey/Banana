#!/usr/bin/env sh

set -eu

WORKSPACE_DIR="${WORKSPACE_DIR:-/workspace}"
BUILD_DIR="${WORKSPACE_DIR}/out/native"
TARGET_DIR="${WORKSPACE_DIR}/out/native/bin"
TARGET_SO="${TARGET_DIR}/libbanana_native.so"
REQUIRED="${BANANA_NATIVE_BOOTSTRAP_REQUIRED:-0}"

fail_or_continue() {
  message="$1"
  if [ "${REQUIRED}" = "1" ]; then
    echo "[native-bootstrap] ${message}; failing because BANANA_NATIVE_BOOTSTRAP_REQUIRED=1"
    exit 1
  fi

  echo "[native-bootstrap] ${message}; continuing with API fallback world service"
}

echo "[native-bootstrap] ensuring Linux native artifact at ${TARGET_SO}"

if [ -f "${TARGET_SO}" ]; then
  echo "[native-bootstrap] found existing ${TARGET_SO}"
  exit 0
fi

if [ "$(uname -s 2>/dev/null || echo unknown)" != "Linux" ]; then
  fail_or_continue "non-Linux runtime detected; skipping Linux artifact build"
fi

if ! command -v cmake >/dev/null 2>&1; then
  if [ "${BANANA_NATIVE_BOOTSTRAP_INSTALL_TOOLS:-0}" = "1" ]; then
    echo "[native-bootstrap] cmake missing; attempting install via apt-get"
    if command -v apt-get >/dev/null 2>&1; then
      if command -v timeout >/dev/null 2>&1; then
        DEBIAN_FRONTEND=noninteractive timeout 45s apt-get update >/dev/null 2>&1 || true
        DEBIAN_FRONTEND=noninteractive timeout 120s apt-get install -y --no-install-recommends \
          cmake build-essential pkg-config libomp-dev >/dev/null 2>&1 || true
      else
        DEBIAN_FRONTEND=noninteractive apt-get update >/dev/null 2>&1 || true
        DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
          cmake build-essential pkg-config libomp-dev >/dev/null 2>&1 || true
      fi
    fi
  else
    fail_or_continue "cmake missing and tool install disabled"
  fi
fi

if ! command -v cmake >/dev/null 2>&1; then
  fail_or_continue "cmake still unavailable"
fi

mkdir -p "${TARGET_DIR}"

echo "[native-bootstrap] configuring native build"
if ! cmake -S "${WORKSPACE_DIR}/src/native" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release >/tmp/banana-native-configure.log 2>&1; then
  fail_or_continue "configure failed"
fi

echo "[native-bootstrap] building native library"
if ! cmake --build "${BUILD_DIR}" --config Release >/tmp/banana-native-build.log 2>&1; then
  fail_or_continue "build failed"
fi

FOUND_SO="$(find "${BUILD_DIR}" -type f -name 'libbanana_native.so' | head -n 1 || true)"
if [ -z "${FOUND_SO}" ]; then
  fail_or_continue "build completed but libbanana_native.so was not found"
fi

cp "${FOUND_SO}" "${TARGET_SO}"
echo "[native-bootstrap] native artifact ready: ${TARGET_SO}"

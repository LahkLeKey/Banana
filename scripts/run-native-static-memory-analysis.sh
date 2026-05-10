#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

STATIC_BUILD_DIR="build/native-static-analysis"
STRICT_BUILD_DIR="build/native-strict-optimization"
SANITIZER_BUILD_DIR="build/sanitizers"
ARTIFACT_DIR=".artifacts/native-static-analysis"

is_windows_shell=0
case "$(uname -s 2>/dev/null || echo unknown)" in
    MINGW*|MSYS*|CYGWIN*)
        is_windows_shell=1
        ;;
esac

if ! command -v scan-build >/dev/null 2>&1; then
    echo "scan-build is required for static memory analysis. Install clang-tools."
    exit 1
fi

mkdir -p "$ARTIFACT_DIR"
rm -rf "$STATIC_BUILD_DIR" "$STRICT_BUILD_DIR"

echo "[native-static] running clang static analyzer with unix.Malloc checks"
cmake -S . -B "$STATIC_BUILD_DIR" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_C_COMPILER=clang \
    -DBANANA_ENABLE_ENGINE=ON

scan-build \
    --status-bugs \
    --keep-going \
    -o "$ARTIFACT_DIR/scan-build" \
    -enable-checker core \
    -enable-checker unix \
    -enable-checker unix.Malloc \
    cmake --build "$STATIC_BUILD_DIR"

echo "[native-static] running sanitizer leak gate (ASAN + UBSAN)"
if [[ "$is_windows_shell" -eq 0 ]]; then
    cmake --preset sanitizers -DBANANA_ENABLE_ENGINE=ON
    cmake --build "$SANITIZER_BUILD_DIR"
    ASAN_OPTIONS="detect_leaks=1:abort_on_error=1" \
    UBSAN_OPTIONS="print_stacktrace=1:abort_on_error=1" \
    ctest --test-dir "$SANITIZER_BUILD_DIR" --output-on-failure --timeout 90
else
    echo "[native-static] skipping sanitizer execution on Windows shell; enforced in Linux CI gate"
fi

echo "[native-static] enforcing strict optimization compile quality"
if [[ "$is_windows_shell" -eq 0 ]]; then
    cmake -S . -B "$STRICT_BUILD_DIR" \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_C_COMPILER=clang \
        -DBANANA_ENABLE_ENGINE=ON \
        -DCMAKE_C_FLAGS="-O3 -DNDEBUG -Wall -Wextra -Wpedantic -Werror"
    cmake --build "$STRICT_BUILD_DIR"
else
    echo "[native-static] skipping strict -Werror compile on Windows shell; enforced in Linux CI gate"
fi

echo "[native-static] static memory + strict optimization gates passed"

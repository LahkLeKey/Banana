#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

BUILD_DIR="build/native-coverage"
ARTIFACT_DIR=".artifacts/native-c"

if [[ -z "${CINTEROP_PG_CONNECTION:-}" ]]; then
    echo "CINTEROP_PG_CONNECTION is required for PostgreSQL-first native coverage runs."
    exit 1
fi

rm -rf "$BUILD_DIR"

cmake -S . -B "$BUILD_DIR" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCINTEROP_ENABLE_POSTGRES=ON \
    -DCINTEROP_ENABLE_NATIVE_TESTS=ON \
    -DCINTEROP_BUILD_SHARED_LIBRARY=OFF \
    -DCINTEROP_ENABLE_COVERAGE=ON

cmake --build "$BUILD_DIR"
ctest --test-dir "$BUILD_DIR" --output-on-failure

mkdir -p "$ARTIFACT_DIR"

gcovr \
    --object-directory "$BUILD_DIR" \
    --root . \
    --filter 'src/native' \
    --exclude 'tests/native' \
    --xml-pretty \
    --output "$ARTIFACT_DIR/coverage-native.xml" \
    --fail-under-line 100

gcovr \
    --object-directory "$BUILD_DIR" \
    --root . \
    --filter 'src/native' \
    --exclude 'tests/native' \
    --txt > "$ARTIFACT_DIR/Summary.txt"

echo "Native C coverage summary:"
cat "$ARTIFACT_DIR/Summary.txt"

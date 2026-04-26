#!/usr/bin/env bash
# Spec 014 — native C tests + lcov.
set -euo pipefail
cd "$(dirname "$0")/.."

cmake -S . -B build/native -DBANANA_BUILD_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build/native --config Debug
ctest --test-dir build/native --output-on-failure

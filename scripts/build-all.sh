#!/usr/bin/env bash
# Spec 005 — top-level build orchestrator. Native -> TypeScript API.
set -euo pipefail
cd "$(dirname "$0")/.."

cmake -S . -B build/native -DCMAKE_BUILD_TYPE=Release
cmake --build build/native --config Release

cd src/typescript/api
bun install --frozen-lockfile
bun run build

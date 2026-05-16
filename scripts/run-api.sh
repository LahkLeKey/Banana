#!/usr/bin/env bash
# Local TypeScript API run with explicit BANANA_NATIVE_PATH.
set -euo pipefail
cd "$(dirname "$0")/.."

: "${BANANA_NATIVE_PATH:=$PWD/out/native/bin}"
export BANANA_NATIVE_PATH

cd src/typescript/api
exec bun run src/index.ts

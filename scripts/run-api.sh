#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

export CINTEROP_NATIVE_PATH="${CINTEROP_NATIVE_PATH:-$ROOT_DIR/build/native/bin/Release}"

dotnet run --project "$ROOT_DIR/src/api/CInteropSharp.Api.csproj"

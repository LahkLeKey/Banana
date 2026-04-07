#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$ROOT_DIR/build/native"

cmake -S "$ROOT_DIR" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" --config Release

dotnet restore "$ROOT_DIR/src/api/CInteropSharp.Api.csproj"
dotnet build "$ROOT_DIR/src/api/CInteropSharp.Api.csproj" -c Release --no-restore

dotnet test "$ROOT_DIR/tests/unit/CInteropSharp.UnitTests.csproj" -c Release
dotnet test "$ROOT_DIR/tests/integration/CInteropSharp.IntegrationTests.csproj" -c Release

echo "Build and test completed."

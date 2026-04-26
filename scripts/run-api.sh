#!/usr/bin/env bash
# Spec 007 — local ASP.NET run with explicit BANANA_NATIVE_PATH.
set -euo pipefail
cd "$(dirname "$0")/.."

: "${BANANA_NATIVE_PATH:=$PWD/build/native}"
export BANANA_NATIVE_PATH

exec dotnet run --project src/c-sharp/asp.net/Banana.Api.csproj

#!/usr/bin/env bash
# Spec 014 — dotnet test + Cobertura coverage merge.
set -euo pipefail
cd "$(dirname "$0")/.."

OUT="${BANANA_TEST_OUTPUT_DIR:-.artifacts/coverage}"
mkdir -p "$OUT"

dotnet test Banana.sln \
  --collect:"XPlat Code Coverage" \
  --results-directory "$OUT" \
  -p:CollectCoverage=true -p:CoverletOutputFormat=cobertura

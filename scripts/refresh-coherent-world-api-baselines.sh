#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
API_DIR="$ROOT_DIR/src/typescript/api"
ARTIFACT_DIR="$ROOT_DIR/artifacts/typescript/api"

mkdir -p "$ARTIFACT_DIR"

cd "$API_DIR"

echo "[031-api-refresh] ensuring API dependencies"
bun install --frozen-lockfile

echo "[031-api-refresh] writing unit baseline -> $ARTIFACT_DIR/coherent-world-units.log"
: > "$ARTIFACT_DIR/coherent-world-units.log"

unit_tests=(
  "src/lib/pii.test.ts"
  "src/middleware/auth.test.ts"
  "src/middleware/requestContext.test.ts"
  "src/routes/auth.test.ts"
)

# game-session route test can leave a long-lived handle under this shell lane;
# keep it out of automated baseline refresh to avoid hanging evidence jobs.

for test_file in "${unit_tests[@]}"; do
  echo "[031-api-refresh] unit -> $test_file" | tee -a "$ARTIFACT_DIR/coherent-world-units.log"
  bun test "$test_file" 2>&1 | tee -a "$ARTIFACT_DIR/coherent-world-units.log"
done

echo "[031-api-refresh] writing integration baseline -> $ARTIFACT_DIR/coherent-world-integration.log"
bun test src/integration 2>&1 \
  | tee "$ARTIFACT_DIR/coherent-world-integration.log"

echo "[031-api-refresh] complete"

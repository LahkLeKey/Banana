#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
NATIVE_BUILD_DIR="${ROOT_DIR}/out/v3-native"
STEAM_BINARY_PATH="${1:-}"
source "${ROOT_DIR}/scripts/lib/database-env-aliases.sh"

sync_database_aliases() {
  if ! banana_sync_database_aliases; then
    echo "[steam-uat] DB aliases not set (continuing with in-memory/non-DB flows where applicable)"
    return
  fi

  echo "[steam-uat] DB aliases synced from authoritative source"
}

echo "[steam-uat] root: ${ROOT_DIR}"
sync_database_aliases

run_step() {
  local label="$1"
  shift
  echo "[steam-uat] ${label}"
  "$@"
}

run_step "Native gameplay tests" \
  ctest --test-dir "${NATIVE_BUILD_DIR}" -C Debug -R "player_builds|runtime_player_builds_integration" --output-on-failure

run_step "API gameplay route tests" \
  bash -lc "cd '${ROOT_DIR}/src/typescript/api' && bun test src/routes/game-session.test.ts"

run_step "React gameplay tests" \
  bash -lc "cd '${ROOT_DIR}/src/typescript/react' && bun test src/lib/api.test.ts src/components/game/OverworldHud.test.tsx"

run_step "API health smoke" \
  bash -lc "curl -sS -o /tmp/banana_api_health.json -w '%{http_code}' http://localhost:8081/health && echo && cat /tmp/banana_api_health.json"

if [[ -n "${STEAM_BINARY_PATH}" ]]; then
  echo "[steam-uat] Checking Steam binary path: ${STEAM_BINARY_PATH}"
  if [[ ! -f "${STEAM_BINARY_PATH}" ]]; then
    echo "[steam-uat] ERROR: Steam binary not found at ${STEAM_BINARY_PATH}" >&2
    exit 1
  fi
  echo "[steam-uat] Steam binary exists."
else
  echo "[steam-uat] NOTE: No Steam binary path provided."
  echo "[steam-uat] Usage: scripts/steam-uat-readiness.sh <path-to-BananaEngineer.exe>"
fi

cat <<'EOF'
[steam-uat] Manual visual checklist:
  1. Launch the game from Steam library entry.
  2. Join from /session-room and verify transition to /game-engine equivalent flow.
  3. Validate Build Lab: class select, allocation apply, gear equip, combo trigger.
  4. Capture screenshot/video evidence and attach logs.
EOF

echo "[steam-uat] Completed automated readiness lanes."

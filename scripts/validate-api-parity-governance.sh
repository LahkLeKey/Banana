#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
API_DIR="$ROOT_DIR/src/typescript/api"
INDEX_FILE="$API_DIR/src/index.ts"
GAMEPLAY_ROUTE="$API_DIR/src/routes/v1/gameplay.ts"
PLAYER_ROUTE="$API_DIR/src/routes/v1/player.ts"

STRICT=false
INVENTORY_ONLY=false

while [[ $# -gt 0 ]]; do
  case "$1" in
    --strict)
      STRICT=true
      shift
      ;;
    --inventory-only)
      INVENTORY_ONLY=true
      shift
      ;;
    *)
      echo "Unknown option: $1" >&2
      exit 2
      ;;
  esac
done

if [[ ! -f "$INDEX_FILE" ]]; then
  echo "Missing API index file: $INDEX_FILE" >&2
  exit 1
fi

echo "[parity] route ownership inventory"
echo "- /v1/gameplay -> gameplay-session-orchestration"
echo "- /v1/player/account -> player-identity-account"
echo "- /v1/player/progression -> progression-inventory"
echo "- /v1/player/inventory -> progression-inventory"
echo "- /v1/player/insights -> player-insights-web"

if [[ "$INVENTORY_ONLY" == true ]]; then
  exit 0
fi

MISSING=0

if ! grep -q "registerV1GameplayRoutes" "$INDEX_FILE"; then
  echo "[parity] missing registerV1GameplayRoutes in src/index.ts" >&2
  MISSING=1
fi

if ! grep -q "registerV1PlayerRoutes" "$INDEX_FILE"; then
  echo "[parity] missing registerV1PlayerRoutes in src/index.ts" >&2
  MISSING=1
fi

if [[ ! -f "$GAMEPLAY_ROUTE" ]]; then
  echo "[parity] missing gameplay route module: $GAMEPLAY_ROUTE" >&2
  MISSING=1
fi

if [[ ! -f "$PLAYER_ROUTE" ]]; then
  echo "[parity] missing player route module: $PLAYER_ROUTE" >&2
  MISSING=1
fi

if [[ "$MISSING" -eq 1 && "$STRICT" == true ]]; then
  exit 1
fi

echo "[parity] validation complete"

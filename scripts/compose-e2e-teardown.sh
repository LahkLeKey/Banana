#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

if [[ "${BANANA_E2E_PRESERVE_RUNTIME:-false}" == "true" ]]; then
  echo "BANANA_E2E_PRESERVE_RUNTIME=true; skipping compose teardown."
  exit 0
fi

PROFILE="${BANANA_E2E_COMPOSE_PROFILE:-apps}"
docker compose --profile "$PROFILE" down -v --remove-orphans

echo "Tore down E2E runtime profile '$PROFILE'."

#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

PROFILE="${BANANA_E2E_COMPOSE_PROFILE:-apps}"
SERVICES="${BANANA_E2E_BOOTSTRAP_SERVICES:-api}"
HEALTH_URL="${BANANA_E2E_HEALTH_URL:-http://localhost:8080/health}"
TIMEOUT_SEC="${BANANA_E2E_HEALTH_TIMEOUT_SEC:-180}"

read -r -a service_array <<< "$SERVICES"
if [[ ${#service_array[@]} -eq 0 ]]; then
  echo "BANANA_E2E_BOOTSTRAP_SERVICES must define at least one service"
  exit 1
fi

docker compose --profile "$PROFILE" up --build -d "${service_array[@]}"

if printf '%s\n' "${service_array[@]}" | grep -qx "api"; then
  deadline=$((SECONDS + TIMEOUT_SEC))
  until curl -fsS "$HEALTH_URL" >/dev/null 2>&1; do
    if (( SECONDS >= deadline )); then
      echo "::error::API health check timed out after ${TIMEOUT_SEC}s ($HEALTH_URL)."
      docker compose --profile "$PROFILE" ps || true
      exit 1
    fi
    sleep 2
  done
fi

mkdir -p .artifacts/e2e
echo "{\"profile\":\"$PROFILE\",\"services\":\"$SERVICES\",\"health_url\":\"$HEALTH_URL\"}" > .artifacts/e2e/bootstrap.json

echo "Bootstrapped E2E runtime profile '$PROFILE' for services: $SERVICES"

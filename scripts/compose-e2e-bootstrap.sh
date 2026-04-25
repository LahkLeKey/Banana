#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "$ROOT_DIR"

export COMPOSE_PROJECT_NAME="${COMPOSE_PROJECT_NAME:-banana-container}"

COMPOSE_PROFILES="${BANANA_E2E_COMPOSE_PROFILES:-apps}"
COMPOSE_SERVICES=()
COMPOSE_ARGS=()

for profile in $COMPOSE_PROFILES; do
    COMPOSE_ARGS+=(--profile "$profile")
done

COMPOSE_ARGS+=(up --build -d)

if [[ -n "${BANANA_E2E_BOOTSTRAP_SERVICES:-}" ]]; then
    read -r -a COMPOSE_SERVICES <<<"$BANANA_E2E_BOOTSTRAP_SERVICES"
    COMPOSE_ARGS+=("${COMPOSE_SERVICES[@]}")
fi

docker compose "${COMPOSE_ARGS[@]}"

if ! command -v curl >/dev/null 2>&1; then
    echo "curl is required for E2E bootstrap health checks." >&2
    exit 127
fi

HEALTH_URL="${BANANA_E2E_HEALTH_URL:-http://localhost:8080/health}"
HEALTH_TIMEOUT_SEC="${BANANA_E2E_HEALTH_TIMEOUT_SEC:-120}"
DEADLINE=$((SECONDS + HEALTH_TIMEOUT_SEC))

until curl -fsS "$HEALTH_URL" >/dev/null 2>&1; do
    if (( SECONDS >= DEADLINE )); then
        echo "Timed out waiting for E2E runtime health endpoint: $HEALTH_URL" >&2
        docker compose --profile apps ps || true
        exit 70
    fi

    sleep 2
done

echo "E2E runtime bootstrap complete. Health endpoint is reachable: $HEALTH_URL"

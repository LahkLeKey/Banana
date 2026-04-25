#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "$ROOT_DIR"

export COMPOSE_PROJECT_NAME="${COMPOSE_PROJECT_NAME:-banana-container}"

COMPOSE_PROFILES="${BANANA_E2E_COMPOSE_PROFILES:-apps}"
COMPOSE_ARGS=()

for profile in $COMPOSE_PROFILES; do
    COMPOSE_ARGS+=(--profile "$profile")
done

COMPOSE_ARGS+=(down --remove-orphans)

if [[ "${BANANA_E2E_TEARDOWN_VOLUMES:-0}" == "1" ]]; then
    COMPOSE_ARGS+=(--volumes)
fi

docker compose "${COMPOSE_ARGS[@]}"

echo "E2E runtime teardown complete for compose project: $COMPOSE_PROJECT_NAME"

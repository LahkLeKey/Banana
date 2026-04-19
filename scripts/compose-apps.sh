#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "$ROOT_DIR"
export COMPOSE_PROJECT_NAME="${COMPOSE_PROJECT_NAME:-banana-container}"
COMPOSE_ARGS=(--profile apps up --build)
COMPOSE_SERVICES=()

if [[ -n "${BANANA_COMPOSE_APPS_SERVICES:-}" ]]; then
	read -r -a COMPOSE_SERVICES <<<"$BANANA_COMPOSE_APPS_SERVICES"
fi

if [[ "${DETACH:-0}" == "1" ]]; then
	COMPOSE_ARGS+=(-d)
fi

if [[ "${#COMPOSE_SERVICES[@]}" -gt 0 ]]; then
	COMPOSE_ARGS+=("${COMPOSE_SERVICES[@]}")
fi

docker compose "${COMPOSE_ARGS[@]}"
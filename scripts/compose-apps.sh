#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "$ROOT_DIR"
export COMPOSE_PROJECT_NAME="${COMPOSE_PROJECT_NAME:-banana-container}"

if [[ "${DETACH:-0}" == "1" ]]; then
	docker compose --profile apps up --build -d
else
	docker compose --profile apps up --build
fi
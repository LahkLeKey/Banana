#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "$ROOT_DIR"
export COMPOSE_PROJECT_NAME="${COMPOSE_PROJECT_NAME:-banana-container}"
docker compose --profile runtime up --build api

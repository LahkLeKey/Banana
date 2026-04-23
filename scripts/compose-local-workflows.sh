#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

export COMPOSE_PROJECT_NAME="${COMPOSE_PROJECT_NAME:-banana-container}"
SERVICE="${1:-workflow-train-not-banana-local}"

cleanup() {
	docker compose --profile workflow-local down --remove-orphans >/dev/null 2>&1 || true
}

trap cleanup EXIT

docker compose --profile workflow-local up --build --abort-on-container-exit --exit-code-from "$SERVICE" "$SERVICE"

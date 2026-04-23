#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

export COMPOSE_PROJECT_NAME="${COMPOSE_PROJECT_NAME:-banana-container}"
SERVICE_INPUT="${1:-train}"

case "$SERVICE_INPUT" in
	train)
		SERVICE="workflow-train-not-banana-local"
		;;
	triaged)
		SERVICE="workflow-orchestrate-triaged-local"
		;;
	sdlc)
		SERVICE="workflow-orchestrate-sdlc-local"
		;;
	workflow-train-not-banana-local|workflow-orchestrate-triaged-local|workflow-orchestrate-sdlc-local)
		SERVICE="$SERVICE_INPUT"
		;;
	*)
		echo "usage: scripts/compose-local-workflows.sh [train|triaged|sdlc|workflow-train-not-banana-local|workflow-orchestrate-triaged-local|workflow-orchestrate-sdlc-local]" >&2
		exit 64
		;;
esac

cleanup() {
	docker compose --profile workflow-local down --remove-orphans >/dev/null 2>&1 || true
}

trap cleanup EXIT

docker compose --profile workflow-local up --build --abort-on-container-exit --exit-code-from "$SERVICE" "$SERVICE"

#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
COMPOSE_CMD=(docker compose --profile keycloak-local)

ensure_docker() {
  if ! command -v docker >/dev/null 2>&1; then
    echo "[keycloak-local] ERROR: docker is not installed or not on PATH." >&2
    exit 1
  fi

  if ! docker info >/dev/null 2>&1; then
    echo "[keycloak-local] ERROR: Docker engine is not reachable. Start Docker Desktop and retry." >&2
    exit 1
  fi
}

up() {
  ensure_docker
  (
    cd "${ROOT_DIR}"
    "${COMPOSE_CMD[@]}" up -d keycloak-local
    bash scripts/check-keycloak-local.sh
  )
  echo "[keycloak-local] Dashboard: http://localhost:8080/admin"
}

down() {
  ensure_docker
  (
    cd "${ROOT_DIR}"
    "${COMPOSE_CMD[@]}" down
  )
}

logs() {
  ensure_docker
  (
    cd "${ROOT_DIR}"
    "${COMPOSE_CMD[@]}" logs -f keycloak-local keycloak-postgres
  )
}

check() {
  (
    cd "${ROOT_DIR}"
    bash scripts/check-keycloak-local.sh
  )
}

bootstrap() {
  (
    cd "${ROOT_DIR}"
    bash scripts/keycloak-realm-bootstrap.sh bootstrap
  )
}

usage() {
  cat <<'EOF'
Usage: bash scripts/keycloak-local.sh <command>

Commands:
  up      Start local Keycloak stack and wait for readiness
  down    Stop local Keycloak stack
  logs    Tail local Keycloak and Postgres logs
  check   Run local issuer readiness checks
  bootstrap  Reconcile realm/client/github provider automation
EOF
}

command="${1:-}"
case "${command}" in
  up) up ;;
  down) down ;;
  logs) logs ;;
  check) check ;;
  bootstrap) bootstrap ;;
  *)
    usage
    exit 1
    ;;
esac

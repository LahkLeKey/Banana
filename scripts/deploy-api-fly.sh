#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
API_DIR="${ROOT_DIR}/src/typescript/api"
APP_NAME="${FLY_APP_NAME:-banana-api}"
SKIP_PARITY_GATE="${BANANA_FLY_SKIP_PARITY_GATE:-false}"
DEPLOY_ENV="${BANANA_DEPLOY_ENV:-prod}"

source "${ROOT_DIR}/scripts/lib/database-env-aliases.sh"

if ! command -v fly >/dev/null 2>&1; then
  echo "[fly-deploy] ERROR: fly CLI is required but not installed or not on PATH." >&2
  exit 1
fi

cd "${API_DIR}"

if [[ "${SKIP_PARITY_GATE}" != "true" ]]; then
  if ! command -v bun >/dev/null 2>&1; then
    echo "[fly-deploy] ERROR: bun is required for parity validation. Set BANANA_FLY_SKIP_PARITY_GATE=true to bypass." >&2
    exit 1
  fi

  echo "[fly-deploy] running strict API parity governance gate"
  bash "${ROOT_DIR}/scripts/validate-api-parity-governance.sh" --strict
fi

if ! grep -q "path = \"/health\"" fly.toml; then
  echo "[fly-deploy] ERROR: fly.toml must include /health checks before deploy." >&2
  exit 1
fi

if ! grep -q "internal_port = 8081" fly.toml; then
  echo "[fly-deploy] ERROR: fly.toml must expose internal_port = 8081." >&2
  exit 1
fi

if [[ "${DEPLOY_ENV}" == "prod" ]]; then
  if ! grep -q 'BANANA_KEYCLOAK_ISSUER_URL = "https://banana-keycloak-prod.fly.dev/realms/banana"' fly.toml; then
    echo "[fly-deploy] ERROR: prod deploy requires BANANA_KEYCLOAK_ISSUER_URL to point at banana-keycloak-prod issuer." >&2
    exit 1
  fi

  if ! grep -q 'BANANA_KEYCLOAK_TOKEN_ISSUER_URL = "https://banana-keycloak-prod.fly.dev/realms/banana"' fly.toml; then
    echo "[fly-deploy] ERROR: prod deploy requires BANANA_KEYCLOAK_TOKEN_ISSUER_URL to point at banana-keycloak-prod issuer." >&2
    exit 1
  fi

  if grep -q 'BANANA_KEYCLOAK_ISSUER_URL = "http://localhost' fly.toml; then
    echo "[fly-deploy] ERROR: prod deploy cannot use localhost Keycloak issuer URLs." >&2
    exit 1
  fi

  if grep -q 'BANANA_KEYCLOAK_TOKEN_ISSUER_URL = "http://localhost' fly.toml; then
    echo "[fly-deploy] ERROR: prod deploy cannot use localhost Keycloak token issuer URLs." >&2
    exit 1
  fi
fi

DATABASE_URL_VALUE=""

if banana_sync_database_aliases; then
  DATABASE_URL_VALUE="${NEON_DATABASE_URL}"

  echo "[fly-deploy] applying self-hosted Neon database secrets"
  fly secrets set -a "${APP_NAME}" \
    NEON_DATABASE_URL="${DATABASE_URL_VALUE}" \
    DATABASE_URL="${DATABASE_URL_VALUE}" \
    BANANA_PG_CONNECTION="${DATABASE_URL_VALUE}" >/dev/null
else
  echo "[fly-deploy] WARN: local database aliases are unset; skipping secret sync and using existing Fly secrets."
fi

cd "${ROOT_DIR}"

echo "[fly-deploy] deploying ${APP_NAME} from ${ROOT_DIR}"
fly deploy . -a "${APP_NAME}" -c "${API_DIR}/fly.toml" --dockerfile "${API_DIR}/Dockerfile"

echo "[fly-deploy] readiness contract complete"
echo "[fly-deploy] rollback hint: fly releases -a ${APP_NAME} && fly releases rollback <version> -a ${APP_NAME}"
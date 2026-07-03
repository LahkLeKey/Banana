#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
KEYCLOAK_DIR="${ROOT_DIR}/src/typescript/keycloak"
KEYCLOAK_ENV="${BANANA_KEYCLOAK_ENV:-prod}"
APP_NAME_DEFAULT="banana-keycloak-${KEYCLOAK_ENV}"
APP_NAME="${FLY_APP_NAME:-${APP_NAME_DEFAULT}}"
PRIMARY_REGION="${BANANA_KEYCLOAK_REGION:-iad}"
KEYCLOAK_ADMIN="${KEYCLOAK_ADMIN:-zephrym}"
KEYCLOAK_ADMIN_PASSWORD="${KEYCLOAK_ADMIN_PASSWORD:-4xcruvfg989}"

if ! command -v fly >/dev/null 2>&1; then
  echo "[keycloak-fly] ERROR: fly CLI is required but not installed or not on PATH." >&2
  exit 1
fi

case "${KEYCLOAK_ENV}" in
  dev|staging|prod)
    ;;
  *)
    echo "[keycloak-fly] ERROR: BANANA_KEYCLOAK_ENV must be one of dev|staging|prod." >&2
    exit 1
    ;;
esac

required_vars=(
  BANANA_KEYCLOAK_DB_URL
  BANANA_KEYCLOAK_DB_USERNAME
  BANANA_KEYCLOAK_DB_PASSWORD
)

for var_name in "${required_vars[@]}"; do
  if [[ -z "${!var_name:-}" ]]; then
    echo "[keycloak-fly] ERROR: ${var_name} is required." >&2
    exit 1
  fi
done

if [[ "${BANANA_KEYCLOAK_DB_URL}" != jdbc:postgresql://* ]]; then
  echo "[keycloak-fly] ERROR: BANANA_KEYCLOAK_DB_URL must start with jdbc:postgresql://" >&2
  exit 1
fi

if [[ "${BANANA_KEYCLOAK_DB_URL}" == *"<"* || "${BANANA_KEYCLOAK_DB_URL}" == *">"* ]]; then
  echo "[keycloak-fly] ERROR: BANANA_KEYCLOAK_DB_URL appears to contain placeholder tokens." >&2
  exit 1
fi

cd "${ROOT_DIR}"

echo "[keycloak-fly] ensuring app ${APP_NAME} exists"
if ! fly apps show "${APP_NAME}" >/dev/null 2>&1; then
  fly apps create "${APP_NAME}" >/dev/null
fi

echo "[keycloak-fly] syncing runtime secrets"
fly secrets set -a "${APP_NAME}" \
  KEYCLOAK_ADMIN="${KEYCLOAK_ADMIN}" \
  KEYCLOAK_ADMIN_PASSWORD="${KEYCLOAK_ADMIN_PASSWORD}" \
  KC_DB_URL="${BANANA_KEYCLOAK_DB_URL}" \
  KC_DB_USERNAME="${BANANA_KEYCLOAK_DB_USERNAME}" \
  KC_DB_PASSWORD="${BANANA_KEYCLOAK_DB_PASSWORD}" >/dev/null

echo "[keycloak-fly] deploying ${APP_NAME} in ${PRIMARY_REGION}"
fly deploy . \
  -a "${APP_NAME}" \
  -c "${KEYCLOAK_DIR}/fly.toml" \
  --dockerfile "${ROOT_DIR}/docker/keycloak.Dockerfile" \
  --primary-region "${PRIMARY_REGION}"

echo "[keycloak-fly] deployment complete"
echo "[keycloak-fly] issuer hint: https://${APP_NAME}.fly.dev/realms/banana"
echo "[keycloak-fly] jwks hint: https://${APP_NAME}.fly.dev/realms/banana/protocol/openid-connect/certs"

if [[ -n "${BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID:-}" && -n "${BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET:-}" ]]; then
  echo "[keycloak-fly] syncing github idp in realm banana"
  BANANA_KEYCLOAK_ENV="${KEYCLOAK_ENV}" \
    FLY_APP_NAME="${APP_NAME}" \
    KEYCLOAK_ADMIN="${KEYCLOAK_ADMIN}" \
    KEYCLOAK_ADMIN_PASSWORD="${KEYCLOAK_ADMIN_PASSWORD}" \
    BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID="${BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID}" \
    BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET="${BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET}" \
    bash "${ROOT_DIR}/scripts/keycloak-fly-github-idp.sh"
else
  echo "[keycloak-fly] github idp sync skipped (missing BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID/SECRET)"
fi

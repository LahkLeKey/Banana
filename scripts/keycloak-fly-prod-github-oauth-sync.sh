#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP_NAME="${FLY_APP_NAME:-banana-keycloak-prod}"
BASE_URL="${BANANA_KEYCLOAK_BASE_URL:-https://kc-idp.banana.engineer}"
KEYCLOAK_ENV="${BANANA_KEYCLOAK_ENV:-prod}"
KEYCLOAK_ADMIN="${KEYCLOAK_ADMIN:-zephrym}"
KEYCLOAK_ADMIN_PASSWORD="${KEYCLOAK_ADMIN_PASSWORD:-4xcruvfg989}"

CLIENT_ID=""
CLIENT_SECRET=""

print_usage() {
  cat <<'EOF'
Usage: bash scripts/keycloak-fly-prod-github-oauth-sync.sh --client-id <id> --client-secret <secret> [options]

Required:
  --client-id <value>       GitHub OAuth app client ID
  --client-secret <value>   GitHub OAuth app client secret

Optional:
  --app-name <value>        Fly app name (default: banana-keycloak-prod)
  --base-url <value>        Keycloak base URL (default: https://kc-idp.banana.engineer)
  --admin-user <value>      Keycloak admin username (default: zephrym)
  --admin-password <value>  Keycloak admin password
  --help                    Show this help message

What this script does:
  1) Stores GitHub OAuth credentials as Fly secrets on the Keycloak app
  2) Syncs the GitHub IdP in the banana realm using those credentials
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --client-id)
      CLIENT_ID="${2:-}"
      shift 2
      ;;
    --client-secret)
      CLIENT_SECRET="${2:-}"
      shift 2
      ;;
    --app-name)
      APP_NAME="${2:-}"
      shift 2
      ;;
    --base-url)
      BASE_URL="${2:-}"
      shift 2
      ;;
    --admin-user)
      KEYCLOAK_ADMIN="${2:-}"
      shift 2
      ;;
    --admin-password)
      KEYCLOAK_ADMIN_PASSWORD="${2:-}"
      shift 2
      ;;
    --help|-h)
      print_usage
      exit 0
      ;;
    *)
      echo "[keycloak-prod-github-sync] ERROR: unknown argument: $1" >&2
      print_usage
      exit 1
      ;;
  esac
done

if [[ -z "${CLIENT_ID}" || -z "${CLIENT_SECRET}" ]]; then
  echo "[keycloak-prod-github-sync] ERROR: both --client-id and --client-secret are required." >&2
  print_usage
  exit 1
fi

if ! command -v fly >/dev/null 2>&1; then
  echo "[keycloak-prod-github-sync] ERROR: fly CLI not found in PATH." >&2
  exit 1
fi

echo "[keycloak-prod-github-sync] setting Fly secrets on ${APP_NAME}"
fly secrets set -a "${APP_NAME}" \
  BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID="${CLIENT_ID}" \
  BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET="${CLIENT_SECRET}" >/dev/null

echo "[keycloak-prod-github-sync] syncing Keycloak github idp at ${BASE_URL}"
BANANA_KEYCLOAK_ENV="${KEYCLOAK_ENV}" \
FLY_APP_NAME="${APP_NAME}" \
BANANA_KEYCLOAK_BASE_URL="${BASE_URL}" \
KEYCLOAK_ADMIN="${KEYCLOAK_ADMIN}" \
KEYCLOAK_ADMIN_PASSWORD="${KEYCLOAK_ADMIN_PASSWORD}" \
BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID="${CLIENT_ID}" \
BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET="${CLIENT_SECRET}" \
bash "${ROOT_DIR}/scripts/keycloak-fly-github-idp.sh"

echo "[keycloak-prod-github-sync] done"
echo "[keycloak-prod-github-sync] GitHub OAuth callback must be: ${BASE_URL}/realms/banana/broker/github/endpoint"

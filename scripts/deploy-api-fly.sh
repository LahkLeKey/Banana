#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
API_DIR="${ROOT_DIR}/src/typescript/api"
APP_NAME="${FLY_APP_NAME:-banana-api}"
DATABASE_URL_VALUE="${NEON_DATABASE_URL:-}"

if ! command -v fly >/dev/null 2>&1; then
  echo "[fly-deploy] ERROR: fly CLI is required but not installed or not on PATH." >&2
  exit 1
fi

cd "${API_DIR}"

if [[ -z "${DATABASE_URL_VALUE}" ]]; then
  echo "[fly-deploy] ERROR: NEON_DATABASE_URL is required for the database cutover." >&2
  exit 1
fi

echo "[fly-deploy] applying self-hosted Neon database secrets"
fly secrets set -a "${APP_NAME}" \
  NEON_DATABASE_URL="${DATABASE_URL_VALUE}" \
  DATABASE_URL="${DATABASE_URL_VALUE}" \
  BANANA_PG_CONNECTION="${DATABASE_URL_VALUE}" >/dev/null

echo "[fly-deploy] deploying ${APP_NAME} from ${API_DIR}"
fly deploy -a "${APP_NAME}" --config fly.toml
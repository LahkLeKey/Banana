#!/usr/bin/env bash

set -euo pipefail

APP_NAME="${1:-${FLY_APP_NAME:-}}"
REALM="${2:-banana}"
MAX_ATTEMPTS="${BANANA_KEYCLOAK_CHECK_ATTEMPTS:-12}"
SLEEP_SECONDS="${BANANA_KEYCLOAK_CHECK_SLEEP_SECONDS:-5}"

if [[ -z "${APP_NAME}" ]]; then
  echo "[keycloak-check] usage: bash scripts/check-keycloak-issuer.sh <fly-app-name> [realm]" >&2
  exit 1
fi

if [[ "${APP_NAME}" == http://* || "${APP_NAME}" == https://* ||
      "${APP_NAME}" == *"/"* || "${APP_NAME}" == *"."* ]]; then
  echo "[keycloak-check] ERROR: pass Fly app name only (for example banana-keycloak-dev), not a URL or hostname." >&2
  exit 1
fi

if ! command -v curl >/dev/null 2>&1; then
  echo "[keycloak-check] ERROR: curl is required." >&2
  exit 1
fi

issuer="https://${APP_NAME}.fly.dev/realms/${REALM}"
well_known="${issuer}/.well-known/openid-configuration"
jwks="${issuer}/protocol/openid-connect/certs"

for url in "${issuer}" "${well_known}" "${jwks}"; do
  attempt=1
  while :; do
    status_code="$(curl -sS -o /dev/null -w "%{http_code}" "${url}" || echo "000")"
    if [[ "${status_code}" -ge 200 && "${status_code}" -lt 400 ]]; then
      echo "[keycloak-check] OK   ${url} (status ${status_code})"
      break
    fi

    if [[ "${attempt}" -ge "${MAX_ATTEMPTS}" ]]; then
      echo "[keycloak-check] FAIL ${url} after ${attempt} attempts (last status ${status_code})" >&2
      exit 1
    fi

    echo "[keycloak-check] WAIT ${url} attempt ${attempt}/${MAX_ATTEMPTS} (status ${status_code})"
    attempt=$((attempt + 1))
    sleep "${SLEEP_SECONDS}"
  done
done

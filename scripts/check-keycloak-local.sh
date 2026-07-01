#!/usr/bin/env bash

set -euo pipefail

BASE_URL="${1:-http://localhost:8080}"
REALM="${2:-banana}"
MAX_ATTEMPTS="${BANANA_KEYCLOAK_CHECK_ATTEMPTS:-24}"
SLEEP_SECONDS="${BANANA_KEYCLOAK_CHECK_SLEEP_SECONDS:-5}"

if ! command -v curl >/dev/null 2>&1; then
  echo "[keycloak-local-check] ERROR: curl is required." >&2
  exit 1
fi

issuer="${BASE_URL%/}/realms/${REALM}"
well_known="${issuer}/.well-known/openid-configuration"
jwks="${issuer}/protocol/openid-connect/certs"

for url in "${issuer}" "${well_known}" "${jwks}"; do
  attempt=1
  while :; do
    status_code="$(curl -sS -o /dev/null -w "%{http_code}" "${url}" || echo "000")"
    if [[ "${status_code}" -ge 200 && "${status_code}" -lt 400 ]]; then
      echo "[keycloak-local-check] OK   ${url} (status ${status_code})"
      break
    fi

    if [[ "${attempt}" -ge "${MAX_ATTEMPTS}" ]]; then
      echo "[keycloak-local-check] FAIL ${url} after ${attempt} attempts (last status ${status_code})" >&2
      exit 1
    fi

    echo "[keycloak-local-check] WAIT ${url} attempt ${attempt}/${MAX_ATTEMPTS} (status ${status_code})"
    attempt=$((attempt + 1))
    sleep "${SLEEP_SECONDS}"
  done
done

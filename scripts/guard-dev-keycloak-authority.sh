#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

EXPECTED_DEV_ISSUER="https://banana-keycloak-dev.fly.dev/realms/banana"
FORBIDDEN_PROD_HOST='kc-idp.banana.engineer'
DEV_SURFACE_PATTERNS='BANANA_KEYCLOAK_(ISSUER_URL|TOKEN_ISSUER_URL):\s*https://kc-idp\.banana\.engineer'

matches="$(grep -nE "${DEV_SURFACE_PATTERNS}" docker-compose.yml 2>/dev/null || true)"
if [[ -n "${matches}" ]]; then
  echo "[keycloak-dev-guard] ERROR: production auth authority host found in docker-compose dev runtime config:" >&2
  echo "${matches}" >&2
  echo "[keycloak-dev-guard] Expected dev authority host in docker-compose: banana-keycloak-dev.fly.dev" >&2
  exit 1
fi

if ! grep -q "${EXPECTED_DEV_ISSUER}" docker-compose.yml; then
  echo "[keycloak-dev-guard] ERROR: docker-compose.yml must point API issuer to ${EXPECTED_DEV_ISSUER}" >&2
  exit 1
fi

if grep -nE "const KEYCLOAK_DEFAULT_ISSUER\s*=\s*'https://${FORBIDDEN_PROD_HOST}/realms/banana'" src/typescript/api/src/routes/auth.ts >/dev/null 2>&1; then
  echo "[keycloak-dev-guard] ERROR: auth route default issuer must not point at production authority host." >&2
  exit 1
fi

if ! grep -q "${EXPECTED_DEV_ISSUER}" src/typescript/api/src/routes/auth.ts; then
  echo "[keycloak-dev-guard] ERROR: auth route default issuer must be ${EXPECTED_DEV_ISSUER}" >&2
  exit 1
fi

echo "[keycloak-dev-guard] OK: dev authority mapping surfaces are aligned"

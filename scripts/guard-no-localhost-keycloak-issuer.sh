#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

# Guard active runtime/config/docs surfaces from reintroducing localhost issuer defaults.
scan_paths=(
  docker-compose.yml
  src/typescript/api
  docs/keycloak
  .github/workflows
)

matches="$(grep -RInE \
  'BANANA_KEYCLOAK_(ISSUER_URL|TOKEN_ISSUER_URL)\s*[:=]\s*"?http://localhost|http://localhost:8080/realms/banana|keycloak-local' \
  "${scan_paths[@]}" \
  --exclude='guard-no-localhost-keycloak-issuer.sh' \
  --exclude-dir='.git' \
  2>/dev/null || true)"

if [[ -n "${matches}" ]]; then
  echo "[keycloak-guard] ERROR: localhost Keycloak issuer defaults or keycloak-local prototype references detected:" >&2
  echo "${matches}" >&2
  echo "[keycloak-guard] Expected dev auth issuer: https://banana-keycloak-dev.fly.dev/realms/banana" >&2
  exit 1
fi

echo "[keycloak-guard] OK: no localhost Keycloak issuer defaults detected"

#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ENV_FILE="${ROOT_DIR}/.env.keycloak.local"
TEMPLATE_FILE="${ROOT_DIR}/docs/keycloak/env.keycloak.local.example"

GITHUB_CLIENT_ID="${BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID:-}"
GITHUB_CLIENT_SECRET="${BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET:-}"
DO_BOOTSTRAP=false

usage() {
  cat <<'EOF'
Usage: bash scripts/setup-github-idp-manual.sh [options]

Options:
  --client-id <value>       Set BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID
  --client-secret <value>   Set BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET
  --bootstrap               Run keycloak realm bootstrap after writing env file
  --help                    Show help

What this script does:
  - Creates .env.keycloak.local from docs/keycloak/env.keycloak.local.example when missing
  - Writes GitHub IdP client id/secret to .env.keycloak.local when provided
  - Prints direct manual setup links for the GitHub OAuth App and Keycloak admin
EOF
}

set_env_value() {
  local key="$1"
  local value="$2"

  if grep -qE "^${key}=" "${ENV_FILE}"; then
    sed -i "s|^${key}=.*|${key}=${value}|" "${ENV_FILE}"
  else
    printf '%s=%s\n' "${key}" "${value}" >>"${ENV_FILE}"
  fi
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --client-id)
      GITHUB_CLIENT_ID="${2:-}"
      shift 2
      ;;
    --client-secret)
      GITHUB_CLIENT_SECRET="${2:-}"
      shift 2
      ;;
    --bootstrap)
      DO_BOOTSTRAP=true
      shift
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage
      exit 1
      ;;
  esac
done

if [[ ! -f "${ENV_FILE}" ]]; then
  cp "${TEMPLATE_FILE}" "${ENV_FILE}"
  echo "[github-idp-setup] created ${ENV_FILE} from template"
fi

if [[ -n "${GITHUB_CLIENT_ID}" ]]; then
  set_env_value "BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID" "${GITHUB_CLIENT_ID}"
  echo "[github-idp-setup] set BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID"
fi

if [[ -n "${GITHUB_CLIENT_SECRET}" ]]; then
  set_env_value "BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET" "${GITHUB_CLIENT_SECRET}"
  echo "[github-idp-setup] set BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET"
fi

cat <<'EOF'

Manual links:
- GitHub OAuth Apps: https://github.com/settings/developers
- New OAuth App: https://github.com/settings/developers
- Keycloak Admin (realm banana): http://localhost:8080/admin/master/console/#/banana/identity-providers

Use these values in GitHub OAuth App:
- Application name: Banana Local Keycloak
- Homepage URL: http://localhost:5173
- Authorization callback URL: http://localhost:8080/realms/banana/broker/github/endpoint

After saving the GitHub OAuth app credentials, run:
- bash scripts/keycloak-realm-bootstrap.sh bootstrap
- bash scripts/keycloak-realm-bootstrap.sh status

EOF

if [[ "${DO_BOOTSTRAP}" == true ]]; then
  echo "[github-idp-setup] running bootstrap..."
  (
    cd "${ROOT_DIR}"
    bash scripts/keycloak-realm-bootstrap.sh bootstrap
  )
fi

#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
COMPOSE_CMD=(docker compose --profile keycloak-local)
REALM="${BANANA_KEYCLOAK_REALM:-banana}"
ADMIN_USER="${KEYCLOAK_ADMIN:-zephrym}"
ADMIN_PASSWORD="${KEYCLOAK_ADMIN_PASSWORD:-}"
ENV_FILE="${BANANA_KEYCLOAK_ENV_FILE:-${ROOT_DIR}/.env.keycloak.local}"

REDIRECT_URIS='["https://banana.engineer/*","https://www.banana.engineer/*","https://*.vercel.app/*","http://localhost:5173/*","http://localhost:8081/auth/keycloak/callback*"]'
WEB_ORIGINS='["https://banana.engineer","https://www.banana.engineer","https://*.vercel.app","http://localhost:5173"]'

ensure_docker() {
  if ! command -v docker >/dev/null 2>&1; then
    echo "[keycloak-bootstrap] ERROR: docker is required." >&2
    exit 1
  fi

  if ! docker info >/dev/null 2>&1; then
    echo "[keycloak-bootstrap] ERROR: Docker engine is not reachable." >&2
    exit 1
  fi
}

load_env_file() {
  if [[ -f "${ENV_FILE}" ]]; then
    # shellcheck disable=SC1090
    set -a && source "${ENV_FILE}" && set +a
    echo "[keycloak-bootstrap] loaded env file ${ENV_FILE}"
  else
    echo "[keycloak-bootstrap] env file not found at ${ENV_FILE}; using current shell env"
  fi
}

ensure_admin_password() {
  if [[ -z "${ADMIN_PASSWORD}" ]]; then
    echo "[keycloak-bootstrap] ERROR: KEYCLOAK_ADMIN_PASSWORD is required (export it or set it in ${ENV_FILE})." >&2
    exit 1
  fi
}

compose_up() {
  (
    cd "${ROOT_DIR}"
    "${COMPOSE_CMD[@]}" up -d keycloak-local
  )
  (
    cd "${ROOT_DIR}"
    bash scripts/check-keycloak-local.sh
  )
}

compose_reset() {
  (
    cd "${ROOT_DIR}"
    "${COMPOSE_CMD[@]}" down -v
  )
}

kcadm() {
  (
    cd "${ROOT_DIR}"
    MSYS_NO_PATHCONV=1 "${COMPOSE_CMD[@]}" exec -T keycloak-local /opt/keycloak/bin/kcadm.sh "$@"
  )
}

kcadm_login() {
  kcadm config credentials --server http://localhost:8080 --realm master --user "${ADMIN_USER}" --password "${ADMIN_PASSWORD}" >/dev/null
}

get_client_id() {
  local client_name="$1"
  kcadm get clients -r "${REALM}" -q clientId="${client_name}" --fields id --format csv |
      tail -n 1 | tr -d '\r' | sed 's/"//g'
}

ensure_react_client_contract() {
  local client_uuid
  client_uuid="$(get_client_id "banana-react-spa")"
  if [[ -z "${client_uuid}" ]]; then
    echo "[keycloak-bootstrap] ERROR: client banana-react-spa not found in realm ${REALM}." >&2
    exit 1
  fi

  kcadm update "clients/${client_uuid}" -r "${REALM}" \
      -s "redirectUris=${REDIRECT_URIS}" \
      -s "webOrigins=${WEB_ORIGINS}" \
      -s 'attributes."pkce.code.challenge.method"='

  echo "[keycloak-bootstrap] ensured client banana-react-spa redirect/web-origin contract"
}

upsert_github_idp() {
  local client_id="${BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID:-}"
  local client_secret="${BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET:-}"

  if [[ -z "${client_id}" || -z "${client_secret}" ]]; then
    echo "[keycloak-bootstrap] github idp skipped (missing BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID/SECRET)"
    return
  fi

  if kcadm get "identity-provider/instances/github" -r "${REALM}" >/dev/null 2>&1; then
    kcadm update "identity-provider/instances/github" -r "${REALM}" \
      -s enabled=true \
      -s trustEmail=true \
      -s updateProfileFirstLoginMode=on \
      -s authenticateByDefault=false \
      -s linkOnly=false \
      -s firstBrokerLoginFlowAlias='first broker login' \
      -s "config.clientId=${client_id}" \
      -s "config.clientSecret=${client_secret}" \
      -s "config.defaultScope=read:user user:email" \
      -s "config.useJwksUrl=true"
  else
    kcadm create identity-provider/instances -r "${REALM}" \
      -s alias=github \
      -s providerId=github \
      -s enabled=true \
      -s trustEmail=true \
      -s updateProfileFirstLoginMode=on \
      -s authenticateByDefault=false \
      -s linkOnly=false \
      -s firstBrokerLoginFlowAlias='first broker login' \
      -s "config.clientId=${client_id}" \
      -s "config.clientSecret=${client_secret}" \
      -s "config.defaultScope=read:user user:email" \
      -s "config.useJwksUrl=true"
  fi

  echo "[keycloak-bootstrap] ensured github idp"
}

upsert_google_idp() {
  local client_id="${BANANA_KEYCLOAK_IDP_GOOGLE_CLIENT_ID:-}"
  local client_secret="${BANANA_KEYCLOAK_IDP_GOOGLE_CLIENT_SECRET:-}"

  if [[ -z "${client_id}" || -z "${client_secret}" ]]; then
    echo "[keycloak-bootstrap] google idp skipped (missing BANANA_KEYCLOAK_IDP_GOOGLE_CLIENT_ID/SECRET)"
    return
  fi

  if kcadm get "identity-provider/instances/google" -r "${REALM}" >/dev/null 2>&1; then
    kcadm update "identity-provider/instances/google" -r "${REALM}" \
      -s enabled=true \
      -s trustEmail=true \
      -s updateProfileFirstLoginMode=on \
      -s authenticateByDefault=false \
      -s linkOnly=false \
      -s firstBrokerLoginFlowAlias='first broker login' \
      -s "config.clientId=${client_id}" \
      -s "config.clientSecret=${client_secret}" \
      -s 'config.defaultScope=openid profile email' \
      -s 'config.useJwksUrl=true'
  else
    kcadm create identity-provider/instances -r "${REALM}" \
      -s alias=google \
      -s providerId=google \
      -s enabled=true \
      -s trustEmail=true \
      -s updateProfileFirstLoginMode=on \
      -s authenticateByDefault=false \
      -s linkOnly=false \
      -s firstBrokerLoginFlowAlias='first broker login' \
      -s "config.clientId=${client_id}" \
      -s "config.clientSecret=${client_secret}" \
      -s 'config.defaultScope=openid profile email' \
      -s 'config.useJwksUrl=true'
  fi

  echo "[keycloak-bootstrap] ensured google idp"
}

upsert_linkedin_idp() {
  local client_id="${BANANA_KEYCLOAK_IDP_LINKEDIN_CLIENT_ID:-}"
  local client_secret="${BANANA_KEYCLOAK_IDP_LINKEDIN_CLIENT_SECRET:-}"
  local auth_url="${BANANA_KEYCLOAK_IDP_LINKEDIN_AUTH_URL:-https://www.linkedin.com/oauth/v2/authorization}"
  local token_url="${BANANA_KEYCLOAK_IDP_LINKEDIN_TOKEN_URL:-https://www.linkedin.com/oauth/v2/accessToken}"
  local userinfo_url="${BANANA_KEYCLOAK_IDP_LINKEDIN_USERINFO_URL:-https://api.linkedin.com/v2/userinfo}"
  local jwks_url="${BANANA_KEYCLOAK_IDP_LINKEDIN_JWKS_URL:-https://www.linkedin.com/oauth/openid/jwks}"
  local issuer="${BANANA_KEYCLOAK_IDP_LINKEDIN_ISSUER:-https://www.linkedin.com}"

  if [[ -z "${client_id}" || -z "${client_secret}" ]]; then
    echo "[keycloak-bootstrap] linkedin idp skipped (missing BANANA_KEYCLOAK_IDP_LINKEDIN_CLIENT_ID/SECRET)"
    return
  fi

  if kcadm get "identity-provider/instances/linkedin" -r "${REALM}" >/dev/null 2>&1; then
    kcadm update "identity-provider/instances/linkedin" -r "${REALM}" \
      -s enabled=true \
      -s trustEmail=true \
      -s updateProfileFirstLoginMode=on \
      -s authenticateByDefault=false \
      -s linkOnly=false \
      -s firstBrokerLoginFlowAlias='first broker login' \
      -s "config.clientId=${client_id}" \
      -s "config.clientSecret=${client_secret}" \
      -s 'config.defaultScope=openid profile email' \
      -s "config.authorizationUrl=${auth_url}" \
      -s "config.tokenUrl=${token_url}" \
      -s "config.userInfoUrl=${userinfo_url}" \
      -s "config.jwksUrl=${jwks_url}" \
      -s "config.issuer=${issuer}" \
      -s 'config.useJwksUrl=true' \
      -s 'config.validateSignature=true'
  else
    kcadm create identity-provider/instances -r "${REALM}" \
      -s alias=linkedin \
      -s providerId=oidc \
      -s enabled=true \
      -s trustEmail=true \
      -s updateProfileFirstLoginMode=on \
      -s authenticateByDefault=false \
      -s linkOnly=false \
      -s firstBrokerLoginFlowAlias='first broker login' \
      -s "config.clientId=${client_id}" \
      -s "config.clientSecret=${client_secret}" \
      -s 'config.defaultScope=openid profile email' \
      -s "config.authorizationUrl=${auth_url}" \
      -s "config.tokenUrl=${token_url}" \
      -s "config.userInfoUrl=${userinfo_url}" \
      -s "config.jwksUrl=${jwks_url}" \
      -s "config.issuer=${issuer}" \
      -s 'config.useJwksUrl=true' \
      -s 'config.validateSignature=true'
  fi

  echo "[keycloak-bootstrap] ensured linkedin idp"
}

print_status() {
  echo "[keycloak-bootstrap] identity providers in realm ${REALM}:"
  kcadm get identity-provider/instances -r "${REALM}" || true
}

bootstrap() {
  ensure_docker
  load_env_file
  ensure_admin_password
  compose_up
  kcadm_login
  ensure_react_client_contract
  upsert_github_idp
  upsert_google_idp
  upsert_linkedin_idp
  print_status
  echo "[keycloak-bootstrap] done"
}

reset_and_bootstrap() {
  ensure_docker
  load_env_file
  compose_reset
  compose_up
  kcadm_login
  ensure_react_client_contract
  upsert_github_idp
  upsert_google_idp
  upsert_linkedin_idp
  print_status
  echo "[keycloak-bootstrap] reset + bootstrap done"
}

usage() {
  cat <<'EOF'
Usage: bash scripts/keycloak-realm-bootstrap.sh <command>

Commands:
  bootstrap        Start stack if needed and reconcile realm/client/github idp
  reset-bootstrap  Recreate local keycloak volumes then reconcile realm/client/github idp
  status           Show current identity provider instances for banana realm

Notes:
  - Optional env file: .env.keycloak.local (override with BANANA_KEYCLOAK_ENV_FILE)
  - Required per provider:
    GitHub: BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID and _SECRET
    Google: BANANA_KEYCLOAK_IDP_GOOGLE_CLIENT_ID and _SECRET
    LinkedIn: BANANA_KEYCLOAK_IDP_LINKEDIN_CLIENT_ID and _SECRET
EOF
}

command="${1:-}"
case "${command}" in
  bootstrap)
    bootstrap
    ;;
  reset-bootstrap)
    reset_and_bootstrap
    ;;
  status)
    ensure_docker
    load_env_file
    compose_up
    kcadm_login
    print_status
    ;;
  *)
    usage
    exit 1
    ;;
esac

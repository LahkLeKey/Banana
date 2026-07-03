#!/usr/bin/env bash

set -euo pipefail

KEYCLOAK_ENV="${BANANA_KEYCLOAK_ENV:-prod}"
REALM="${BANANA_KEYCLOAK_REALM:-banana}"
APP_NAME_DEFAULT="banana-keycloak-${KEYCLOAK_ENV}"
APP_NAME="${FLY_APP_NAME:-${APP_NAME_DEFAULT}}"
BASE_URL="${BANANA_KEYCLOAK_BASE_URL:-https://${APP_NAME}.fly.dev}"
KEYCLOAK_ADMIN="${KEYCLOAK_ADMIN:-zephrym}"
KEYCLOAK_ADMIN_PASSWORD="${KEYCLOAK_ADMIN_PASSWORD:-4xcruvfg989}"

required_vars=(
  BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID
  BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET
)

for var_name in "${required_vars[@]}"; do
  if [[ -z "${!var_name:-}" ]]; then
    echo "[keycloak-fly-github] ERROR: ${var_name} is required." >&2
    exit 1
  fi
done

token_response="$(curl -fsS \
  -X POST "${BASE_URL}/realms/master/protocol/openid-connect/token" \
  -H 'content-type: application/x-www-form-urlencoded' \
  --data-urlencode 'client_id=admin-cli' \
  --data-urlencode 'grant_type=password' \
  --data-urlencode "username=${KEYCLOAK_ADMIN}" \
  --data-urlencode "password=${KEYCLOAK_ADMIN_PASSWORD}")"

access_token="$(printf '%s' "${token_response}" | sed -n 's/.*"access_token":"\([^"]*\)".*/\1/p')"

if [[ -z "${access_token}" ]]; then
  echo "[keycloak-fly-github] ERROR: could not obtain access token from ${BASE_URL}." >&2
  exit 1
fi

idp_payload="$(cat <<EOF
{
  "alias": "github",
  "providerId": "github",
  "enabled": true,
  "updateProfileFirstLoginMode": "on",
  "trustEmail": true,
  "storeToken": false,
  "addReadTokenRoleOnCreate": false,
  "authenticateByDefault": false,
  "linkOnly": false,
  "firstBrokerLoginFlowAlias": "first broker login",
  "config": {
    "clientId": "${BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_ID}",
    "clientSecret": "${BANANA_KEYCLOAK_IDP_GITHUB_CLIENT_SECRET}",
    "defaultScope": "read:user user:email",
    "useJwksUrl": "true"
  }
}
EOF
)"

idp_status="$(curl -sS -o /dev/null -w '%{http_code}' \
  -H "authorization: Bearer ${access_token}" \
  "${BASE_URL}/admin/realms/${REALM}/identity-provider/instances/github")"

case "${idp_status}" in
  200)
    curl -fsS \
      -X PUT "${BASE_URL}/admin/realms/${REALM}/identity-provider/instances/github" \
      -H "authorization: Bearer ${access_token}" \
      -H 'content-type: application/json' \
      -d "${idp_payload}" >/dev/null
    echo "[keycloak-fly-github] updated github idp on ${APP_NAME} (${REALM})"
    ;;
  404)
    curl -fsS \
      -X POST "${BASE_URL}/admin/realms/${REALM}/identity-provider/instances" \
      -H "authorization: Bearer ${access_token}" \
      -H 'content-type: application/json' \
      -d "${idp_payload}" >/dev/null
    echo "[keycloak-fly-github] created github idp on ${APP_NAME} (${REALM})"
    ;;
  *)
    echo "[keycloak-fly-github] ERROR: unexpected status ${idp_status} while reading github idp." >&2
    exit 1
    ;;
esac

echo "[keycloak-fly-github] callback URI to set in GitHub OAuth App: ${BASE_URL}/realms/${REALM}/broker/github/endpoint"
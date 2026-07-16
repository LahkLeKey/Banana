#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${ROOT_DIR}"

EXPECTED_KEYCLOAK_HOST="${BANANA_EXPECTED_DEV_KEYCLOAK_HOST:-banana-keycloak-dev.fly.dev}"
PROBE_HOSTS_CSV="${BANANA_AUTH_REDIRECT_SMOKE_HOSTS:-https://api.banana.engineer,https://banana-api.fly.dev}"
RETURN_TO_URL="${BANANA_AUTH_REDIRECT_RETURN_TO:-https://banana.engineer/login}"
ERROR_MARKERS_REGEX='Invalid parameter: redirect_uri|invalid_redirect_uri|error=invalid_request|keycloak_auth_not_completed'

if ! command -v curl >/dev/null 2>&1; then
  echo "[auth-redirect-check] ERROR: curl is required." >&2
  exit 1
fi

IFS=',' read -r -a probe_hosts <<< "${PROBE_HOSTS_CSV}"
if [[ "${#probe_hosts[@]}" -eq 0 ]]; then
  echo "[auth-redirect-check] ERROR: provide at least one host in BANANA_AUTH_REDIRECT_SMOKE_HOSTS" >&2
  exit 1
fi

extract_location_header() {
  local header_file="$1"
  awk 'tolower($1) == "location:" {print $2}' "${header_file}" | tr -d '\r' | tail -n 1
}

extract_host() {
  local url="$1"
  echo "${url}" | sed -E 's#^[a-zA-Z]+://([^/]+).*#\1#' | tr '[:upper:]' '[:lower:]'
}

for base_url_raw in "${probe_hosts[@]}"; do
  base_url="$(echo "${base_url_raw}" | xargs)"
  if [[ -z "${base_url}" ]]; then
    continue
  fi

  start_url="${base_url%/}/auth/keycloak/start"
  start_headers="$(mktemp)"
  start_body="$(mktemp)"

  start_status="$(curl -sS -G \
    --data-urlencode "provider=github" \
    --data-urlencode "returnTo=${RETURN_TO_URL}" \
    -D "${start_headers}" \
    -o "${start_body}" \
    -w "%{http_code}" \
    "${start_url}")"
  start_location="$(extract_location_header "${start_headers}")"

  if [[ "${start_status}" -lt 300 || "${start_status}" -ge 400 ]]; then
    echo "[auth-redirect-check] ERROR: expected 3xx from ${start_url}, got ${start_status}" >&2
    cat "${start_body}" >&2 || true
    rm -f "${start_headers}" "${start_body}"
    exit 1
  fi

  if [[ -z "${start_location}" ]]; then
    echo "[auth-redirect-check] ERROR: missing redirect location for ${start_url}" >&2
    rm -f "${start_headers}" "${start_body}"
    exit 1
  fi

  keycloak_host="$(extract_host "${start_location}")"
  if [[ "${keycloak_host}" != "${EXPECTED_KEYCLOAK_HOST}" ]]; then
    echo "[auth-redirect-check] ERROR: ${start_url} redirected to ${keycloak_host}; expected ${EXPECTED_KEYCLOAK_HOST}" >&2
    rm -f "${start_headers}" "${start_body}"
    exit 1
  fi

  if [[ "${start_location}" != *"kc_idp_hint=github"* ]]; then
    echo "[auth-redirect-check] ERROR: missing kc_idp_hint=github in redirect for ${start_url}" >&2
    rm -f "${start_headers}" "${start_body}"
    exit 1
  fi

  keycloak_headers="$(mktemp)"
  keycloak_body="$(mktemp)"
  keycloak_status="$(curl -sS -D "${keycloak_headers}" -o "${keycloak_body}" -w "%{http_code}" "${start_location}")"
  keycloak_location="$(extract_location_header "${keycloak_headers}")"

  if grep -Eiq "${ERROR_MARKERS_REGEX}" "${keycloak_body}"; then
    echo "[auth-redirect-check] ERROR: Keycloak error marker detected for ${base_url}" >&2
    cat "${keycloak_body}" >&2 || true
    rm -f "${start_headers}" "${start_body}" "${keycloak_headers}" "${keycloak_body}"
    exit 1
  fi

  if [[ "${keycloak_status}" -ge 300 && "${keycloak_status}" -lt 400 ]]; then
    if [[ "${keycloak_location}" != *"github.com"* &&
          "${keycloak_location}" != *"/broker/github/login"* ]]; then
      echo "[auth-redirect-check] ERROR: expected broker redirect to GitHub handoff, got ${keycloak_location}" >&2
      rm -f "${start_headers}" "${start_body}" "${keycloak_headers}" "${keycloak_body}"
      exit 1
    fi
  fi

  echo "[auth-redirect-check] OK: ${base_url} -> ${EXPECTED_KEYCLOAK_HOST} -> GitHub"

  rm -f "${start_headers}" "${start_body}" "${keycloak_headers}" "${keycloak_body}"
done
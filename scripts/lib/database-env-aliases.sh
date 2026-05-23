#!/usr/bin/env bash

# Shared database env alias helpers for Banana automation paths.

banana_resolve_database_url() {
  echo "${NEON_DATABASE_URL:-${DATABASE_URL:-${BANANA_PG_CONNECTION:-}}}"
}

banana_sync_database_aliases() {
  local db_url
  db_url="$(banana_resolve_database_url)"

  if [[ -z "${db_url}" ]]; then
    return 1
  fi

  export NEON_DATABASE_URL="${db_url}"
  export DATABASE_URL="${db_url}"
  export BANANA_PG_CONNECTION="${db_url}"
  return 0
}

banana_require_database_aliases() {
  local context="${1:-database}"

  if banana_sync_database_aliases; then
    return 0
  fi

  echo "[${context}] ERROR: set NEON_DATABASE_URL (or DATABASE_URL / BANANA_PG_CONNECTION)." >&2
  return 1
}

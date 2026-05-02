#!/usr/bin/env bash
# Bootstrap script for Vault integration in Banana.
# Sets up a dev Vault instance with Banana secrets.
# Usage: bash scripts/vault-init.sh [--dev]
set -euo pipefail

DEV_MODE=false
while [[ $# -gt 0 ]]; do
  case "$1" in
    --dev) DEV_MODE=true; shift ;;
    *) echo "Unknown argument: $1" >&2; exit 1 ;;
  esac
done

VAULT_ADDR="${VAULT_ADDR:-http://127.0.0.1:8200}"
VAULT_TOKEN="${VAULT_TOKEN:-banana-dev-token}"

export VAULT_ADDR VAULT_TOKEN

echo "Waiting for Vault at $VAULT_ADDR..."
for i in $(seq 1 20); do
  if vault status -address="$VAULT_ADDR" >/dev/null 2>&1; then
    echo "Vault is ready."
    break
  fi
  if [[ "$i" -eq 20 ]]; then
    echo "ERROR: Vault did not become ready in time." >&2
    exit 1
  fi
  sleep 2
done

# Enable KV v2 secrets engine if not already enabled
vault secrets enable -address="$VAULT_ADDR" -version=2 kv 2>/dev/null || true

echo "Writing Banana secrets to Vault..."

if [[ -z "${BANANA_PG_CONNECTION:-}" ]]; then
  echo "WARNING: BANANA_PG_CONNECTION is not set; writing empty value." >&2
fi
vault kv put -address="$VAULT_ADDR" secret/banana/postgres \
  connection="${BANANA_PG_CONNECTION:-}"

if [[ -z "${BANANA_JWT_SECRET:-}" ]]; then
  echo "WARNING: BANANA_JWT_SECRET is not set; writing empty value." >&2
fi
vault kv put -address="$VAULT_ADDR" secret/banana/jwt \
  secret="${BANANA_JWT_SECRET:-}"

echo "Vault bootstrap complete."
echo "  secret/banana/postgres  -> connection"
echo "  secret/banana/jwt       -> secret"

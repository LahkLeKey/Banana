#!/usr/bin/env bash
# Spec 047 -- API parity governance validator (US1 inventory + gap detection).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

inventory_only="false"
strict_mode="false"

usage() {
  cat <<'EOF'
Usage: scripts/validate-api-parity-governance.sh [--inventory-only] [--strict]

Options:
  --inventory-only  Build and validate overlap inventory, skip gap report gating.
  --strict          Exit non-zero when missing-route findings exist.
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --inventory-only)
      inventory_only="true"
      shift
      ;;
    --strict)
      strict_mode="true"
      shift
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "[banana] unknown argument: $1" >&2
      usage
      exit 2
      ;;
  esac
done

inventory_file=".specify/specs/047-api-parity-governance/artifacts/overlapping-routes.inventory.json"
exceptions_file=".specify/specs/047-api-parity-governance/artifacts/parity-exceptions.json"
drift_file=".specify/specs/047-api-parity-governance/artifacts/parity-drift-report.json"

echo "[banana] building overlap inventory"
node scripts/api-parity/build-overlap-inventory.mjs --repo-root "$ROOT_DIR" --out-file "$inventory_file"

echo "[banana] asserting overlap inventory coverage"
node scripts/api-parity/assert-inventory-coverage.mjs --repo-root "$ROOT_DIR" --inventory-file "$inventory_file"

echo "[banana] validating exception ledger"
node -e "import('./scripts/api-parity/lib/validate-parity-exceptions.mjs').then(m=>{const r=m.validateParityExceptions('$exceptions_file');if(!r.ok){for(const err of r.errors){console.error('[banana] exception error: '+err);}process.exit(1);}console.log('[banana] parity exceptions valid ('+r.exceptions.length+')');})"

if [[ "$inventory_only" == "true" ]]; then
  echo "[banana] inventory-only mode complete"
  exit 0
fi

echo "[banana] detecting missing-route gaps"
if [[ "$strict_mode" == "true" ]]; then
  node scripts/api-parity/detect-missing-route-gaps.mjs --repo-root "$ROOT_DIR" --out-file "$drift_file" --strict
else
  node scripts/api-parity/detect-missing-route-gaps.mjs --repo-root "$ROOT_DIR" --out-file "$drift_file"
fi

echo "[banana] api parity governance validation complete"

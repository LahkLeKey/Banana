#!/usr/bin/env bash
# Spec 043 - frontend runtime config drift validator.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

require_running="false"

usage() {
  cat <<'EOF'
Usage: scripts/validate-frontend-config-drift.sh [--require-running]

Checks:
  1) Canonical env keys exist in docker-compose.yml
  2) Canonical frontend launch wiring exists in .vscode/launch.json and .vscode/tasks.json
  3) Interpolated compose config contains expected frontend API base keys
  4) If --require-running is set, verifies running service env values
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --require-running)
      require_running="true"
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

failures=0

check_contains() {
  local file="$1"
  local pattern="$2"
  local label="$3"
  if grep -Fq "$pattern" "$file"; then
    echo "[pass] $label"
  else
    echo "[fail] $label" >&2
    failures=$((failures + 1))
  fi
}

echo "[banana] validating static contract surfaces"
check_contains "docker-compose.yml" 'VITE_BANANA_API_BASE_URL: ${BANANA_REACT_API_URL:-http://localhost:8080}' 'react compose env mapping'
check_contains "docker-compose.yml" 'BANANA_API_BASE_URL: ${BANANA_ELECTRON_API_URL:-http://localhost:8080}' 'electron compose env mapping'
check_contains "docker-compose.yml" 'EXPO_PUBLIC_BANANA_API_BASE_URL: ${BANANA_MOBILE_API_URL:-http://localhost:8080}' 'react-native-web compose env mapping'
check_contains ".vscode/launch.json" '"preLaunchTask": "Compose: frontend react up + ready"' 'react launch preLaunch task'
check_contains ".vscode/launch.json" '"postDebugTask": "Compose: frontend react down"' 'react launch postDebug task'
check_contains ".vscode/tasks.json" '"label": "Compose: frontend react up + ready"' 'react frontend task label'

echo "[banana] validating interpolated compose config"
config_tmp="$(mktemp)"
docker compose --profile apps config > "$config_tmp"
check_contains "$config_tmp" 'VITE_BANANA_API_BASE_URL: http://localhost:8080' 'interpolated react api base default'
check_contains "$config_tmp" 'BANANA_API_BASE_URL: http://localhost:8080' 'interpolated electron api base default'
check_contains "$config_tmp" 'EXPO_PUBLIC_BANANA_API_BASE_URL: http://localhost:8080' 'interpolated mobile api base default'
rm -f "$config_tmp"

if [[ "$require_running" == "true" ]]; then
  echo "[banana] validating running service environment"
  if docker compose ps react-app 2>/dev/null | tail -n +2 | grep -qE '(Up|running|healthy)'; then
    react_env="$(docker compose exec -T react-app printenv VITE_BANANA_API_BASE_URL || true)"
    if [[ "$react_env" == "http://localhost:8080" ]]; then
      echo "[pass] react-app runtime VITE_BANANA_API_BASE_URL"
    else
      echo "[fail] react-app runtime VITE_BANANA_API_BASE_URL expected http://localhost:8080 got '$react_env'" >&2
      failures=$((failures + 1))
    fi
  else
    echo "[fail] react-app is not running; launch canonical profile first" >&2
    failures=$((failures + 1))
  fi

  echo "[banana] rendered-ui check: open http://localhost:5173 and confirm API base is not <unset>"
fi

if [[ "$failures" -gt 0 ]]; then
  echo "[banana] frontend config drift validation failed ($failures issue(s))" >&2
  echo "[banana] remediation: run 'Compose: frontend react down' then 'Compose: frontend react up + ready' and re-run this script." >&2
  exit 1
fi

echo "[banana] frontend config drift validation passed"

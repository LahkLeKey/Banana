#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

PROFILE="${BANANA_E2E_COMPOSE_PROFILE:-apps}"
SERVICES="${BANANA_E2E_BOOTSTRAP_SERVICES:-api}"
HEALTH_URL="${BANANA_E2E_HEALTH_URL:-http://localhost:8080/health}"
TIMEOUT_SEC="${BANANA_E2E_HEALTH_TIMEOUT_SEC:-180}"
ARTIFACT_DIR=".artifacts/e2e"
DIAGNOSTICS_DIR="$ARTIFACT_DIR/diagnostics"

mkdir -p "$ARTIFACT_DIR" "$DIAGNOSTICS_DIR"

capture_bootstrap_state() {
  docker compose --profile "$PROFILE" ps > "$DIAGNOSTICS_DIR/compose-ps-bootstrap.txt" 2>&1 || true
  docker compose --profile "$PROFILE" logs --no-color > "$DIAGNOSTICS_DIR/compose-bootstrap.log" 2>&1 || true
}

read -r -a service_array <<< "$SERVICES"
if [[ ${#service_array[@]} -eq 0 ]]; then
  echo "BANANA_E2E_BOOTSTRAP_SERVICES must define at least one service"
  exit 1
fi

if ! docker compose --profile "$PROFILE" up --build -d "${service_array[@]}"; then
  capture_bootstrap_state
  cat > "$ARTIFACT_DIR/bootstrap-status.json" <<'EOF'
{"status":"fail","stage":"compose-up","reason_code":"compose_up_failed","exit_code":1}
EOF
  echo "::error::Failed to bootstrap compose profile '$PROFILE'."
  exit 1
fi

if printf '%s\n' "${service_array[@]}" | grep -qx "api"; then
  deadline=$((SECONDS + TIMEOUT_SEC))
  until curl -fsS "$HEALTH_URL" >/dev/null 2>&1; do
    if (( SECONDS >= deadline )); then
      echo "::error::API health check timed out after ${TIMEOUT_SEC}s ($HEALTH_URL)."
      capture_bootstrap_state
      cat > "$ARTIFACT_DIR/bootstrap-status.json" <<'EOF'
{"status":"fail","stage":"health-check","reason_code":"health_timeout","exit_code":1}
EOF
      exit 1
    fi
    sleep 2
  done
fi

capture_bootstrap_state
echo "{\"profile\":\"$PROFILE\",\"services\":\"$SERVICES\",\"health_url\":\"$HEALTH_URL\"}" > "$ARTIFACT_DIR/bootstrap.json"
cat > "$ARTIFACT_DIR/bootstrap-status.json" <<'EOF'
{"status":"pass","stage":"health-check","reason_code":"success","exit_code":0}
EOF

echo "Bootstrapped E2E runtime profile '$PROFILE' for services: $SERVICES"

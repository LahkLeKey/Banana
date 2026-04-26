#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")/.."

ARTIFACT_DIR=".artifacts/e2e"
DIAGNOSTICS_DIR="$ARTIFACT_DIR/diagnostics"
mkdir -p "$ARTIFACT_DIR" "$DIAGNOSTICS_DIR"

if [[ "${BANANA_E2E_PRESERVE_RUNTIME:-false}" == "true" ]]; then
  echo "BANANA_E2E_PRESERVE_RUNTIME=true; skipping compose teardown."
  cat > "$ARTIFACT_DIR/teardown-status.json" <<'EOF'
{"status":"skip","stage":"teardown","reason_code":"preserve_runtime","exit_code":0}
EOF
  exit 0
fi

PROFILE="${BANANA_E2E_COMPOSE_PROFILE:-apps}"

docker compose --profile "$PROFILE" ps > "$DIAGNOSTICS_DIR/compose-ps-before-teardown.txt" 2>&1 || true
docker compose --profile "$PROFILE" logs --no-color > "$DIAGNOSTICS_DIR/compose-before-teardown.log" 2>&1 || true

set +e
docker compose --profile "$PROFILE" down -v --remove-orphans
teardown_exit=$?
set -e

if (( teardown_exit != 0 )); then
  docker compose --profile "$PROFILE" ps > "$DIAGNOSTICS_DIR/compose-ps-after-teardown-failure.txt" 2>&1 || true
  docker compose --profile "$PROFILE" logs --no-color > "$DIAGNOSTICS_DIR/compose-after-teardown-failure.log" 2>&1 || true

  cat > "$ARTIFACT_DIR/teardown-status.json" <<EOF
{"status":"fail","stage":"teardown","reason_code":"compose_down_failed","exit_code":$teardown_exit}
EOF
  echo "Teardown failed for E2E runtime profile '$PROFILE'."
  exit "$teardown_exit"
fi

cat > "$ARTIFACT_DIR/teardown-status.json" <<'EOF'
{"status":"pass","stage":"teardown","reason_code":"success","exit_code":0}
EOF

echo "Tore down E2E runtime profile '$PROFILE'."

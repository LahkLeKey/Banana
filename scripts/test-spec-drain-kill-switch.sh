#!/usr/bin/env bash
set -euo pipefail

# Verifies that BANANA_AUTONOMY_STOP=true causes the spec-drain loop to stop
# immediately and emit a kill_switch stop reason in the checkpoint state.
#
# Usage:
#   bash scripts/test-spec-drain-kill-switch.sh

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

TMPDIR_TEST=$(mktemp -d)
trap 'rm -rf "$TMPDIR_TEST"' EXIT

STATE_PATH="${TMPDIR_TEST}/spec-drain-state.json"
echo "[test:kill-switch] Using state path: $STATE_PATH"

# Initialize a fresh checkpoint.
bash scripts/workflow-spec-drain-state.sh init "$STATE_PATH" "test-kill-switch" "1"

# Run the orchestrator with kill switch ON and dry-run to avoid real effects.
BANANA_SDLC_MODE=spec-drain \
BANANA_AUTONOMY_STOP=true \
BANANA_DRAIN_DRY_RUN=true \
BANANA_DRAIN_STATE_PATH="$STATE_PATH" \
BANANA_SDLC_DRAIN_STATE_PATH="$STATE_PATH" \
BANANA_SDLC_AUTO_DEBUG_CI=false \
BANANA_SDLC_AUTO_SCAFFOLD_SPECS=false \
BANANA_SDLC_VALIDATE_AI_CONTRACTS=false \
BANANA_SDLC_ENABLE_WIKI_SYNC=false \
bash scripts/workflow-orchestrate-sdlc.sh

# Verify the stop reason in the checkpoint state.
STOP_REASON=$(python - "$STATE_PATH" <<'PY'
import json, pathlib, sys
state = json.loads(pathlib.Path(sys.argv[1]).read_text(encoding="utf-8"))
print(state.get("stop_reason", ""))
PY
)

if [[ "$STOP_REASON" != "kill_switch" ]]; then
  echo "[test:kill-switch] FAIL: Expected stop_reason='kill_switch', got '$STOP_REASON'" >&2
  exit 1
fi

STATUS=$(python - "$STATE_PATH" <<'PY'
import json, pathlib, sys
state = json.loads(pathlib.Path(sys.argv[1]).read_text(encoding="utf-8"))
print(state.get("status", ""))
PY
)

if [[ "$STATUS" != "stopped" ]]; then
  echo "[test:kill-switch] FAIL: Expected status='stopped', got '$STATUS'" >&2
  exit 1
fi

echo "[test:kill-switch] PASS"
echo "  stop_reason: $STOP_REASON"
echo "  status: $STATUS"
echo "  Kill switch correctly halted the drain before processing any specs."

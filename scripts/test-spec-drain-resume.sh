#!/usr/bin/env bash
set -euo pipefail

# Simulates a mid-run spec-drain interruption and verifies that the loop
# resumes correctly from the checkpoint state.
#
# Usage:
#   bash scripts/test-spec-drain-resume.sh

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

TMPDIR_TEST=$(mktemp -d)
trap 'rm -rf "$TMPDIR_TEST"' EXIT

STATE_PATH="${TMPDIR_TEST}/spec-drain-state.json"
echo "[test:resume] Using state path: $STATE_PATH"

# --- Phase 1: Initialize state and mark one spec as completed ---
bash scripts/workflow-spec-drain-state.sh init "$STATE_PATH" "test-resume" "1"

# Discover what specs actually exist so we can simulate partial progress.
FIRST_SPEC=$(bash scripts/workflow-spec-drain-state.sh get-next-runnable "$STATE_PATH")
if [[ -z "$FIRST_SPEC" ]]; then
  echo "[test:resume] SKIP: No runnable specs found (no spec with spec.md + tasks.md). Skipping resume test."
  exit 0
fi

echo "[test:resume] Simulating completed spec: $FIRST_SPEC"
bash scripts/workflow-spec-drain-state.sh mark-completed "$STATE_PATH" "$FIRST_SPEC"

# Verify is-completed returns true.
RESULT=$(bash scripts/workflow-spec-drain-state.sh is-completed "$STATE_PATH" "$FIRST_SPEC")
if [[ "$RESULT" != "true" ]]; then
  echo "[test:resume] FAIL: is-completed returned '$RESULT' for $FIRST_SPEC; expected 'true'" >&2
  exit 1
fi

# --- Phase 2: Re-initialize state (simulates resume after interruption) ---
# init must NOT overwrite existing checkpoint.
bash scripts/workflow-spec-drain-state.sh init "$STATE_PATH" "test-resume" "2"

# Verify spec is STILL completed after re-init.
RESULT=$(bash scripts/workflow-spec-drain-state.sh is-completed "$STATE_PATH" "$FIRST_SPEC")
if [[ "$RESULT" != "true" ]]; then
  echo "[test:resume] FAIL: checkpoint was not preserved after re-init. is-completed returned '$RESULT'" >&2
  exit 1
fi

# --- Phase 3: get-next-runnable must skip the completed spec ---
NEXT=$(bash scripts/workflow-spec-drain-state.sh get-next-runnable "$STATE_PATH")
if [[ "$NEXT" == "$FIRST_SPEC" ]]; then
  echo "[test:resume] FAIL: get-next-runnable returned the already-completed spec '$FIRST_SPEC'" >&2
  exit 1
fi

echo "[test:resume] PASS"
echo "  Checkpoint preserved across re-init."
echo "  Completed spec '$FIRST_SPEC' correctly skipped on resume."
if [[ -n "$NEXT" ]]; then
  echo "  Next runnable spec: $NEXT"
else
  echo "  No further runnable specs (drain exhausted after completing first)."
fi

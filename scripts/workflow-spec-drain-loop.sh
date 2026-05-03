#!/usr/bin/env bash
set -euo pipefail

# Spec-drain execution loop.
# Processes runnable specs sequentially until backlog is exhausted or a stop condition fires.
#
# Environment variables:
#   BANANA_DRAIN_STATE_PATH      Path to checkpoint state JSON (required)
#   BANANA_DRAIN_MAX_FAILURES    Max unique failed specs before stopping (default: 5)
#   BANANA_DRAIN_MAX_RETRIES     Max retries per spec before permanently failing (default: 2)
#   BANANA_DRAIN_DRY_RUN         If "true", simulate without making real changes (default: false)
#   BANANA_AUTONOMY_STOP         Emergency kill switch (default: false)
#   BANANA_DRAIN_OUTPUT_DIR      Output dir for per-spec evidence (default: artifacts/orchestration/spec-drain)
#   GH_TOKEN                     GitHub token for PR/branch operations

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

STATE_PATH="${BANANA_DRAIN_STATE_PATH:-artifacts/sdlc-orchestration/spec-drain-state.json}"
MAX_FAILURES="${BANANA_DRAIN_MAX_FAILURES:-5}"
MAX_RETRIES="${BANANA_DRAIN_MAX_RETRIES:-2}"
DRY_RUN="${BANANA_DRAIN_DRY_RUN:-false}"
AUTONOMY_STOP="${BANANA_AUTONOMY_STOP:-false}"
OUTPUT_DIR="${BANANA_DRAIN_OUTPUT_DIR:-artifacts/orchestration/spec-drain}"
RUN_ID="${GITHUB_RUN_ID:-local-run}"
PLAN_PATH="${BANANA_DRAIN_PLAN_PATH:-}"

mkdir -p "$OUTPUT_DIR"

log() { echo "[spec-drain] $*"; }
log_err() { echo "[spec-drain][ERROR] $*" >&2; }

load_spec_plan() {
  local plan_path="$1"
  local spec_id="$2"

  python - "$plan_path" "$spec_id" <<'PY'
import json
import os
import pathlib
import shlex
import sys

plan_path = pathlib.Path(sys.argv[1])
spec_id = sys.argv[2]

if not plan_path.exists():
  raise SystemExit(f"plan file not found: {plan_path}")

payload = json.loads(plan_path.read_text(encoding="utf-8"))
if isinstance(payload, dict):
  entries = payload.get("specs", payload.get("entries", payload.get("increments", [])))
elif isinstance(payload, list):
  entries = payload
else:
  raise SystemExit("spec-drain plan must be a JSON object or array")

if not isinstance(entries, list):
  raise SystemExit("spec-drain plan entries must be a JSON array")

entry = None
for candidate in entries:
  if not isinstance(candidate, dict):
    continue
  candidate_id = str(candidate.get("spec_id", candidate.get("id", ""))).strip()
  if candidate_id == spec_id:
    entry = candidate
    break

if entry is None:
  raise SystemExit(f"spec '{spec_id}' missing from spec-drain plan")

change_command = str(entry.get("change_command", "")).strip()
if not change_command:
  raise SystemExit(f"spec '{spec_id}' missing required change_command")

defaults = {
  "BANANA_TRIAGE_CHANGE_COMMAND": change_command,
  "BANANA_TRIAGE_ID": str(entry.get("triage_id", spec_id)).strip() or spec_id,
  "BANANA_COMMIT_MESSAGE": str(entry.get("commit_message", f"chore(spec-drain): implement {spec_id}")).strip(),
  "BANANA_PR_TITLE": str(entry.get("pr_title", f"triage({spec_id}): automated changes requiring human approval")).strip(),
  "BANANA_PR_BODY": str(entry.get("pr_body", f"Automated spec-drain implementation for {spec_id}.")).strip(),
  "BANANA_PR_LABELS": str(entry.get("labels", os.environ.get("BANANA_PR_LABELS", "automation,triaged-item,requires-human-approval,copilot-auto-approve,speckit-driven"))).strip(),
  "BANANA_PR_REVIEWERS": str(entry.get("reviewers", os.environ.get("BANANA_PR_REVIEWERS", ""))).strip(),
  "BANANA_DRAFT_PR": str(entry.get("draft_pr", os.environ.get("BANANA_DRAFT_PR", "true"))).strip(),
  "BANANA_BASE_BRANCH": str(entry.get("base_branch", os.environ.get("BANANA_BASE_BRANCH", "main"))).strip(),
  "BANANA_BRANCH_PREFIX": str(entry.get("branch_prefix", os.environ.get("BANANA_BRANCH_PREFIX", "triage"))).strip(),
  "BANANA_SKIP_IF_NO_CHANGES": str(entry.get("skip_if_no_changes", os.environ.get("BANANA_SKIP_IF_NO_CHANGES", "false"))).strip(),
  "BANANA_REQUIRE_REAL_UPDATES": str(entry.get("require_real_updates", os.environ.get("BANANA_REQUIRE_REAL_UPDATES", "true"))).strip(),
  "BANANA_SKIP_DOCS_ONLY_CHANGES": str(entry.get("skip_docs_only_changes", os.environ.get("BANANA_SKIP_DOCS_ONLY_CHANGES", "true"))).strip(),
}

for key, value in defaults.items():
  print(f"export {key}={shlex.quote(value)}")
PY
}

# --- Kill switch ---
if [[ "$AUTONOMY_STOP" == "true" ]]; then
  log "Kill switch active. Setting stop reason and exiting."
  bash scripts/workflow-spec-drain-state.sh set-stop "$STATE_PATH" "kill_switch"
  bash scripts/workflow-spec-drain-state.sh get-summary "$STATE_PATH"
  exit 0
fi

# --- Main drain loop (T008) ---
LOOP_COUNT=0
MAX_LOOP=200  # Safety cap; each spec should take well under this many iterations.

while [[ $LOOP_COUNT -lt $MAX_LOOP ]]; do
  LOOP_COUNT=$((LOOP_COUNT + 1))

  # Check kill switch env each iteration (can be set by signal handler or parent).
  if [[ "${BANANA_AUTONOMY_STOP:-false}" == "true" ]]; then
    log "Kill switch detected mid-loop."
    bash scripts/workflow-spec-drain-state.sh set-stop "$STATE_PATH" "kill_switch"
    break
  fi

  # Get next runnable spec (T005, T017 idempotency guard).
  NEXT_SPEC=$(bash scripts/workflow-spec-drain-state.sh get-next-runnable "$STATE_PATH")

  if [[ -z "$NEXT_SPEC" ]]; then
    log "No runnable specs remain. Drain exhausted."
    bash scripts/workflow-spec-drain-state.sh set-stop "$STATE_PATH" "exhausted"
    break
  fi

  log "Next runnable spec: $NEXT_SPEC (iteration $LOOP_COUNT)"

  SPEC_OUTPUT_DIR="${OUTPUT_DIR}/${RUN_ID}/${NEXT_SPEC}"
  EVIDENCE_PATH="${SPEC_OUTPUT_DIR}/result.json"
  mkdir -p "$SPEC_OUTPUT_DIR"

  # Check failure budget (T006, T013).
  BUDGET_STATUS=$(bash scripts/workflow-spec-drain-state.sh get-budget-status "$STATE_PATH" "$MAX_FAILURES")
  if [[ "$BUDGET_STATUS" == "exceeded" ]]; then
    log "Failure budget exceeded (max: $MAX_FAILURES unique failures). Stopping."
    bash scripts/workflow-spec-drain-state.sh set-stop "$STATE_PATH" "budget_exceeded"
    break
  fi

  # Check retry count for this spec.
  RETRY_COUNT=$(python - "$STATE_PATH" "$NEXT_SPEC" <<'PY'
import json, pathlib, sys
state = json.loads(pathlib.Path(sys.argv[1]).read_text(encoding="utf-8"))
counts = state.get("failure_counts", {})
print(counts.get(sys.argv[2], 0))
PY
  )

  if [[ "$RETRY_COUNT" -ge "$MAX_RETRIES" ]]; then
    log "Spec $NEXT_SPEC has exceeded max retries ($MAX_RETRIES). Permanently failing."
    bash scripts/workflow-spec-drain-state.sh mark-failed "$STATE_PATH" "$NEXT_SPEC" "max_retries_exceeded"
    bash scripts/workflow-spec-drain-state.sh write-evidence "$STATE_PATH" "$NEXT_SPEC" \
      "$EVIDENCE_PATH" "permanently_failed" "max_retries_exceeded"
    continue
  fi

  # Validate spec quality before attempting implementation (T010, spec 104 integration).
  if [[ -f "scripts/validate-spec-quality.sh" ]]; then
    SPEC_FILE=".specify/specs/${NEXT_SPEC}/spec.md"
    if ! bash scripts/validate-spec-quality.sh --spec "$SPEC_FILE" 2>/dev/null; then
      log "Spec $NEXT_SPEC failed quality gate. Marking as BLOCKED: quality_gate."
      bash scripts/workflow-spec-drain-state.sh mark-failed "$STATE_PATH" "$NEXT_SPEC" "quality_gate"
      bash scripts/workflow-spec-drain-state.sh write-evidence "$STATE_PATH" "$NEXT_SPEC" \
        "$EVIDENCE_PATH" "blocked" "quality_gate"
      continue
    fi
  fi

  # Execute the spec (T009 — per-spec branch and PR orchestration).
  SPEC_EXIT=0
  if [[ "$DRY_RUN" == "true" ]]; then
    log "[dry-run] Would implement spec: $NEXT_SPEC"
    SPEC_STATUS="dry_run"
    SPEC_REASON=""
  else
    log "Implementing spec: $NEXT_SPEC"

    if [[ -z "$PLAN_PATH" ]]; then
      SPEC_STATUS="policy_blocked"
      SPEC_REASON="missing_spec_drain_plan"
      log_err "Real execution requires BANANA_DRAIN_PLAN_PATH with per-spec change_command entries."
      bash scripts/workflow-spec-drain-state.sh set-stop "$STATE_PATH" "policy_blocked"
      bash scripts/workflow-spec-drain-state.sh write-evidence "$STATE_PATH" "$NEXT_SPEC" \
        "$EVIDENCE_PATH" "$SPEC_STATUS" "$SPEC_REASON"
      break
    fi

    PLAN_EXPORTS=""
    if ! PLAN_EXPORTS="$(load_spec_plan "$PLAN_PATH" "$NEXT_SPEC")"; then
      SPEC_STATUS="policy_blocked"
      SPEC_REASON="invalid_or_missing_plan_entry"
      log_err "Unable to resolve execution plan for $NEXT_SPEC from $PLAN_PATH"
      bash scripts/workflow-spec-drain-state.sh set-stop "$STATE_PATH" "policy_blocked"
      bash scripts/workflow-spec-drain-state.sh write-evidence "$STATE_PATH" "$NEXT_SPEC" \
        "$EVIDENCE_PATH" "$SPEC_STATUS" "$SPEC_REASON"
      break
    fi

    eval "$PLAN_EXPORTS"

    # Delegate to the triaged-item PR orchestration flow (T009).
    export BANANA_SDLC_OUTPUT_DIR="$SPEC_OUTPUT_DIR"
    export BANANA_SDLC_SUMMARY_PATH="${SPEC_OUTPUT_DIR}/sdlc-summary.json"

    if bash scripts/workflow-orchestrate-triaged-item-pr.sh \
        --spec "$NEXT_SPEC" \
        --output "$SPEC_OUTPUT_DIR" 2>&1 | tee "${SPEC_OUTPUT_DIR}/execution.log"; then
      SPEC_STATUS="completed"
      SPEC_REASON=""
    else
      SPEC_EXIT=$?
      SPEC_STATUS="failed"
      SPEC_REASON="exit_code_${SPEC_EXIT}"
    fi
  fi

  # Emit per-spec evidence JSON (T011).
  bash scripts/workflow-spec-drain-state.sh write-evidence "$STATE_PATH" "$NEXT_SPEC" \
    "$EVIDENCE_PATH" "$SPEC_STATUS" "$SPEC_REASON"

  # Update state based on outcome (T015 cursor persistence).
  if [[ "$SPEC_STATUS" == "completed" || "$SPEC_STATUS" == "dry_run" ]]; then
    bash scripts/workflow-spec-drain-state.sh mark-completed "$STATE_PATH" "$NEXT_SPEC"
    log "Spec $NEXT_SPEC completed successfully."
  else
    bash scripts/workflow-spec-drain-state.sh mark-failed "$STATE_PATH" "$NEXT_SPEC" "$SPEC_REASON"
    log "Spec $NEXT_SPEC failed: $SPEC_REASON (attempt $((RETRY_COUNT + 1)) of $MAX_RETRIES)."
  fi
done

# --- Terminal summary (T014) ---
SUMMARY=$(bash scripts/workflow-spec-drain-state.sh get-summary "$STATE_PATH")
echo "$SUMMARY"

SUMMARY_FILE="${OUTPUT_DIR}/${RUN_ID}/drain-summary.json"
echo "$SUMMARY" > "$SUMMARY_FILE"
log "Terminal summary written to: $SUMMARY_FILE"

# Determine exit code based on stop reason.
STOP_REASON=$(python - "$SUMMARY_FILE" <<'PY'
import json
import pathlib
import sys

summary_path = pathlib.Path(sys.argv[1])
payload = json.loads(summary_path.read_text(encoding="utf-8"))
print(payload.get("stop_reason", "unknown"))
PY
)

case "$STOP_REASON" in
  exhausted|kill_switch|dry_run) exit 0 ;;
  budget_exceeded|policy_blocked) exit 2 ;;
  *) exit 1 ;;
esac

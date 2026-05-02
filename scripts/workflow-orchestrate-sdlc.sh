#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

bash scripts/workflow-ensure-speckit.sh

RUN_ID="${GITHUB_RUN_ID:-local-run}"
RUN_ATTEMPT="${GITHUB_RUN_ATTEMPT:-1}"
SDLC_ID="${BANANA_SDLC_ID:-banana-sdlc}"
SDLC_MODE="${BANANA_SDLC_MODE:-incremental}"
AUTONOMY_STOP="${BANANA_AUTONOMY_STOP:-false}"
AUTO_DEBUG_CI="${BANANA_SDLC_AUTO_DEBUG_CI:-true}"
AUTO_SCAFFOLD_SPECS="${BANANA_SDLC_AUTO_SCAFFOLD_SPECS:-true}"
CI_IDEA_BUDGET="${BANANA_SDLC_CI_IDEA_BUDGET:-3}"
OPEN_CI_ISSUES="${BANANA_SDLC_CI_OPEN_ISSUES:-true}"
CI_ISSUE_LABELS="${BANANA_SDLC_CI_ISSUE_LABELS:-automation,ci,triage,agent:workflow-agent}"
CI_ISSUE_TITLE_PREFIX="${BANANA_SDLC_CI_ISSUE_TITLE_PREFIX:-CI Auto-Debug}"
CI_LOG_EXCERPT_LINES="${BANANA_SDLC_CI_LOG_EXCERPT_LINES:-120}"
PLAN_JSON="${BANANA_SDLC_INCREMENT_PLAN_JSON:-}"
PLAN_PATH="${BANANA_SDLC_INCREMENT_PLAN_PATH:-}"
PLAN_MODEL="${BANANA_SDLC_INCREMENT_PLAN_MODEL:-}"
PLAN_CATALOG_PATH="${BANANA_SDLC_INCREMENT_CATALOG_PATH:-docs/automation/agent-pulse/autonomous-self-training-default-increments.json}"
DEFAULT_BASE_BRANCH="${BANANA_BASE_BRANCH:-main}"
DEFAULT_BRANCH_PREFIX="${BANANA_BRANCH_PREFIX:-sdlc}"
DEFAULT_DRAFT_PR="${BANANA_DRAFT_PR:-true}"
DEFAULT_LABELS="${BANANA_PR_LABELS:-automation,sdlc,triaged-item,requires-human-approval,copilot-auto-approve,speckit-driven,agent:workflow-agent}"
DEFAULT_REVIEWERS="${BANANA_PR_REVIEWERS:-}"
SKIP_NO_CHANGES="${BANANA_SDLC_SKIP_NO_CHANGES:-true}"
CONTINUE_ON_ERROR="${BANANA_SDLC_CONTINUE_ON_ERROR:-false}"
SDLC_REQUIRE_REAL_UPDATES="${BANANA_SDLC_REQUIRE_REAL_UPDATES:-true}"
SDLC_SKIP_NON_REAL_UPDATES="${BANANA_SDLC_SKIP_NON_REAL_UPDATES:-true}"
REAL_UPDATE_PATH_PATTERNS="${BANANA_REAL_UPDATE_PATH_PATTERNS:-src/**,tests/**,scripts/**,.github/workflows/**,docker/**,docker-compose.yml,CMakeLists.txt,Directory.Build.props}"
INCREMENT_TIMEOUT_SECONDS="${BANANA_SDLC_INCREMENT_TIMEOUT_SECONDS:-900}"
ENABLE_WIKI_SYNC="${BANANA_SDLC_ENABLE_WIKI_SYNC:-true}"
WIKI_DRY_RUN="${BANANA_WIKI_DRY_RUN:-false}"
WIKI_PUSH="${BANANA_WIKI_PUSH:-true}"
WIKI_STRICT="${BANANA_WIKI_STRICT:-false}"
WIKI_COMMIT_MESSAGE="${BANANA_WIKI_COMMIT_MESSAGE:-docs(wiki): sync automated SDLC documentation snapshots}"
WIKI_REMOTE_URL="${BANANA_WIKI_REMOTE_URL:-https://github.com/LahkLeKey/Banana.wiki.git}"
WIKI_ENFORCE_CANONICAL_REMOTE="${BANANA_ENFORCE_CANONICAL_WIKI_REMOTE:-true}"
OUTPUT_DIR="${BANANA_SDLC_OUTPUT_DIR:-artifacts/sdlc-orchestration}"
SUMMARY_PATH="${BANANA_SDLC_SUMMARY_PATH:-${OUTPUT_DIR}/summary-${RUN_ID}-attempt-${RUN_ATTEMPT}.json}"
SPEC_DRAIN_STATE_PATH="${BANANA_SDLC_DRAIN_STATE_PATH:-${OUTPUT_DIR}/spec-drain-state-${RUN_ID}-attempt-${RUN_ATTEMPT}.json}"
CI_FOLLOWUP_OUTPUT_PATH="${BANANA_SDLC_CI_FOLLOWUP_OUTPUT_PATH:-${OUTPUT_DIR}/ci-followup-${RUN_ID}-attempt-${RUN_ATTEMPT}.json}"
RUN_API_PARITY_CHECK="${BANANA_SDLC_RUN_API_PARITY_CHECK:-false}"
API_PARITY_STRICT="${BANANA_SDLC_API_PARITY_STRICT:-false}"
ENABLE_AUTO_MERGE="${BANANA_ENABLE_AUTO_MERGE:-true}"
AUTO_MERGE_METHOD="${BANANA_AUTO_MERGE_METHOD:-squash}"

CLI_MODE="${1:-}"
CLI_TARGET="${2:-}"

# Positional shortcut: bash scripts/workflow-orchestrate-sdlc.sh speckit:bootstrap 104
if [[ "$CLI_MODE" == "speckit:bootstrap" ]]; then
  SDLC_MODE="speckit-bootstrap"
  if [[ -n "$CLI_TARGET" ]]; then
    export BANANA_SPECKIT_BOOTSTRAP_TARGET="$CLI_TARGET"
  fi
fi

mkdir -p "$OUTPUT_DIR"

# ---------------------------------------------------------------------------
# speckit-bootstrap mode (spec 104 FR-6)
# ---------------------------------------------------------------------------
if [[ "$SDLC_MODE" == "speckit-bootstrap" ]]; then
  BOOTSTRAP_TARGET="${BANANA_SPECKIT_BOOTSTRAP_TARGET:-}"
  if [[ -z "$BOOTSTRAP_TARGET" ]]; then
    echo "Error: BANANA_SPECKIT_BOOTSTRAP_TARGET must be set for speckit-bootstrap mode." >&2
    exit 1
  fi

  # Allow numeric target (e.g., 104) by resolving to .specify/specs/104-*
  if [[ "$BOOTSTRAP_TARGET" =~ ^[0-9]+$ ]]; then
    BOOTSTRAP_TARGET_PADDED=$(printf "%03d" "$((10#$BOOTSTRAP_TARGET))")
    MATCHED_SPEC_DIR=$(find .specify/specs -maxdepth 1 -type d -name "${BOOTSTRAP_TARGET_PADDED}-*" | head -n 1 || true)
    if [[ -n "$MATCHED_SPEC_DIR" ]]; then
      BOOTSTRAP_TARGET="$(basename "$MATCHED_SPEC_DIR")"
    fi
  fi

  SPEC_DIR=".specify/specs/${BOOTSTRAP_TARGET}"
  SPEC_FILE="${SPEC_DIR}/spec.md"

  if [[ ! -f "$SPEC_FILE" ]]; then
    echo "Error: spec.md not found at ${SPEC_FILE}. Run specify specify first." >&2
    exit 1
  fi

  echo "[speckit:bootstrap] Running quality gate on spec: $BOOTSTRAP_TARGET"
  if [[ -f "scripts/validate-spec-quality.sh" ]]; then
    bash scripts/validate-spec-quality.sh --spec "$SPEC_FILE"
  fi

  echo "[speckit:bootstrap] Running planning/task generation for: $BOOTSTRAP_TARGET"
  bash scripts/workflow-ensure-speckit.sh

  BOOTSTRAP_USED_CLI="false"

  # Legacy/compatible CLI surface.
  if specify plan --help >/dev/null 2>&1 && specify tasks --help >/dev/null 2>&1; then
    if specify plan && specify tasks; then
      BOOTSTRAP_USED_CLI="true"
    fi
  fi

  # Deterministic non-interactive fallback for CLI variants that do not expose
  # plan/tasks subcommands directly.
  if [[ "$BOOTSTRAP_USED_CLI" != "true" ]]; then
    echo "[speckit:bootstrap] Using local fallback for plan/tasks generation."

    if [[ ! -f "${SPEC_DIR}/plan.md" ]]; then
      if [[ -f ".specify/templates/plan-template.md" ]]; then
        cp ".specify/templates/plan-template.md" "${SPEC_DIR}/plan.md"
      elif [[ -f ".specify/templates/plan-skeleton.md" ]]; then
        cp ".specify/templates/plan-skeleton.md" "${SPEC_DIR}/plan.md"
      else
        : > "${SPEC_DIR}/plan.md"
      fi
    fi

    if [[ ! -f "${SPEC_DIR}/tasks.md" ]]; then
      if [[ -f ".specify/templates/tasks-template.md" ]]; then
        cp ".specify/templates/tasks-template.md" "${SPEC_DIR}/tasks.md"
      else
        : > "${SPEC_DIR}/tasks.md"
      fi
    fi
  fi

  if [[ ! -f "${SPEC_DIR}/plan.md" || ! -f "${SPEC_DIR}/tasks.md" ]]; then
    echo "Error: bootstrap did not produce required artifacts in ${SPEC_DIR}." >&2
    exit 1
  fi

  echo "[speckit:bootstrap] Bootstrap complete for: $BOOTSTRAP_TARGET"
  echo "  spec:  ${SPEC_FILE}"
  echo "  plan:  ${SPEC_DIR}/plan.md"
  echo "  tasks: ${SPEC_DIR}/tasks.md"
  exit 0
fi

if [[ "$SDLC_MODE" == "spec-drain" ]]; then
  echo "Running SDLC in spec-drain mode..."

  bash scripts/workflow-spec-drain-state.sh init "$SPEC_DRAIN_STATE_PATH" "$RUN_ID" "$RUN_ATTEMPT"

  if [[ "$AUTONOMY_STOP" == "true" ]]; then
    bash scripts/workflow-spec-drain-state.sh set-stop "$SPEC_DRAIN_STATE_PATH" "kill_switch"
  fi

  if [[ "$AUTO_DEBUG_CI" == "true" && "$AUTO_SCAFFOLD_SPECS" == "true" && "$AUTONOMY_STOP" != "true" ]]; then
    export BANANA_SDLC_CI_IDEA_BUDGET="$CI_IDEA_BUDGET"
    export BANANA_SDLC_CI_FOLLOWUP_OUTPUT_PATH="$CI_FOLLOWUP_OUTPUT_PATH"
    export BANANA_SDLC_CI_OPEN_ISSUES="$OPEN_CI_ISSUES"
    export BANANA_SDLC_CI_ISSUE_LABELS="$CI_ISSUE_LABELS"
    export BANANA_SDLC_CI_ISSUE_TITLE_PREFIX="$CI_ISSUE_TITLE_PREFIX"
    export BANANA_SDLC_CI_LOG_EXCERPT_LINES="$CI_LOG_EXCERPT_LINES"
    bash scripts/workflow-autonomous-ci-followup.sh
  fi

  python - "$SUMMARY_PATH" "$SPEC_DRAIN_STATE_PATH" "$AUTONOMY_STOP" "$CI_FOLLOWUP_OUTPUT_PATH" <<'PY'
import json
import pathlib
import re
import sys

summary_path = pathlib.Path(sys.argv[1])
state_path = pathlib.Path(sys.argv[2])
autonomy_stop = sys.argv[3].lower() == "true"
ci_followup_path = pathlib.Path(sys.argv[4])

state = json.loads(state_path.read_text(encoding="utf-8"))

spec_dirs = []
for path in sorted(pathlib.Path(".specify/specs").glob("*")):
    if not path.is_dir():
        continue
    name = path.name
    if not re.match(r"^[0-9]{3,}-", name):
        continue
    if (path / "spec.md").exists():
        spec_dirs.append(name)

completed = set(state.get("completed_specs", []))
failed = {str(item.get("spec_id")) for item in state.get("failed_specs", [])}
runnable = [spec for spec in spec_dirs if spec not in completed and spec not in failed]

stop_reason = state.get("stop_reason")
ci_followup = None
if ci_followup_path.exists():
    try:
        ci_followup = json.loads(ci_followup_path.read_text(encoding="utf-8"))
    except Exception:
        ci_followup = {"status": "failed", "reason": f"unable to parse {ci_followup_path.name}"}

if stop_reason is None:
    if autonomy_stop:
      stop_reason = "kill_switch"
    elif ci_followup is not None and ci_followup.get("status") != "ok":
      stop_reason = "ci_followup_failed"
    elif ci_followup is not None and int(ci_followup.get("created_spec_count", 0)) == 0:
      stop_reason = "ci_runs_out_of_ideas"
    elif not runnable:
      stop_reason = "exhausted"
    else:
      stop_reason = "runnable_specs_available"

summary = {
    "mode": "spec-drain",
    "status": "stopped" if autonomy_stop or stop_reason == "exhausted" else "running",
    "stop_reason": stop_reason,
    "spec_discovery": {
        "total_specs": len(spec_dirs),
        "runnable_specs": runnable,
        "completed_specs": sorted(completed),
        "failed_specs": sorted(failed),
    },
    "ci_followup": ci_followup,
    "state_path": str(state_path),
}

summary_path.parent.mkdir(parents=True, exist_ok=True)
summary_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
print(json.dumps(summary, indent=2))
PY

  echo "Spec-drain summary written to: ${SUMMARY_PATH}"
  exit 0
fi

if [[ "${BANANA_SDLC_VALIDATE_AI_CONTRACTS:-true}" == "true" ]]; then
  echo "Validating AI customization and wiki-sync contracts..."
  python scripts/validate-ai-contracts.py > "${OUTPUT_DIR}/ai-contract-report-${RUN_ID}-attempt-${RUN_ATTEMPT}.json"
fi

if [[ "$RUN_API_PARITY_CHECK" == "true" ]]; then
  echo "Running API parity governance preflight..."
  parity_args=()
  if [[ "$API_PARITY_STRICT" == "true" ]]; then
    parity_args+=(--strict)
  else
    parity_args+=(--inventory-only)
  fi
  bash scripts/validate-api-parity-governance.sh "${parity_args[@]}"
fi

PLAN_INPUT_PATH="${OUTPUT_DIR}/plan-${RUN_ID}-attempt-${RUN_ATTEMPT}.json"
PLAN_ROWS_PATH="${OUTPUT_DIR}/plan-rows-${RUN_ID}-attempt-${RUN_ATTEMPT}.tsv"
WIKI_REPORT_PATH="${OUTPUT_DIR}/wiki-sync-${RUN_ID}-attempt-${RUN_ATTEMPT}.json"
DETERMINISTIC_PLAN_PATH="${BANANA_SDLC_DETERMINISTIC_PLAN_PATH:-${OUTPUT_DIR}/deterministic-agent-pulse-plan-${RUN_ID}-attempt-${RUN_ATTEMPT}.json}"
DETERMINISTIC_MODEL_JSON_PATH="${BANANA_SDLC_DETERMINISTIC_MODEL_JSON_PATH:-${OUTPUT_DIR}/deterministic-agent-pulse-model-${RUN_ID}-attempt-${RUN_ATTEMPT}.json}"
DETERMINISTIC_MODEL_BINARY_PATH="${BANANA_SDLC_DETERMINISTIC_MODEL_BINARY_PATH:-${OUTPUT_DIR}/banana_agent_pulse_model_cli-${RUN_ID}-attempt-${RUN_ATTEMPT}}"

if [[ -z "$PLAN_JSON" && "$PLAN_MODEL" == "native-deterministic-agent-pulse" ]]; then
  echo "Rendering deterministic SDLC plan from native C model..."
  bash scripts/generate-deterministic-agent-pulse-plan.sh \
    --catalog "$PLAN_CATALOG_PATH" \
    --output "$DETERMINISTIC_PLAN_PATH" \
    --model-json "$DETERMINISTIC_MODEL_JSON_PATH" \
    --model-bin "$DETERMINISTIC_MODEL_BINARY_PATH"
  PLAN_PATH="$DETERMINISTIC_PLAN_PATH"
fi

if [[ -n "$PLAN_JSON" ]]; then
  printf '%s\n' "$PLAN_JSON" > "$PLAN_INPUT_PATH"
elif [[ -n "$PLAN_PATH" ]]; then
  cp "$PLAN_PATH" "$PLAN_INPUT_PATH"
else
  cat > "$PLAN_INPUT_PATH" <<'EOF'
[
  {
    "id": "classifier-feedback-training",
    "change_command": "python scripts/apply-not-banana-feedback.py --feedback data/not-banana/feedback/inbox.json --corpus data/not-banana/corpus.json --status-filter approved --consume --require-human-review --minimum-human-reviewers 1 --report artifacts/not-banana-feedback/sdlc-apply-report.json && python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/not-banana-model --training-profile ci --session-mode single --max-sessions 1 && mkdir -p src/native/core/domain/generated && cp artifacts/not-banana-model/banana_signal_tokens.h src/native/core/domain/generated/banana_signal_tokens.h",
    "commit_message": "chore(classifier): apply feedback and refresh vocabulary snapshot",
    "pr_title": "triage(classifier): feedback and vocabulary refresh",
    "pr_body": "Automated SDLC increment: apply approved not-banana feedback and refresh generated native vocabulary header.",
    "labels": "automation,sdlc,triaged-item,requires-human-approval,copilot-auto-approve,speckit-driven,feedback-loop,model-training,agent:banana-classifier-agent"
  },
  {
    "id": "api-coverage-denominator-sync",
    "change_command": "python scripts/sync-banana-api-coverage-denominator.py",
    "commit_message": "chore(api): sync coverage denominator manifest",
    "pr_title": "triage(api): sync coverage denominator manifest",
    "pr_body": "Automated SDLC increment: keep Banana.Api coverage denominator manifest aligned with source file inventory.",
    "labels": "automation,sdlc,triaged-item,requires-human-approval,copilot-auto-approve,speckit-driven,coverage-100,agent:workflow-agent"
  }
]
EOF
fi

python - "$PLAN_INPUT_PATH" "$PLAN_ROWS_PATH" <<'PY'
import base64
import json
import pathlib
import re
import sys

plan_path = pathlib.Path(sys.argv[1])
rows_path = pathlib.Path(sys.argv[2])
raw = json.loads(plan_path.read_text(encoding="utf-8"))

if isinstance(raw, dict):
    increments = raw.get("increments", [])
elif isinstance(raw, list):
    increments = raw
else:
    raise SystemExit("SDLC plan must be a JSON array or an object with an 'increments' array.")

if not increments:
    raise SystemExit("SDLC plan has no increments.")

rows: list[str] = []
seen_ids: set[str] = set()
for index, entry in enumerate(increments, start=1):
    if not isinstance(entry, dict):
        raise SystemExit(f"Increment #{index} must be a JSON object.")

    increment_id_raw = str(entry.get("id", f"increment-{index}"))
    increment_id = re.sub(r"[^a-z0-9._-]+", "-", increment_id_raw.lower()).strip("-._")
    if not increment_id:
        increment_id = f"increment-{index}"

    if increment_id in seen_ids:
        raise SystemExit(f"Duplicate increment id '{increment_id}' in SDLC plan.")
    seen_ids.add(increment_id)

    change_command = str(entry.get("change_command", "")).strip()
    if not change_command:
        raise SystemExit(f"Increment '{increment_id}' must provide change_command.")

    def encode(value: str) -> str:
        return base64.b64encode(value.encode("utf-8")).decode("ascii")

    commit_message = str(entry.get("commit_message", "")).strip()
    pr_title = str(entry.get("pr_title", "")).strip()
    pr_body = str(entry.get("pr_body", "")).strip()
    labels = str(entry.get("labels", "")).strip()
    reviewers = str(entry.get("reviewers", "")).strip()
    draft_pr = str(entry.get("draft_pr", "")).strip()
    base_branch = str(entry.get("base_branch", "")).strip()
    branch_prefix = str(entry.get("branch_prefix", "")).strip()

    rows.append(
        "\t".join(
            [
                increment_id,
                encode(change_command),
                encode(commit_message),
                encode(pr_title),
                encode(pr_body),
                encode(labels),
                encode(reviewers),
                encode(draft_pr),
                encode(base_branch),
                encode(branch_prefix),
            ]
        )
    )

rows_path.write_text("\n".join(rows) + "\n", encoding="utf-8")
PY

decode_base64() {
  local raw="${1:-}"

  if [[ -z "$raw" ]]; then
    printf ''
    return 0
  fi

  printf '%s' "$raw" | tr -d '\r\n' | base64 --decode
}

declare -a increment_reports=()
has_failure="false"
while IFS=$'\t' read -r increment_id change_b64 commit_b64 title_b64 body_b64 labels_b64 reviewers_b64 draft_b64 base_b64 prefix_b64; do
  if [[ -z "$increment_id" ]]; then
    continue
  fi

  change_command="$(decode_base64 "$change_b64")"
  commit_message="$(decode_base64 "$commit_b64")"
  pr_title="$(decode_base64 "$title_b64")"
  pr_body="$(decode_base64 "$body_b64")"
  labels="$(decode_base64 "$labels_b64")"
  reviewers="$(decode_base64 "$reviewers_b64")"
  draft_pr="$(decode_base64 "$draft_b64")"
  base_branch="$(decode_base64 "$base_b64")"
  branch_prefix="$(decode_base64 "$prefix_b64")"

  if [[ -z "$commit_message" ]]; then
    commit_message="chore(sdlc): ${increment_id}"
  fi

  if [[ -z "$base_branch" ]]; then
    base_branch="$DEFAULT_BASE_BRANCH"
  fi

  if [[ -z "$branch_prefix" ]]; then
    branch_prefix="$DEFAULT_BRANCH_PREFIX"
  fi

  if [[ -z "$draft_pr" ]]; then
    draft_pr="$DEFAULT_DRAFT_PR"
  fi

  if [[ -z "$labels" ]]; then
    labels="$DEFAULT_LABELS"
  fi

  if [[ -z "$reviewers" ]]; then
    reviewers="$DEFAULT_REVIEWERS"
  fi

  increment_report_path="${OUTPUT_DIR}/increment-${increment_id}-${RUN_ID}-attempt-${RUN_ATTEMPT}.json"
  increment_reports+=("$increment_report_path")

  echo "Running SDLC increment '${increment_id}'..."

  export BANANA_TRIAGE_ID="${SDLC_ID}-${increment_id}"
  export BANANA_TRIAGE_CHANGE_COMMAND="$change_command"
  export BANANA_BASE_BRANCH="$base_branch"
  export BANANA_BRANCH_PREFIX="$branch_prefix"
  export BANANA_COMMIT_MESSAGE="$commit_message"
  export BANANA_PR_TITLE="$pr_title"
  export BANANA_PR_BODY="$pr_body"
  export BANANA_DRAFT_PR="$draft_pr"
  export BANANA_PR_LABELS="$labels"
  export BANANA_PR_REVIEWERS="$reviewers"
  export BANANA_ENABLE_AUTO_MERGE="$ENABLE_AUTO_MERGE"
  export BANANA_AUTO_MERGE_METHOD="$AUTO_MERGE_METHOD"
  export BANANA_SKIP_IF_NO_CHANGES="$SKIP_NO_CHANGES"
  export BANANA_REQUIRE_REAL_UPDATES="$SDLC_REQUIRE_REAL_UPDATES"
  export BANANA_SKIP_DOCS_ONLY_CHANGES="$SDLC_SKIP_NON_REAL_UPDATES"
  export BANANA_REAL_UPDATE_PATH_PATTERNS="$REAL_UPDATE_PATH_PATTERNS"
  export BANANA_PR_OUTPUT_PATH="$increment_report_path"

  increment_exit_code=0
  if command -v timeout >/dev/null 2>&1 && [[ "$INCREMENT_TIMEOUT_SECONDS" =~ ^[0-9]+$ ]] && [[ "$INCREMENT_TIMEOUT_SECONDS" -gt 0 ]]; then
    timeout --preserve-status "${INCREMENT_TIMEOUT_SECONDS}s" bash scripts/workflow-orchestrate-triaged-item-pr.sh || increment_exit_code=$?
  else
    bash scripts/workflow-orchestrate-triaged-item-pr.sh || increment_exit_code=$?
  fi

  if [[ "$increment_exit_code" -ne 0 ]]; then
    if [[ "$increment_exit_code" -eq 124 ]]; then
      echo "::warning::SDLC increment '${increment_id}' timed out after ${INCREMENT_TIMEOUT_SECONDS}s."
    fi

    python - "$increment_report_path" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
path.parent.mkdir(parents=True, exist_ok=True)
if not path.exists():
    payload = {"status": "failed", "reason": "increment command failed before PR output was generated"}
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
    has_failure="true"

    if [[ "$CONTINUE_ON_ERROR" != "true" ]]; then
      echo "::error::SDLC increment '${increment_id}' failed."
      break
    fi

    echo "::warning::SDLC increment '${increment_id}' failed but continue-on-error is enabled."
  fi

done < "$PLAN_ROWS_PATH"

if [[ "$ENABLE_WIKI_SYNC" == "true" ]]; then
  echo "Running wiki sync layer..."
  export BANANA_WIKI_DRY_RUN="$WIKI_DRY_RUN"
  export BANANA_WIKI_PUSH="$WIKI_PUSH"
  export BANANA_WIKI_STRICT="$WIKI_STRICT"
  export BANANA_WIKI_COMMIT_MESSAGE="$WIKI_COMMIT_MESSAGE"
  export BANANA_WIKI_REMOTE_URL="$WIKI_REMOTE_URL"
  export BANANA_ENFORCE_CANONICAL_WIKI_REMOTE="$WIKI_ENFORCE_CANONICAL_REMOTE"
  export BANANA_WIKI_OUTPUT_PATH="$WIKI_REPORT_PATH"
  bash scripts/workflow-sync-wiki.sh
fi

python - "$SUMMARY_PATH" "$OUTPUT_DIR" "$WIKI_REPORT_PATH" "$RUN_ID" "$RUN_ATTEMPT" <<'PY'
import json
import pathlib
import sys

summary_path = pathlib.Path(sys.argv[1])
output_dir = pathlib.Path(sys.argv[2])
wiki_report_path = pathlib.Path(sys.argv[3])
run_id = sys.argv[4]
run_attempt = sys.argv[5]

increment_reports = []
pattern = f"increment-*-{run_id}-attempt-{run_attempt}.json"
for report_path in sorted(output_dir.glob(pattern)):
    try:
        increment_reports.append(json.loads(report_path.read_text(encoding="utf-8")))
    except Exception:
        increment_reports.append({"status": "failed", "reason": f"unable to parse {report_path.name}"})

wiki_report = None
if wiki_report_path.exists():
    try:
        wiki_report = json.loads(wiki_report_path.read_text(encoding="utf-8"))
    except Exception:
        wiki_report = {"status": "failed", "reason": f"unable to parse {wiki_report_path.name}"}

status_counts: dict[str, int] = {}
for item in increment_reports:
    key = str(item.get("status", "unknown"))
    status_counts[key] = status_counts.get(key, 0) + 1

summary = {
    "increment_report_count": len(increment_reports),
    "increment_status_counts": status_counts,
    "increments": increment_reports,
    "wiki_sync": wiki_report,
}

summary_path.parent.mkdir(parents=True, exist_ok=True)
summary_path.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
print(json.dumps(summary, indent=2))
PY

echo "SDLC orchestration summary written to: ${SUMMARY_PATH}"

if [[ "$has_failure" == "true" && "$CONTINUE_ON_ERROR" != "true" ]]; then
  exit 1
fi

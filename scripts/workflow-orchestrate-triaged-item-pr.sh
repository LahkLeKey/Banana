#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

bash scripts/workflow-ensure-speckit.sh

RUN_ID="${GITHUB_RUN_ID:-local-run}"
RUN_ATTEMPT="${GITHUB_RUN_ATTEMPT:-1}"
TRIAGE_ID="${BANANA_TRIAGE_ID:-local-triage}"
CHANGE_COMMAND="${BANANA_TRIAGE_CHANGE_COMMAND:-:}"
BASE_BRANCH="${BANANA_BASE_BRANCH:-main}"
BRANCH_PREFIX="${BANANA_BRANCH_PREFIX:-triage}"
COMMIT_MESSAGE="${BANANA_COMMIT_MESSAGE:-chore(triage): automated triaged change}"
PR_TITLE_OVERRIDE="${BANANA_PR_TITLE:-}"
PR_BODY_OVERRIDE="${BANANA_PR_BODY:-}"
DRAFT_PR="${BANANA_DRAFT_PR:-true}"
ENABLE_AUTO_MERGE="${BANANA_ENABLE_AUTO_MERGE:-false}"
AUTO_MERGE_METHOD="${BANANA_AUTO_MERGE_METHOD:-squash}"
PR_LABELS="${BANANA_PR_LABELS:-automation,triaged-item,requires-human-approval,copilot-auto-approve,speckit-driven}"
PR_REVIEWERS="${BANANA_PR_REVIEWERS:-}"
LOCAL_DRY_RUN="${BANANA_LOCAL_DRY_RUN:-false}"
SKIP_IF_NO_CHANGES="${BANANA_SKIP_IF_NO_CHANGES:-false}"
REQUIRE_REAL_UPDATES="${BANANA_REQUIRE_REAL_UPDATES:-true}"
SKIP_NON_REAL_UPDATES="${BANANA_SKIP_DOCS_ONLY_CHANGES:-true}"
REAL_UPDATE_PATH_PATTERNS="${BANANA_REAL_UPDATE_PATH_PATTERNS:-src/**,tests/**,scripts/**,.github/workflows/**,docker/**,docker-compose.yml,CMakeLists.txt,Directory.Build.props}"
PR_OUTPUT_PATH="${BANANA_PR_OUTPUT_PATH:-}"
REQUIRED_HUMAN_REVIEWER="${BANANA_REQUIRED_HUMAN_REVIEWER:-LahkLeKey}"
RUN_API_PARITY_CHECK="${BANANA_TRIAGE_RUN_API_PARITY_CHECK:-false}"
API_PARITY_STRICT="${BANANA_TRIAGE_API_PARITY_STRICT:-false}"
AGENT_CONTRIBUTOR_OVERRIDE="${BANANA_AGENT_CONTRIBUTOR:-}"
AGENT_CONTRIBUTOR_LOGIN_OVERRIDE="${BANANA_AGENT_CONTRIBUTOR_LOGIN:-}"
AGENT_CONTRIBUTOR_NAME_OVERRIDE="${BANANA_AGENT_CONTRIBUTOR_NAME:-}"
AGENT_CONTRIBUTOR_EMAIL_OVERRIDE="${BANANA_AGENT_CONTRIBUTOR_EMAIL:-}"
AGENT_IDENTITY_REGISTRY_PATH="${BANANA_AGENT_IDENTITY_REGISTRY_PATH:-docs/automation/agent-pulse/agent-identities.json}"
AGENT_CONTRIBUTOR_SLUG=""
AGENT_CONTRIBUTOR_LOGIN=""
AGENT_CONTRIBUTOR_NAME=""
AGENT_CONTRIBUTOR_EMAIL=""
AGENT_CONTRIBUTOR_LABEL=""
AGENT_CONTRIBUTOR_ICON=""
AGENT_CONTRIBUTOR_PROFILE_URL=""
AGENT_CONTRIBUTOR_FOLLOWABLE="false"
AGENT_CONTRIBUTOR_CAN_ASSIGN="false"
AGENT_IDENTITY_DISPLAY_NAME=""
AGENT_IDENTITY_GITHUB_LOGIN=""
AGENT_IDENTITY_CONTRIBUTOR_NAME=""
AGENT_IDENTITY_CONTRIBUTOR_EMAIL=""
PR_AUTHOR_TOKEN_SOURCE="github-token"

write_pr_output() {
  local status="$1"
  local pr_url="$2"
  local pr_number="$3"
  local reason="$4"

  if [[ -z "$PR_OUTPUT_PATH" ]]; then
    return 0
  fi

  PR_OUTPUT_STATUS="$status" \
  PR_OUTPUT_URL="$pr_url" \
  PR_OUTPUT_NUMBER="$pr_number" \
  PR_OUTPUT_REASON="$reason" \
  PR_OUTPUT_BRANCH="$WORK_BRANCH" \
  PR_OUTPUT_TRIAGE_ID="$TRIAGE_ID" \
  PR_OUTPUT_BASE_BRANCH="$BASE_BRANCH" \
  PR_OUTPUT_AGENT_CONTRIBUTOR_NAME="$AGENT_CONTRIBUTOR_NAME" \
  PR_OUTPUT_AGENT_CONTRIBUTOR_EMAIL="$AGENT_CONTRIBUTOR_EMAIL" \
  PR_OUTPUT_AGENT_CONTRIBUTOR_SLUG="$AGENT_CONTRIBUTOR_SLUG" \
  PR_OUTPUT_AGENT_CONTRIBUTOR_LOGIN="$AGENT_CONTRIBUTOR_LOGIN" \
  PR_OUTPUT_AGENT_CONTRIBUTOR_LABEL="$AGENT_CONTRIBUTOR_LABEL" \
  PR_OUTPUT_AGENT_CONTRIBUTOR_ICON="$AGENT_CONTRIBUTOR_ICON" \
  PR_OUTPUT_AGENT_CONTRIBUTOR_PROFILE_URL="$AGENT_CONTRIBUTOR_PROFILE_URL" \
  PR_OUTPUT_AGENT_CONTRIBUTOR_FOLLOWABLE="$AGENT_CONTRIBUTOR_FOLLOWABLE" \
  PR_OUTPUT_REQUIRED_HUMAN_REVIEWER="$REQUIRED_HUMAN_REVIEWER" \
  PR_OUTPUT_AUTHOR_TOKEN_SOURCE="$PR_AUTHOR_TOKEN_SOURCE" \
  python - "$PR_OUTPUT_PATH" <<'PY'
import json
import os
import pathlib
import sys

output_path = pathlib.Path(sys.argv[1])
output_path.parent.mkdir(parents=True, exist_ok=True)

payload = {
    "status": os.environ.get("PR_OUTPUT_STATUS", ""),
    "triage_id": os.environ.get("PR_OUTPUT_TRIAGE_ID", ""),
    "base_branch": os.environ.get("PR_OUTPUT_BASE_BRANCH", ""),
    "branch": os.environ.get("PR_OUTPUT_BRANCH", ""),
    "pr_url": os.environ.get("PR_OUTPUT_URL", ""),
    "pr_number": os.environ.get("PR_OUTPUT_NUMBER", ""),
    "reason": os.environ.get("PR_OUTPUT_REASON", ""),
    "automation_contributor_name": os.environ.get("PR_OUTPUT_AGENT_CONTRIBUTOR_NAME", ""),
    "automation_contributor_email": os.environ.get("PR_OUTPUT_AGENT_CONTRIBUTOR_EMAIL", ""),
    "automation_contributor_slug": os.environ.get("PR_OUTPUT_AGENT_CONTRIBUTOR_SLUG", ""),
    "automation_contributor_login": os.environ.get("PR_OUTPUT_AGENT_CONTRIBUTOR_LOGIN", ""),
    "automation_contributor_label": os.environ.get("PR_OUTPUT_AGENT_CONTRIBUTOR_LABEL", ""),
    "automation_contributor_icon": os.environ.get("PR_OUTPUT_AGENT_CONTRIBUTOR_ICON", ""),
    "automation_contributor_profile_url": os.environ.get("PR_OUTPUT_AGENT_CONTRIBUTOR_PROFILE_URL", ""),
    "automation_contributor_followable": os.environ.get("PR_OUTPUT_AGENT_CONTRIBUTOR_FOLLOWABLE", ""),
    "required_human_reviewer": os.environ.get("PR_OUTPUT_REQUIRED_HUMAN_REVIEWER", ""),
    "author_token_source": os.environ.get("PR_OUTPUT_AUTHOR_TOKEN_SOURCE", ""),
}

output_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
}

parse_list_input() {
  local raw="${1:-}"
  local -n out_items="$2"
  local normalized
  local item
  local -a raw_items=()

  normalized="${raw//$'\r'/}"
  normalized="${normalized//$'\n'/,}"
  normalized="$(echo "$normalized" | xargs)"

  if [[ -z "$normalized" ]]; then
    return 0
  fi

  if [[ "$normalized" == \[*\] ]]; then
    normalized="${normalized#[}"
    normalized="${normalized%]}"
  fi

  IFS=',' read -r -a raw_items <<< "$normalized"
  for raw_item in "${raw_items[@]}"; do
    item="$(echo "$raw_item" | xargs)"
    item="${item#\"}"
    item="${item%\"}"
    item="${item#\'}"
    item="${item%\'}"
    item="$(echo "$item" | xargs)"
    if [[ -n "$item" ]]; then
      out_items+=("$item")
    fi
  done
}

collect_workspace_changed_files() {
  mapfile -t WORKSPACE_CHANGED_FILES < <(
    {
      git diff --name-only --diff-filter=ACMRTUXB
      git diff --cached --name-only --diff-filter=ACMRTUXB
      git ls-files --others --exclude-standard
    } | sed 's#\\#/#g' | awk 'NF && !seen[$0]++'
  )
}

collect_staged_changed_files() {
  mapfile -t STAGED_CHANGED_FILES < <(git diff --cached --name-only --diff-filter=ACMRTUXB | sed 's#\\#/#g')
}

evaluate_real_update_scope() {
  local source_label="$1"
  shift

  local -a changed_files=("$@")
  local -a patterns=()
  local -a matched_real_updates=()
  local changed_file
  local pattern

  if [[ "$REQUIRE_REAL_UPDATES" != "true" ]]; then
    return 0
  fi

  parse_list_input "$REAL_UPDATE_PATH_PATTERNS" patterns
  if [[ ${#patterns[@]} -eq 0 ]]; then
    echo "::error::BANANA_REAL_UPDATE_PATH_PATTERNS is empty while BANANA_REQUIRE_REAL_UPDATES=true."
    exit 1
  fi

  for changed_file in "${changed_files[@]}"; do
    for pattern in "${patterns[@]}"; do
      if [[ "$changed_file" == $pattern ]]; then
        matched_real_updates+=("$changed_file")
        break
      fi
    done
  done

  if [[ ${#matched_real_updates[@]} -gt 0 ]]; then
    echo "Real-update scope check passed (${source_label}): ${#matched_real_updates[@]} matching files."
    return 0
  fi

  echo "Changed files (${source_label}) did not match required real-update path patterns."
  if [[ ${#changed_files[@]} -eq 0 ]]; then
    echo "  (no changed files detected)"
  else
    printf '  - %s\n' "${changed_files[@]}"
  fi

  if [[ "$SKIP_NON_REAL_UPDATES" == "true" ]]; then
    echo "Skipping PR creation by policy (non-impact/doc-only changes)."
    write_pr_output "skipped" "" "" "non-impact/doc-only changes"
    exit 0
  fi

  echo "::error::Change set did not touch required source/workflow paths."
  exit 1
}

slugify_agent_contributor() {
  local raw="${1:-}"
  local slug

  slug="$(printf '%s' "$raw" | tr '[:upper:]' '[:lower:]' | sed -E 's/^agent://; s/[^a-z0-9._-]+/-/g; s/^-+//; s/-+$//')"

  if [[ -z "$slug" ]]; then
    slug="workflow-agent"
  fi

  printf '%s' "$slug"
}

append_csv_item_if_missing() {
  local csv="${1:-}"
  local item="${2:-}"
  local -a parsed_items=()
  local normalized_item
  local existing

  normalized_item="$(echo "$item" | xargs)"
  if [[ -z "$normalized_item" ]]; then
    printf '%s' "$csv"
    return 0
  fi

  parse_list_input "$csv" parsed_items
  for existing in "${parsed_items[@]}"; do
    if [[ "${existing,,}" == "${normalized_item,,}" ]]; then
      printf '%s' "$csv"
      return 0
    fi
  done

  if [[ -z "$csv" ]]; then
    printf '%s' "$normalized_item"
  else
    printf '%s,%s' "$csv" "$normalized_item"
  fi
}

load_agent_identity_profile() {
  local slug="$1"
  local serialized

  AGENT_IDENTITY_DISPLAY_NAME=""
  AGENT_IDENTITY_GITHUB_LOGIN=""
  AGENT_IDENTITY_CONTRIBUTOR_NAME=""
  AGENT_IDENTITY_CONTRIBUTOR_EMAIL=""
  AGENT_CONTRIBUTOR_ICON=""
  AGENT_CONTRIBUTOR_PROFILE_URL=""
  AGENT_CONTRIBUTOR_FOLLOWABLE="false"

  if [[ ! -f "$AGENT_IDENTITY_REGISTRY_PATH" ]]; then
    return 0
  fi

  serialized="$(python - "$AGENT_IDENTITY_REGISTRY_PATH" "$slug" <<'PY'
import json
import pathlib
import shlex
import sys

registry_path = pathlib.Path(sys.argv[1])
agent_slug = sys.argv[2]
entry = {}

try:
    payload = json.loads(registry_path.read_text(encoding="utf-8"))
except Exception:
    payload = {}

if not isinstance(payload, dict):
  payload = {}

agents = payload.get("agents", [])
if not isinstance(agents, list):
  agents = []

for candidate in agents:
    if not isinstance(candidate, dict):
        continue
    if str(candidate.get("slug", "")).strip() == agent_slug:
        entry = candidate
        break

def clean(value: object) -> str:
  return str(value or "").replace("\n", " ").strip()

values = {
  "AGENT_IDENTITY_DISPLAY_NAME": clean(entry.get("display_name", "")),
  "AGENT_CONTRIBUTOR_ICON": clean(entry.get("icon", "")),
  "AGENT_IDENTITY_GITHUB_LOGIN": clean(entry.get("github_login", "")),
  "AGENT_CONTRIBUTOR_PROFILE_URL": clean(entry.get("follow_url", "")),
  "AGENT_CONTRIBUTOR_FOLLOWABLE": clean(entry.get("followable", "false")).lower(),
  "AGENT_IDENTITY_CONTRIBUTOR_NAME": clean(entry.get("contributor_name", "")),
  "AGENT_IDENTITY_CONTRIBUTOR_EMAIL": clean(entry.get("contributor_email", "")),
}

if values["AGENT_CONTRIBUTOR_FOLLOWABLE"] not in {"true", "false"}:
  values["AGENT_CONTRIBUTOR_FOLLOWABLE"] = "false"

for key, value in values.items():
  print(f"{key}={shlex.quote(value)}")
PY
)"

  eval "$serialized"
}

resolve_agent_contributor() {
  local candidate="${AGENT_CONTRIBUTOR_OVERRIDE:-}"
  local label
  local -a parsed_labels=()

  if [[ -z "$candidate" ]]; then
    parse_list_input "${PR_LABELS:-}" parsed_labels
    for label in "${parsed_labels[@]}"; do
      if [[ "$label" == agent:* ]]; then
        candidate="${label#agent:}"
        break
      fi
    done
  fi

  if [[ -z "$candidate" ]]; then
    candidate="workflow-agent"
  fi

  AGENT_CONTRIBUTOR_SLUG="$(slugify_agent_contributor "$candidate")"
  load_agent_identity_profile "$AGENT_CONTRIBUTOR_SLUG"

  if [[ -n "$AGENT_CONTRIBUTOR_LOGIN_OVERRIDE" ]]; then
    AGENT_CONTRIBUTOR_LOGIN="$AGENT_CONTRIBUTOR_LOGIN_OVERRIDE"
  elif [[ -n "$AGENT_IDENTITY_GITHUB_LOGIN" ]]; then
    AGENT_CONTRIBUTOR_LOGIN="$AGENT_IDENTITY_GITHUB_LOGIN"
  else
    AGENT_CONTRIBUTOR_LOGIN="$AGENT_CONTRIBUTOR_SLUG"
  fi

  AGENT_CONTRIBUTOR_LABEL="contributor:${AGENT_CONTRIBUTOR_SLUG}"

  if [[ -n "$AGENT_CONTRIBUTOR_NAME_OVERRIDE" ]]; then
    AGENT_CONTRIBUTOR_NAME="$AGENT_CONTRIBUTOR_NAME_OVERRIDE"
  elif [[ -n "$AGENT_IDENTITY_CONTRIBUTOR_NAME" ]]; then
    AGENT_CONTRIBUTOR_NAME="$AGENT_IDENTITY_CONTRIBUTOR_NAME"
  elif [[ -n "$AGENT_IDENTITY_DISPLAY_NAME" && -n "$AGENT_CONTRIBUTOR_ICON" ]]; then
    AGENT_CONTRIBUTOR_NAME="${AGENT_CONTRIBUTOR_ICON} ${AGENT_IDENTITY_DISPLAY_NAME}"
  elif [[ -n "$AGENT_IDENTITY_DISPLAY_NAME" ]]; then
    AGENT_CONTRIBUTOR_NAME="$AGENT_IDENTITY_DISPLAY_NAME"
  else
    AGENT_CONTRIBUTOR_NAME="$AGENT_CONTRIBUTOR_LOGIN"
  fi

  if [[ -n "$AGENT_CONTRIBUTOR_EMAIL_OVERRIDE" ]]; then
    AGENT_CONTRIBUTOR_EMAIL="$AGENT_CONTRIBUTOR_EMAIL_OVERRIDE"
  elif [[ -n "$AGENT_IDENTITY_CONTRIBUTOR_EMAIL" ]]; then
    AGENT_CONTRIBUTOR_EMAIL="$AGENT_IDENTITY_CONTRIBUTOR_EMAIL"
  else
    AGENT_CONTRIBUTOR_EMAIL="${AGENT_CONTRIBUTOR_LOGIN}@users.noreply.github.com"
  fi

  if [[ -z "$AGENT_CONTRIBUTOR_PROFILE_URL" ]]; then
    AGENT_CONTRIBUTOR_PROFILE_URL="docs/automation/agent-pulse/agents/${AGENT_CONTRIBUTOR_SLUG}/"
  fi

  if [[ -n "$AGENT_CONTRIBUTOR_LOGIN_OVERRIDE" ]]; then
    AGENT_CONTRIBUTOR_CAN_ASSIGN="true"
  elif [[ "$AGENT_CONTRIBUTOR_FOLLOWABLE" == "true" && -n "$AGENT_CONTRIBUTOR_LOGIN" ]]; then
    AGENT_CONTRIBUTOR_CAN_ASSIGN="true"
  else
    AGENT_CONTRIBUTOR_CAN_ASSIGN="false"
  fi

  PR_LABELS="$(append_csv_item_if_missing "$PR_LABELS" "$AGENT_CONTRIBUTOR_LABEL")"
}

declare -a parsed_reviewers=()
parse_list_input "${PR_REVIEWERS:-}" parsed_reviewers

resolve_agent_contributor

if [[ -n "$REQUIRED_HUMAN_REVIEWER" ]]; then
  parsed_reviewers+=("$REQUIRED_HUMAN_REVIEWER")
fi

PARSED_REVIEWERS_CSV="$(IFS=','; echo "${parsed_reviewers[*]:-}")"

echo "Using automation contributor: ${AGENT_CONTRIBUTOR_NAME} <${AGENT_CONTRIBUTOR_EMAIL}> (icon=${AGENT_CONTRIBUTOR_ICON:-n/a}, follow=${AGENT_CONTRIBUTOR_PROFILE_URL})"
if [[ "$AGENT_CONTRIBUTOR_FOLLOWABLE" != "true" ]]; then
  echo "::notice::Pulse will show a default avatar for '${AGENT_CONTRIBUTOR_SLUG}' until a real GitHub login/avatar is mapped in docs/automation/agent-pulse/agent-identities.json."
fi

TRIAGE_ID_SLUG="$(echo "$TRIAGE_ID" | tr '[:upper:]' '[:lower:]' | sed -E 's/[^a-z0-9._-]+/-/g; s/^-+//; s/-+$//')"
if [[ -z "$TRIAGE_ID_SLUG" ]]; then
  echo "::error::triage_id does not produce a valid branch slug."
  exit 1
fi

BRANCH_PREFIX="${BRANCH_PREFIX%/}"
WORK_BRANCH="${BRANCH_PREFIX}/${TRIAGE_ID_SLUG}-run-${RUN_ID}-attempt-${RUN_ATTEMPT}"

echo "Applying triaged code changes..."
eval "$CHANGE_COMMAND"

if [[ "$RUN_API_PARITY_CHECK" == "true" ]]; then
  echo "Running API parity governance preflight for triaged change..."
  parity_args=()
  if [[ "$API_PARITY_STRICT" == "true" ]]; then
    parity_args+=(--strict)
  else
    parity_args+=(--inventory-only)
  fi
  bash scripts/validate-api-parity-governance.sh "${parity_args[@]}"
fi

collect_workspace_changed_files
evaluate_real_update_scope "workspace" "${WORKSPACE_CHANGED_FILES[@]}"

if [[ "$LOCAL_DRY_RUN" == "true" ]]; then
  PLAN_DIR="artifacts/local-workflow-dry-run/triaged-pr"
  mkdir -p "$PLAN_DIR"

  CHANGED_FILES="$(printf '%s\n' "${WORKSPACE_CHANGED_FILES[@]}" | paste -sd ',' -)"
  if [[ -z "$CHANGED_FILES" ]]; then
    CHANGED_FILES=""
    echo "Dry-run mode: no repository changes produced by change command."
  fi

  PLAN_FILE="$PLAN_DIR/plan-${TRIAGE_ID_SLUG}-${RUN_ID}-attempt-${RUN_ATTEMPT}.json"
  cat > "$PLAN_FILE" <<EOF
{
  "mode": "dry-run",
  "triage_id": "${TRIAGE_ID}",
  "run_id": "${RUN_ID}",
  "run_attempt": "${RUN_ATTEMPT}",
  "change_command": "${CHANGE_COMMAND}",
  "target_branch": "${WORK_BRANCH}",
  "base_branch": "${BASE_BRANCH}",
  "commit_message": "${COMMIT_MESSAGE}",
  "labels": "${PR_LABELS}",
  "author_token_source": "${PR_AUTHOR_TOKEN_SOURCE}",
  "automation_contributor_name": "${AGENT_CONTRIBUTOR_NAME}",
  "automation_contributor_email": "${AGENT_CONTRIBUTOR_EMAIL}",
  "automation_contributor_login": "${AGENT_CONTRIBUTOR_LOGIN}",
  "automation_contributor_label": "${AGENT_CONTRIBUTOR_LABEL}",
  "automation_contributor_icon": "${AGENT_CONTRIBUTOR_ICON}",
  "automation_contributor_profile_url": "${AGENT_CONTRIBUTOR_PROFILE_URL}",
  "automation_contributor_followable": "${AGENT_CONTRIBUTOR_FOLLOWABLE}",
  "required_human_reviewer": "${REQUIRED_HUMAN_REVIEWER}",
  "reviewers": "${PARSED_REVIEWERS_CSV}",
  "open_draft": "${DRAFT_PR}",
  "changed_files_csv": "${CHANGED_FILES}",
  "simulated_pr_url": "https://example.invalid/${WORK_BRANCH}"
}
EOF

  write_pr_output "dry-run" "https://example.invalid/${WORK_BRANCH}" "" "dry-run"
  echo "Dry-run triaged PR plan generated: $PLAN_FILE"
  exit 0
fi

if ! command -v gh >/dev/null 2>&1; then
  echo "::error::GitHub CLI is required to orchestrate triaged pull requests."
  exit 1
fi

git fetch origin "$BASE_BRANCH"
git checkout -B "$WORK_BRANCH" "origin/$BASE_BRANCH"

git add -A
collect_staged_changed_files

if git diff --cached --quiet; then
  if [[ "$SKIP_IF_NO_CHANGES" == "true" ]]; then
    echo "No file changes were produced by change_command. Skipping PR creation by policy."
    write_pr_output "skipped" "" "" "no file changes"
    exit 0
  fi

  echo "::error::No file changes were produced by change_command."
  exit 1
fi

evaluate_real_update_scope "staged" "${STAGED_CHANGED_FILES[@]}"

git config user.name "$AGENT_CONTRIBUTOR_NAME"
git config user.email "$AGENT_CONTRIBUTOR_EMAIL"
git commit -m "$COMMIT_MESSAGE"
git push -u origin "$WORK_BRANCH"

DEFAULT_PR_TITLE="triage(${TRIAGE_ID}): automated changes requiring human approval"
if [[ -n "$PR_TITLE_OVERRIDE" ]]; then
  PR_TITLE="$PR_TITLE_OVERRIDE"
else
  PR_TITLE="$DEFAULT_PR_TITLE"
fi

DEFAULT_PR_BODY=$(cat <<EOF
This pull request was orchestrated for a triaged item and requires human approval before merge.

- Triage item: ${TRIAGE_ID}
- Run ID: ${RUN_ID}
- Attempt: ${RUN_ATTEMPT}
- Source branch: ${WORK_BRANCH}
- Automation contributor: ${AGENT_CONTRIBUTOR_NAME} <${AGENT_CONTRIBUTOR_EMAIL}>
- Automation contributor login: ${AGENT_CONTRIBUTOR_LOGIN}
- Automation contributor icon: ${AGENT_CONTRIBUTOR_ICON}
- Automation contributor profile: ${AGENT_CONTRIBUTOR_PROFILE_URL}
- Automation contributor followable: ${AGENT_CONTRIBUTOR_FOLLOWABLE}
- Required human reviewer: ${REQUIRED_HUMAN_REVIEWER}

Command executed:
${CHANGE_COMMAND}
EOF
)

if [[ -n "$PR_BODY_OVERRIDE" ]]; then
  PR_BODY="$PR_BODY_OVERRIDE"
  if [[ "$PR_BODY" != *"Automation contributor:"* ]]; then
    PR_BODY="${PR_BODY}

- Automation contributor: ${AGENT_CONTRIBUTOR_NAME} <${AGENT_CONTRIBUTOR_EMAIL}>"
  fi
  if [[ "$PR_BODY" != *"Automation contributor login:"* ]]; then
    PR_BODY="${PR_BODY}

- Automation contributor login: ${AGENT_CONTRIBUTOR_LOGIN}"
  fi
  if [[ "$PR_BODY" != *"Automation contributor icon:"* ]]; then
    PR_BODY="${PR_BODY}

- Automation contributor icon: ${AGENT_CONTRIBUTOR_ICON}"
  fi
  if [[ "$PR_BODY" != *"Automation contributor profile:"* ]]; then
    PR_BODY="${PR_BODY}

- Automation contributor profile: ${AGENT_CONTRIBUTOR_PROFILE_URL}"
  fi
  if [[ "$PR_BODY" != *"Automation contributor followable:"* ]]; then
    PR_BODY="${PR_BODY}

- Automation contributor followable: ${AGENT_CONTRIBUTOR_FOLLOWABLE}"
  fi
  if [[ "$PR_BODY" != *"Required human reviewer:"* ]]; then
    PR_BODY="${PR_BODY}

- Required human reviewer: ${REQUIRED_HUMAN_REVIEWER}"
  fi
else
  PR_BODY="$DEFAULT_PR_BODY"
fi

CREATE_ARGS=(
  --base "$BASE_BRANCH"
  --head "$WORK_BRANCH"
  --title "$PR_TITLE"
  --body "$PR_BODY"
)
if [[ "$DRAFT_PR" == "true" ]]; then
  CREATE_ARGS+=(--draft)
fi

pr_url="$(gh pr create "${CREATE_ARGS[@]}")"
pr_number="${pr_url##*/}"

IFS=',' read -r -a label_items <<< "$PR_LABELS"
for raw_label in "${label_items[@]}"; do
  label="$(echo "$raw_label" | xargs)"
  if [[ -z "$label" ]]; then
    continue
  fi
  if ! gh pr edit "$pr_number" --add-label "$label" >/dev/null 2>&1; then
    echo "::warning::Failed to apply label '$label' to PR #$pr_number."
  fi
done

if [[ "$AGENT_CONTRIBUTOR_CAN_ASSIGN" == "true" && -n "$AGENT_CONTRIBUTOR_LOGIN" ]]; then
  if ! gh pr edit "$pr_number" --add-assignee "$AGENT_CONTRIBUTOR_LOGIN" >/dev/null 2>&1; then
    echo "::warning::Failed to assign contributor '$AGENT_CONTRIBUTOR_LOGIN' to PR #$pr_number."
  fi
fi

if [[ ${#parsed_reviewers[@]} -gt 0 ]]; then
  declare -A seen_reviewers=()
  for reviewer in "${parsed_reviewers[@]}"; do
    reviewer_key="$(echo "$reviewer" | tr '[:upper:]' '[:lower:]')"
    if [[ -n "${seen_reviewers[$reviewer_key]:-}" ]]; then
      continue
    fi
    seen_reviewers[$reviewer_key]=1

    if ! gh pr edit "$pr_number" --add-reviewer "$reviewer" >/dev/null 2>&1; then
      echo "::warning::Failed to request reviewer '$reviewer' for PR #$pr_number."
    fi
  done
fi

if [[ "$ENABLE_AUTO_MERGE" == "true" ]]; then
  merge_method_flag="--squash"
  case "$AUTO_MERGE_METHOD" in
    merge) merge_method_flag="--merge" ;;
    rebase) merge_method_flag="--rebase" ;;
    squash|*) merge_method_flag="--squash" ;;
  esac

  if ! gh pr merge "$pr_number" --auto "$merge_method_flag" >/dev/null 2>&1; then
    echo "::warning::Failed to enable auto-merge for PR #$pr_number."
  fi
fi

echo "Triaged PR ready for review: $pr_url"
write_pr_output "created" "$pr_url" "$pr_number" ""

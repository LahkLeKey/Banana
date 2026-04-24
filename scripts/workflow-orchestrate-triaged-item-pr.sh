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
PR_LABELS="${BANANA_PR_LABELS:-automation,triaged-item,requires-human-approval,copilot-auto-approve,speckit-driven}"
PR_REVIEWERS="${BANANA_PR_REVIEWERS:-}"
LOCAL_DRY_RUN="${BANANA_LOCAL_DRY_RUN:-false}"
SKIP_IF_NO_CHANGES="${BANANA_SKIP_IF_NO_CHANGES:-false}"
PR_OUTPUT_PATH="${BANANA_PR_OUTPUT_PATH:-}"
REQUIRED_HUMAN_REVIEWER="${BANANA_REQUIRED_HUMAN_REVIEWER:-LahkLeKey}"
AGENT_CONTRIBUTOR_OVERRIDE="${BANANA_AGENT_CONTRIBUTOR:-}"
AGENT_CONTRIBUTOR_LOGIN_OVERRIDE="${BANANA_AGENT_CONTRIBUTOR_LOGIN:-}"
AGENT_CONTRIBUTOR_NAME_OVERRIDE="${BANANA_AGENT_CONTRIBUTOR_NAME:-}"
AGENT_CONTRIBUTOR_EMAIL_OVERRIDE="${BANANA_AGENT_CONTRIBUTOR_EMAIL:-}"
AGENT_CONTRIBUTOR_SLUG=""
AGENT_CONTRIBUTOR_LOGIN=""
AGENT_CONTRIBUTOR_NAME=""
AGENT_CONTRIBUTOR_EMAIL=""
AGENT_CONTRIBUTOR_LABEL=""
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
  AGENT_CONTRIBUTOR_LOGIN="${AGENT_CONTRIBUTOR_LOGIN_OVERRIDE:-$AGENT_CONTRIBUTOR_SLUG}"
  AGENT_CONTRIBUTOR_LABEL="contributor:${AGENT_CONTRIBUTOR_SLUG}"

  AGENT_CONTRIBUTOR_NAME="${AGENT_CONTRIBUTOR_NAME_OVERRIDE:-$AGENT_CONTRIBUTOR_LOGIN}"
  AGENT_CONTRIBUTOR_EMAIL="${AGENT_CONTRIBUTOR_EMAIL_OVERRIDE:-${AGENT_CONTRIBUTOR_LOGIN}@users.noreply.github.com}"

  PR_LABELS="$(append_csv_item_if_missing "$PR_LABELS" "$AGENT_CONTRIBUTOR_LABEL")"
}

declare -a parsed_reviewers=()
parse_list_input "${PR_REVIEWERS:-}" parsed_reviewers

resolve_agent_contributor

if [[ -n "$REQUIRED_HUMAN_REVIEWER" ]]; then
  parsed_reviewers+=("$REQUIRED_HUMAN_REVIEWER")
fi

PARSED_REVIEWERS_CSV="$(IFS=','; echo "${parsed_reviewers[*]:-}")"

echo "Using automation contributor: ${AGENT_CONTRIBUTOR_NAME} <${AGENT_CONTRIBUTOR_EMAIL}>"

TRIAGE_ID_SLUG="$(echo "$TRIAGE_ID" | tr '[:upper:]' '[:lower:]' | sed -E 's/[^a-z0-9._-]+/-/g; s/^-+//; s/-+$//')"
if [[ -z "$TRIAGE_ID_SLUG" ]]; then
  echo "::error::triage_id does not produce a valid branch slug."
  exit 1
fi

BRANCH_PREFIX="${BRANCH_PREFIX%/}"
WORK_BRANCH="${BRANCH_PREFIX}/${TRIAGE_ID_SLUG}-run-${RUN_ID}-attempt-${RUN_ATTEMPT}"

echo "Applying triaged code changes..."
eval "$CHANGE_COMMAND"

if [[ "$LOCAL_DRY_RUN" == "true" ]]; then
  PLAN_DIR="artifacts/local-workflow-dry-run/triaged-pr"
  mkdir -p "$PLAN_DIR"

  CHANGED_FILES="$(git status --porcelain --untracked-files=all | awk '{print $2}' | paste -sd ',' -)"
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
if git diff --cached --quiet; then
  if [[ "$SKIP_IF_NO_CHANGES" == "true" ]]; then
    echo "No file changes were produced by change_command. Skipping PR creation by policy."
    write_pr_output "skipped" "" "" "no file changes"
    exit 0
  fi

  echo "::error::No file changes were produced by change_command."
  exit 1
fi

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

if [[ -n "$AGENT_CONTRIBUTOR_LOGIN" ]]; then
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

echo "Triaged PR ready for review: $pr_url"
write_pr_output "created" "$pr_url" "$pr_number" ""

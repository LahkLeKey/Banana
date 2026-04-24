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

declare -a parsed_reviewers=()
parse_list_input "${PR_REVIEWERS:-}" parsed_reviewers
PARSED_REVIEWERS_CSV="$(IFS=','; echo "${parsed_reviewers[*]:-}")"

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

git config user.name "github-actions[bot]"
git config user.email "41898282+github-actions[bot]@users.noreply.github.com"
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

Command executed:
${CHANGE_COMMAND}
EOF
)

if [[ -n "$PR_BODY_OVERRIDE" ]]; then
  PR_BODY="$PR_BODY_OVERRIDE"
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

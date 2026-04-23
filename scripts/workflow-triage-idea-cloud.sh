#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

RUN_ID="${GITHUB_RUN_ID:-local-run}"
RUN_ATTEMPT="${GITHUB_RUN_ATTEMPT:-1}"

TRIAGE_IDEA="${BANANA_TRIAGE_IDEA:-}"
TRIAGE_ISSUE_NUMBER="${BANANA_TRIAGE_ISSUE_NUMBER:-}"
TRIAGE_ISSUE_LABELS="${BANANA_TRIAGE_ISSUE_LABELS:-triage-idea,copilot-suggestion,ai-generated,automation,copilot-bypass-vibe-coded}"
TRIAGE_PR_LABELS="${BANANA_TRIAGE_PR_LABELS:-automation,triaged-item,copilot-auto-approve,copilot-autonomous-cycle,copilot-bypass-vibe-coded}"
TRIAGE_INTAKE_DIR="${BANANA_TRIAGE_INTAKE_DIR:-docs/triage/intake}"
BASE_BRANCH="${BANANA_BASE_BRANCH:-main}"
BRANCH_PREFIX="${BANANA_BRANCH_PREFIX:-triage}"
DRAFT_PR="${BANANA_DRAFT_PR:-false}"
TRIAGE_CLEAR_BACKLOG="${BANANA_TRIAGE_CLEAR_BACKLOG:-true}"
TRIAGE_BACKLOG_ISSUE_LABELS="${BANANA_TRIAGE_BACKLOG_ISSUE_LABELS:-copilot-suggestion,ai-generated}"
TRIAGE_BACKLOG_PR_LABELS="${BANANA_TRIAGE_BACKLOG_PR_LABELS:-automation,triaged-item}"
TRIAGE_BACKLOG_PR_BRANCH_PREFIXES="${BANANA_TRIAGE_BACKLOG_PR_BRANCH_PREFIXES:-triage/,triage-copilot/,triage-feedback/}"
TRIAGE_DISPATCH_REQUIRED_CHECKS="${BANANA_TRIAGE_DISPATCH_REQUIRED_CHECKS:-true}"
TRIAGE_CHECK_WORKFLOWS="${BANANA_TRIAGE_CHECK_WORKFLOWS:-copilot-review-triage.yml,require-human-approval.yml}"
PR_REVIEWERS="${BANANA_PR_REVIEWERS:-}"
SKIP_IF_NO_CHANGES="${BANANA_SKIP_IF_NO_CHANGES:-true}"
PR_OUTPUT_PATH="${BANANA_PR_OUTPUT_PATH:-artifacts/triage-idea/triage-pr-output-${RUN_ID}-attempt-${RUN_ATTEMPT}.json}"
HUMAN_TRIAGE_LABEL="human-triage"
SOURCE_HUMAN_ISSUE_LABEL="source-human-issue"
AI_GENERATED_LABEL="ai-generated"
SOURCE_AI_ISSUE_LABEL="source-ai-issue"

mkdir -p "$(dirname "$PR_OUTPUT_PATH")"

if ! command -v gh >/dev/null 2>&1; then
  echo "::error::GitHub CLI is required to orchestrate triage ideas in cloud workflows."
  exit 1
fi

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

extract_issue_marker() {
  local body="${1:-}"
  local marker="$2"

  printf '%s\n' "$body" \
    | sed -n -E "s/.*<!--[[:space:]]*${marker}:([^[:space:]]+)[[:space:]]*-->.*/\\1/p" \
    | head -n 1
}

sanitize_agent_name() {
  local raw="${1:-}"
  local normalized

  normalized="$(printf '%s' "$raw" | tr '[:upper:]' '[:lower:]' | sed -E 's/[^a-z0-9-]+/-/g; s/^-+//; s/-+$//')"
  printf '%s' "$normalized"
}

normalize_whitespace() {
  local raw="${1:-}"
  printf '%s' "$raw" | tr '\r\n' '  ' | sed -E 's/[[:space:]]+/ /g' | xargs
}

get_repo_slug() {
  if [[ -n "${GITHUB_REPOSITORY:-}" ]]; then
    printf '%s' "$GITHUB_REPOSITORY"
    return 0
  fi

  gh repo view --json nameWithOwner -q .nameWithOwner
}

dispatch_gate_workflow() {
  local repo_slug="$1"
  local workflow_file="$2"
  local pr_number="$3"
  local pr_branch="$4"

  if [[ -z "$workflow_file" ]]; then
    return 0
  fi

  if gh api -X POST "repos/${repo_slug}/actions/workflows/${workflow_file}/dispatches" \
    -f ref="$pr_branch" \
    -f inputs[pull_number]="$pr_number" >/dev/null 2>&1; then
    echo "Dispatched gate workflow '${workflow_file}' for PR #${pr_number} (${pr_branch})."
    return 0
  fi

  echo "::warning::Unable to dispatch gate workflow '${workflow_file}' for PR #${pr_number}."
  return 1
}

close_backlog_pull_requests() {
  declare -A seen_pr_numbers=()
  local -a parsed_pr_labels=()
  local -a parsed_pr_prefixes=()
  local -a backlog_pr_numbers=()
  local -a label_filtered_numbers=()
  local -a prefix_filtered_rows=()

  parse_list_input "$TRIAGE_BACKLOG_PR_LABELS" parsed_pr_labels
  parse_list_input "$TRIAGE_BACKLOG_PR_BRANCH_PREFIXES" parsed_pr_prefixes

  if [[ ${#parsed_pr_labels[@]} -gt 0 ]]; then
    local -a pr_label_query=(gh pr list --state open --limit 200 --json number)
    for label in "${parsed_pr_labels[@]}"; do
      pr_label_query+=(--label "$label")
    done

    while IFS= read -r pr_number; do
      if [[ -n "$pr_number" ]]; then
        label_filtered_numbers+=("$pr_number")
      fi
    done < <("${pr_label_query[@]}" --jq '.[].number')
  fi

  while IFS= read -r pr_number; do
    if [[ -n "$pr_number" ]]; then
      seen_pr_numbers["$pr_number"]=1
    fi
  done < <(printf '%s\n' "${label_filtered_numbers[@]:-}")

  if [[ ${#parsed_pr_prefixes[@]} -gt 0 ]]; then
    while IFS= read -r row; do
      if [[ -n "$row" ]]; then
        prefix_filtered_rows+=("$row")
      fi
    done < <(gh pr list --state open --limit 200 --json number,headRefName --jq '.[] | "\(.number)\t\(.headRefName)"')

    for row in "${prefix_filtered_rows[@]}"; do
      pr_number="${row%%$'\t'*}"
      head_ref="${row#*$'\t'}"

      for prefix in "${parsed_pr_prefixes[@]}"; do
        if [[ "$head_ref" == "$prefix"* ]]; then
          seen_pr_numbers["$pr_number"]=1
          break
        fi
      done
    done
  fi

  for pr_number in "${!seen_pr_numbers[@]}"; do
    backlog_pr_numbers+=("$pr_number")
  done

  if [[ ${#backlog_pr_numbers[@]} -gt 1 ]]; then
    IFS=$'\n' backlog_pr_numbers=($(printf '%s\n' "${backlog_pr_numbers[@]}" | sort -n))
    unset IFS
  fi

  if [[ ${#backlog_pr_numbers[@]} -eq 0 ]]; then
    echo "No backlog triage pull requests found for cleanup."
    return 0
  fi

  echo "Closing ${#backlog_pr_numbers[@]} backlog triage pull request(s)..."
  for pr_number in "${backlog_pr_numbers[@]}"; do
    if gh pr close "$pr_number" --comment "Closed automatically by triage backlog cleanup before run ${RUN_ID} attempt ${RUN_ATTEMPT}." >/dev/null 2>&1; then
      echo "Closed backlog PR #${pr_number}."
    else
      echo "::warning::Unable to close backlog PR #${pr_number}."
    fi
  done
}

close_backlog_issues() {
  local keep_issue_number="${1:-}"
  declare -A seen_issue_numbers=()
  local -a parsed_issue_backlog_labels=()
  local -a backlog_issue_numbers=()
  local -a label_issue_numbers=()

  parse_list_input "$TRIAGE_BACKLOG_ISSUE_LABELS" parsed_issue_backlog_labels

  for label in "${parsed_issue_backlog_labels[@]}"; do
    while IFS= read -r issue_number; do
      if [[ -n "$issue_number" ]]; then
        label_issue_numbers+=("$issue_number")
      fi
    done < <(gh issue list --state open --limit 200 --label "$label" --json number --jq '.[].number')
  done

  for issue_number in "${label_issue_numbers[@]}"; do
    if [[ -n "$issue_number" ]]; then
      seen_issue_numbers["$issue_number"]=1
    fi
  done

  for issue_number in "${!seen_issue_numbers[@]}"; do
    if [[ -n "$keep_issue_number" && "$issue_number" == "$keep_issue_number" ]]; then
      continue
    fi
    backlog_issue_numbers+=("$issue_number")
  done

  if [[ ${#backlog_issue_numbers[@]} -gt 1 ]]; then
    IFS=$'\n' backlog_issue_numbers=($(printf '%s\n' "${backlog_issue_numbers[@]}" | sort -n))
    unset IFS
  fi

  if [[ ${#backlog_issue_numbers[@]} -eq 0 ]]; then
    echo "No backlog triage issues found for cleanup."
    return 0
  fi

  echo "Closing ${#backlog_issue_numbers[@]} backlog triage issue(s)..."
  for issue_number in "${backlog_issue_numbers[@]}"; do
    if gh issue close "$issue_number" --comment "Closed automatically by triage backlog cleanup before run ${RUN_ID} attempt ${RUN_ATTEMPT}." >/dev/null 2>&1; then
      echo "Closed backlog issue #${issue_number}."
    else
      echo "::warning::Unable to close backlog issue #${issue_number}."
    fi
  done
}

clear_triage_backlog_if_enabled() {
  local keep_issue_number="${1:-}"

  if [[ "$TRIAGE_CLEAR_BACKLOG" != "true" ]]; then
    echo "Backlog cleanup disabled for this run."
    return 0
  fi

  echo "Clearing triage backlog before creating new issue/PR artifacts..."
  close_backlog_pull_requests
  close_backlog_issues "$keep_issue_number"
}

ensure_label() {
  local label_name="$1"
  local color="$2"
  local description="$3"

  if ! gh label create "$label_name" --color "$color" --description "$description" >/dev/null 2>&1; then
    gh label edit "$label_name" --color "$color" --description "$description" >/dev/null 2>&1 || true
  fi
}

ensure_issue_label_contract() {
  ensure_label "triage-idea" "FBCA04" "Idea queued for cloud triage orchestration"
  ensure_label "copilot-suggestion" "C5DEF5" "Copilot review suggestion triaged into issue backlog"
  ensure_label "copilot-bypass-vibe-coded" "0B63A5" "Bypass human gate after Copilot triage while tracking vibe-coded integrations"
  ensure_label "$HUMAN_TRIAGE_LABEL" "1D76DB" "Human-authored issue queued for autonomous triage"
  ensure_label "$SOURCE_HUMAN_ISSUE_LABEL" "0E8A16" "Marks triage intake that originated from a human issue template"
  ensure_label "$AI_GENERATED_LABEL" "5319E7" "Marks triage intake generated by AI automation"
  ensure_label "$SOURCE_AI_ISSUE_LABEL" "6F42C1" "Marks triage intake that originated from AI-generated issue automation"
}

apply_issue_labels() {
  local issue_number="$1"
  local -n labels_ref="$2"

  ensure_issue_label_contract

  for label in "${labels_ref[@]}"; do
    if gh issue edit "$issue_number" --add-label "$label" >/dev/null 2>&1; then
      continue
    fi

    if [[ "$label" == agent:* ]]; then
      agent_name="${label#agent:}"
      ensure_label "$label" "5319E7" "Routes triage automation toward ${agent_name}"
      gh issue edit "$issue_number" --add-label "$label" >/dev/null 2>&1 || true
      continue
    fi

    if [[ "$label" == "triage-idea" || "$label" == "copilot-suggestion" || "$label" == "copilot-bypass-vibe-coded" || "$label" == "$HUMAN_TRIAGE_LABEL" || "$label" == "$SOURCE_HUMAN_ISSUE_LABEL" || "$label" == "$AI_GENERATED_LABEL" || "$label" == "$SOURCE_AI_ISSUE_LABEL" ]]; then
      ensure_issue_label_contract
      gh issue edit "$issue_number" --add-label "$label" >/dev/null 2>&1 || true
      continue
    fi

    echo "::warning::Unable to add label '$label' to issue #$issue_number."
  done
}

ensure_issue_label_contract

declare -a parsed_issue_labels=()
parse_list_input "$TRIAGE_ISSUE_LABELS" parsed_issue_labels

clear_triage_backlog_if_enabled "$TRIAGE_ISSUE_NUMBER"

if [[ -z "$TRIAGE_ISSUE_NUMBER" ]]; then
  normalized_idea="$(normalize_whitespace "$TRIAGE_IDEA")"

  if [[ -z "$normalized_idea" ]]; then
    echo "::error::Provide BANANA_TRIAGE_IDEA when BANANA_TRIAGE_ISSUE_NUMBER is not set."
    exit 1
  fi

  issue_title="${BANANA_TRIAGE_ISSUE_TITLE:-triage: ${normalized_idea}}"
  if [[ ${#issue_title} -gt 120 ]]; then
    issue_title="${issue_title:0:117}..."
  fi

  issue_body_default=$(cat <<EOF
<!-- banana-intake-source: ai -->

## Idea

${TRIAGE_IDEA}

## Requested automation

- Capture this idea in ${TRIAGE_INTAKE_DIR}.
- Open an automation pull request via triaged-item orchestration.
- Preserve Copilot provenance labels for downstream automation review.

_This issue was generated by cloud triage orchestration._
EOF
)
  issue_body="${BANANA_TRIAGE_ISSUE_BODY:-$issue_body_default}"

  issue_url="$(gh issue create --title "$issue_title" --body "$issue_body")"
  TRIAGE_ISSUE_NUMBER="${issue_url##*/}"
fi

issue_title="$(gh issue view "$TRIAGE_ISSUE_NUMBER" --json title --jq .title)"
issue_url="$(gh issue view "$TRIAGE_ISSUE_NUMBER" --json url --jq .url)"
issue_body="$(gh issue view "$TRIAGE_ISSUE_NUMBER" --json body --jq .body)"
if [[ "$issue_body" == "null" ]]; then
  issue_body=""
fi

issue_source_marker="$(extract_issue_marker "$issue_body" "banana-intake-source")"
issue_source_marker="$(echo "$issue_source_marker" | tr '[:upper:]' '[:lower:]' | xargs)"
target_agent_marker="$(extract_issue_marker "$issue_body" "banana-target-agent")"
target_agent_marker="$(sanitize_agent_name "$target_agent_marker")"

if [[ "$issue_source_marker" == "human" ]]; then
  parsed_issue_labels+=("$HUMAN_TRIAGE_LABEL" "$SOURCE_HUMAN_ISSUE_LABEL")
else
  parsed_issue_labels+=("$AI_GENERATED_LABEL" "$SOURCE_AI_ISSUE_LABEL")
fi

if [[ -n "$target_agent_marker" ]]; then
  parsed_issue_labels+=("agent:${target_agent_marker}")
fi

apply_issue_labels "$TRIAGE_ISSUE_NUMBER" parsed_issue_labels

if [[ "$issue_source_marker" == "human" ]]; then
  gh issue edit "$TRIAGE_ISSUE_NUMBER" --remove-label "$AI_GENERATED_LABEL" >/dev/null 2>&1 || true
  gh issue edit "$TRIAGE_ISSUE_NUMBER" --remove-label "$SOURCE_AI_ISSUE_LABEL" >/dev/null 2>&1 || true
else
  gh issue edit "$TRIAGE_ISSUE_NUMBER" --remove-label "$SOURCE_HUMAN_ISSUE_LABEL" >/dev/null 2>&1 || true
fi

issue_labels_multiline="$(gh issue view "$TRIAGE_ISSUE_NUMBER" --json labels --jq '.labels[].name' || true)"
if [[ -n "$issue_labels_multiline" ]]; then
  issue_labels_csv="$(printf '%s\n' "$issue_labels_multiline" | paste -sd ',' -)"
else
  issue_labels_csv=""
fi

issue_source_kind="ai"
if [[ "$issue_source_marker" == "human" ]]; then
  issue_source_kind="human"
fi

declare -a parsed_pr_labels=()
parse_list_input "$TRIAGE_PR_LABELS" parsed_pr_labels

if [[ -n "$issue_labels_multiline" ]]; then
  while IFS= read -r issue_label; do
    if [[ "$issue_label" == agent:* || "$issue_label" == "$SOURCE_HUMAN_ISSUE_LABEL" || "$issue_label" == "$SOURCE_AI_ISSUE_LABEL" ]]; then
      parsed_pr_labels+=("$issue_label")
    fi
  done < <(printf '%s\n' "$issue_labels_multiline")
fi

if [[ ${#parsed_pr_labels[@]} -gt 0 ]]; then
  TRIAGE_PR_LABELS="$(printf '%s\n' "${parsed_pr_labels[@]}" | awk '!seen[$0]++' | paste -sd ',' -)"
fi

issue_slug="$(printf '%s' "$issue_title" | tr '[:upper:]' '[:lower:]' | sed -E 's/[^a-z0-9._-]+/-/g; s/^-+//; s/-+$//')"
if [[ -z "$issue_slug" ]]; then
  issue_slug="triage-idea"
fi
issue_slug="${issue_slug:0:48}"
triage_id="issue-${TRIAGE_ISSUE_NUMBER}-${issue_slug}"
triage_id="${triage_id:0:96}"

intake_file="${TRIAGE_INTAKE_DIR}/issue-${TRIAGE_ISSUE_NUMBER}.md"
mkdir -p "$TRIAGE_INTAKE_DIR"

cat > "$intake_file" <<EOF
# Triage Intake: Issue #${TRIAGE_ISSUE_NUMBER}

- Source issue: ${issue_url}
- Source title: ${issue_title}
- Source labels: ${issue_labels_csv}
- Intake source: ${issue_source_kind}
- Target agent: ${target_agent_marker:-not-specified}

## Idea

${issue_body}

## Cloud Orchestration Contract

- Entry workflow: Orchestrate Triage Idea Cloud
- Pull request labels: ${TRIAGE_PR_LABELS}
- Base branch: ${BASE_BRANCH}
- Branch prefix: ${BRANCH_PREFIX}
EOF

title_summary="$(normalize_whitespace "$issue_title")"
if [[ ${#title_summary} -gt 72 ]]; then
  title_summary="${title_summary:0:69}..."
fi

pr_body_default=$(cat <<EOF
Cloud triage intake generated from issue #${TRIAGE_ISSUE_NUMBER}.

- Source issue: ${issue_url}
- Intake artifact: ${intake_file}
- Workflow run: ${RUN_ID} attempt ${RUN_ATTEMPT}
EOF
)

export BANANA_TRIAGE_ID="$triage_id"
export BANANA_TRIAGE_CHANGE_COMMAND=":"
export BANANA_BASE_BRANCH="$BASE_BRANCH"
export BANANA_BRANCH_PREFIX="$BRANCH_PREFIX"
export BANANA_COMMIT_MESSAGE="${BANANA_COMMIT_MESSAGE:-chore(triage): intake issue #${TRIAGE_ISSUE_NUMBER}}"
export BANANA_PR_TITLE="${BANANA_PR_TITLE:-triage(issue-${TRIAGE_ISSUE_NUMBER}): intake ${title_summary}}"
export BANANA_PR_BODY="${BANANA_PR_BODY:-$pr_body_default}"
export BANANA_DRAFT_PR="$DRAFT_PR"
export BANANA_PR_LABELS="$TRIAGE_PR_LABELS"
export BANANA_PR_REVIEWERS="$PR_REVIEWERS"
export BANANA_SKIP_IF_NO_CHANGES="$SKIP_IF_NO_CHANGES"
export BANANA_PR_OUTPUT_PATH="$PR_OUTPUT_PATH"

bash scripts/workflow-orchestrate-triaged-item-pr.sh

pr_status="$(python - "$PR_OUTPUT_PATH" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
if not path.exists():
    print("")
    raise SystemExit(0)

payload = json.loads(path.read_text(encoding="utf-8"))
print(payload.get("status", ""))
PY
)"

pr_url="$(python - "$PR_OUTPUT_PATH" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
if not path.exists():
    print("")
    raise SystemExit(0)

payload = json.loads(path.read_text(encoding="utf-8"))
print(payload.get("pr_url", ""))
PY
)"

pr_number="$(python - "$PR_OUTPUT_PATH" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
if not path.exists():
    print("")
    raise SystemExit(0)

payload = json.loads(path.read_text(encoding="utf-8"))
print(payload.get("pr_number", ""))
PY
)"

pr_branch="$(python - "$PR_OUTPUT_PATH" <<'PY'
import json
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
if not path.exists():
    print("")
    raise SystemExit(0)

payload = json.loads(path.read_text(encoding="utf-8"))
print(payload.get("branch", ""))
PY
)"

if [[ "$TRIAGE_DISPATCH_REQUIRED_CHECKS" == "true" && "$pr_status" == "created" && -n "$pr_number" && -n "$pr_branch" ]]; then
  repo_slug="$(get_repo_slug)"

  declare -a parsed_check_workflows=()
  parse_list_input "$TRIAGE_CHECK_WORKFLOWS" parsed_check_workflows
  for workflow_file in "${parsed_check_workflows[@]}"; do
    dispatch_gate_workflow "$repo_slug" "$workflow_file" "$pr_number" "$pr_branch" || true
  done
fi

if [[ -n "$pr_url" ]]; then
  gh issue comment "$TRIAGE_ISSUE_NUMBER" --body "Automation PR opened for this triage idea: ${pr_url}" >/dev/null 2>&1 || true
elif [[ "$pr_status" == "skipped" ]]; then
  gh issue comment "$TRIAGE_ISSUE_NUMBER" --body "Cloud triage orchestration found no effective repository change and skipped PR creation." >/dev/null 2>&1 || true
fi

if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
  {
    echo "issue_number=${TRIAGE_ISSUE_NUMBER}"
    echo "issue_url=${issue_url}"
    echo "triage_id=${triage_id}"
    echo "pr_status=${pr_status}"
    echo "pr_url=${pr_url}"
    echo "pr_output_path=${PR_OUTPUT_PATH}"
  } >> "$GITHUB_OUTPUT"
fi

echo "Triage idea orchestration complete for issue #${TRIAGE_ISSUE_NUMBER}."

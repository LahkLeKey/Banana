#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

RUN_ID="${GITHUB_RUN_ID:-local-run}"
RUN_ATTEMPT="${GITHUB_RUN_ATTEMPT:-1}"
RELEASE_ARTIFACTS_SOURCE="${BANANA_RELEASE_ARTIFACTS_SOURCE:-artifacts/not-banana-release-artifacts}"
BRANCH_PREFIX="${BANANA_REGISTRY_HISTORY_BRANCH:-model-release-history}"
BASE_BRANCH="${BANANA_REGISTRY_PR_BASE_BRANCH:-main}"
HISTORY_PATH="${BANANA_REGISTRY_HISTORY_PATH:-data/not-banana/model-release-history}"
OPEN_DRAFT_PR="${BANANA_REGISTRY_OPEN_DRAFT_PR:-true}"
PR_LABELS="${BANANA_REGISTRY_PR_LABELS:-automation,model-training,requires-human-approval,agent:banana-classifier-agent}"
PR_REVIEWERS="${BANANA_REGISTRY_PR_REVIEWERS:-}"
LOCAL_DRY_RUN="${BANANA_LOCAL_DRY_RUN:-false}"
REQUIRED_HUMAN_REVIEWER="${BANANA_REQUIRED_HUMAN_REVIEWER:-LahkLeKey}"
AGENT_CONTRIBUTOR_OVERRIDE="${BANANA_AGENT_CONTRIBUTOR:-banana-classifier-agent}"
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

for candidate in payload.get("agents", []):
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
  AGENT_CONTRIBUTOR_SLUG="$(slugify_agent_contributor "$AGENT_CONTRIBUTOR_OVERRIDE")"
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

if [[ ! -d "$RELEASE_ARTIFACTS_SOURCE" ]]; then
  echo "::error::Release artifacts source does not exist: $RELEASE_ARTIFACTS_SOURCE"
  exit 1
fi

if ! find "$RELEASE_ARTIFACTS_SOURCE" -mindepth 1 -print -quit | grep -q .; then
  echo "::error::Release artifacts source is empty: $RELEASE_ARTIFACTS_SOURCE"
  exit 1
fi

STAMP="$(date -u +%Y%m%dT%H%M%SZ)"
BRANCH_PREFIX="${BRANCH_PREFIX%/}"
HISTORY_PATH="${HISTORY_PATH%/}"
WORK_BRANCH="${BRANCH_PREFIX}/run-${RUN_ID}-attempt-${RUN_ATTEMPT}"

if [[ "$LOCAL_DRY_RUN" == "true" ]]; then
  DRY_RUN_ROOT="artifacts/local-workflow-dry-run/training-pr"
  STAGED_DIR="$DRY_RUN_ROOT/${STAMP}"
  mkdir -p "$STAGED_DIR"
  cp -R "$RELEASE_ARTIFACTS_SOURCE"/* "$STAGED_DIR/"

  PLAN_FILE="$DRY_RUN_ROOT/plan-${RUN_ID}-attempt-${RUN_ATTEMPT}.json"
  cat > "$PLAN_FILE" <<EOF
{
  "mode": "dry-run",
  "run_id": "${RUN_ID}",
  "run_attempt": "${RUN_ATTEMPT}",
  "source_artifacts": "${RELEASE_ARTIFACTS_SOURCE}",
  "staged_snapshot_dir": "${STAGED_DIR}",
  "target_branch": "${WORK_BRANCH}",
  "base_branch": "${BASE_BRANCH}",
  "history_path": "${HISTORY_PATH}",
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
  "open_draft": "${OPEN_DRAFT_PR}",
  "simulated_pr_url": "https://example.invalid/${WORK_BRANCH}"
}
EOF

  echo "Dry-run registry history PR plan generated: $PLAN_FILE"
  exit 0
fi

if ! command -v gh >/dev/null 2>&1; then
  echo "::error::GitHub CLI is required to orchestrate registry-history pull requests."
  exit 1
fi

git fetch origin "$BASE_BRANCH"
git checkout -B "$WORK_BRANCH" "origin/$BASE_BRANCH"

TARGET_DIR="$HISTORY_PATH/$STAMP"
mkdir -p "$TARGET_DIR"
cp -R "$RELEASE_ARTIFACTS_SOURCE"/* "$TARGET_DIR/"

BANANA_REGISTRY_HISTORY_PATH="$HISTORY_PATH" python - <<'PY'
import json
import os
from pathlib import Path

history_path = Path(os.environ["BANANA_REGISTRY_HISTORY_PATH"]).resolve()
entries = [
    path.name
    for path in history_path.iterdir()
    if path.is_dir() and not path.name.startswith(".")
]
entries.sort(reverse=True)

latest = {
    "schema_version": 1,
    "updated_at_utc": entries[0] if entries else "",
    "snapshots": entries[:50],
}
latest_path = history_path / "latest.json"
latest_path.parent.mkdir(parents=True, exist_ok=True)
latest_path.write_text(json.dumps(latest, indent=2) + "\n", encoding="utf-8")
PY

git config user.name "$AGENT_CONTRIBUTOR_NAME"
git config user.email "$AGENT_CONTRIBUTOR_EMAIL"
git add "$HISTORY_PATH"

if git diff --cached --quiet; then
  echo "No registry history changes to commit."
  exit 0
fi

git commit -m "chore(model): persist release snapshots run ${RUN_ID} [skip ci]"
git push -u origin "$WORK_BRANCH"

PR_TITLE="chore(model): persist release snapshots run ${RUN_ID}"
PR_BODY=$(cat <<EOF
This pull request was orchestrated by the not-banana training workflow and requires human approval before merge.

- Run ID: ${RUN_ID}
- Attempt: ${RUN_ATTEMPT}
- Snapshot path: ${HISTORY_PATH}/${STAMP}
- Source branch: ${WORK_BRANCH}
- Automation contributor: ${AGENT_CONTRIBUTOR_NAME} <${AGENT_CONTRIBUTOR_EMAIL}>
- Automation contributor login: ${AGENT_CONTRIBUTOR_LOGIN}
- Automation contributor icon: ${AGENT_CONTRIBUTOR_ICON}
- Automation contributor profile: ${AGENT_CONTRIBUTOR_PROFILE_URL}
- Automation contributor followable: ${AGENT_CONTRIBUTOR_FOLLOWABLE}
- Required human reviewer: ${REQUIRED_HUMAN_REVIEWER}
EOF
)

existing_pr_number="$(gh pr list --base "$BASE_BRANCH" --head "$WORK_BRANCH" --state open --json number --jq '.[0].number')"

if [[ -n "$existing_pr_number" ]]; then
  gh pr edit "$existing_pr_number" --title "$PR_TITLE" --body "$PR_BODY"
  pr_number="$existing_pr_number"
else
  CREATE_ARGS=(
    --base "$BASE_BRANCH"
    --head "$WORK_BRANCH"
    --title "$PR_TITLE"
    --body "$PR_BODY"
  )
  if [[ "$OPEN_DRAFT_PR" == "true" ]]; then
    CREATE_ARGS+=(--draft)
  fi

  pr_url="$(gh pr create "${CREATE_ARGS[@]}")"
  pr_number="${pr_url##*/}"
fi

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

pr_url="$(gh pr view "$pr_number" --json url --jq '.url')"
echo "Registry history PR ready for review: $pr_url"

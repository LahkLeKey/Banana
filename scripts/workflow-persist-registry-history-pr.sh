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
PR_LABELS="${BANANA_REGISTRY_PR_LABELS:-automation,model-training,requires-human-approval}"
PR_REVIEWERS="${BANANA_REGISTRY_PR_REVIEWERS:-}"
LOCAL_DRY_RUN="${BANANA_LOCAL_DRY_RUN:-false}"

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
  "reviewers": "${PR_REVIEWERS}",
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

git config user.name "github-actions[bot]"
git config user.email "41898282+github-actions[bot]@users.noreply.github.com"
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

if [[ -n "${PR_REVIEWERS:-}" ]]; then
  if ! gh pr edit "$pr_number" --add-reviewer "$PR_REVIEWERS" >/dev/null 2>&1; then
    echo "::warning::Failed to request reviewers '$PR_REVIEWERS' for PR #$pr_number."
  fi
fi

pr_url="$(gh pr view "$pr_number" --json url --jq '.url')"
echo "Registry history PR ready for review: $pr_url"

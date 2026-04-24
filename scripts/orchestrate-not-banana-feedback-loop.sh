#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

bash scripts/workflow-ensure-speckit.sh

RUN_ID="${GITHUB_RUN_ID:-local-run}"
RUN_ATTEMPT="${GITHUB_RUN_ATTEMPT:-1}"

FEEDBACK_PATH="${BANANA_FEEDBACK_PATH:-data/not-banana/feedback/inbox.json}"
CORPUS_PATH="${BANANA_CORPUS_PATH:-data/not-banana/corpus.json}"
STATUS_FILTER="${BANANA_FEEDBACK_STATUS_FILTER:-approved}"
MAX_ENTRIES="${BANANA_FEEDBACK_MAX_ENTRIES:-0}"
CONSUME_APPROVED="${BANANA_FEEDBACK_CONSUME_APPROVED:-true}"
STRICT_VALIDATION="${BANANA_FEEDBACK_STRICT_VALIDATION:-false}"
REQUIRE_HUMAN_REVIEW="${BANANA_FEEDBACK_REQUIRE_HUMAN_REVIEW:-true}"
MIN_HUMAN_REVIEWERS="${BANANA_FEEDBACK_MIN_HUMAN_REVIEWERS:-1}"

TRIAGE_ID="${BANANA_FEEDBACK_TRIAGE_ID:-not-banana-feedback}"
BASE_BRANCH="${BANANA_BASE_BRANCH:-main}"
BRANCH_PREFIX="${BANANA_BRANCH_PREFIX:-triage-feedback}"
COMMIT_MESSAGE="${BANANA_COMMIT_MESSAGE:-chore(feedback): apply not-banana feedback into corpus}"
PR_TITLE_DEFAULT="triage(not-banana-feedback): apply approved feedback to corpus"
PR_TITLE="${BANANA_PR_TITLE:-$PR_TITLE_DEFAULT}"
PR_BODY="${BANANA_PR_BODY:-}"
DRAFT_PR="${BANANA_DRAFT_PR:-true}"
PR_LABELS="${BANANA_PR_LABELS:-automation,triaged-item,requires-human-approval,copilot-auto-approve,speckit-driven,feedback-loop,agent:banana-classifier-agent}"
PR_REVIEWERS="${BANANA_PR_REVIEWERS:-}"
AGENT_CONTRIBUTOR="${BANANA_AGENT_CONTRIBUTOR:-banana-classifier-agent}"

REPORT_ROOT="artifacts/not-banana-feedback"
mkdir -p "$REPORT_ROOT"
PREVIEW_REPORT="$REPORT_ROOT/preview-${RUN_ID}-attempt-${RUN_ATTEMPT}.json"
APPLY_REPORT="$REPORT_ROOT/apply-${RUN_ID}-attempt-${RUN_ATTEMPT}.json"

preview_args=(
  python scripts/apply-not-banana-feedback.py
  --corpus "$CORPUS_PATH"
  --feedback "$FEEDBACK_PATH"
  --status-filter "$STATUS_FILTER"
  --max-entries "$MAX_ENTRIES"
  --report "$PREVIEW_REPORT"
  --dry-run
)

if [[ "$CONSUME_APPROVED" == "true" ]]; then
  preview_args+=(--consume)
else
  preview_args+=(--no-consume)
fi

if [[ "$STRICT_VALIDATION" == "true" ]]; then
  preview_args+=(--strict)
fi

if [[ "$REQUIRE_HUMAN_REVIEW" == "true" ]]; then
  preview_args+=(--require-human-review)
else
  preview_args+=(--no-require-human-review)
fi

preview_args+=(--minimum-human-reviewers "$MIN_HUMAN_REVIEWERS")

"${preview_args[@]}"

read -r eligible processed added duplicate invalid oversight_invalid < <(
  python - "$PREVIEW_REPORT" <<'PY'
import json
import sys
from pathlib import Path

report = json.loads(Path(sys.argv[1]).read_text(encoding="utf-8"))
print(
    report.get("eligible_count", 0),
    report.get("processed_count", 0),
    report.get("added_count", 0),
    report.get("duplicate_count", 0),
    report.get("invalid_count", 0),
    report.get("oversight_invalid_count", 0),
)
PY
)

if [[ "$processed" == "0" ]]; then
  echo "No feedback entries matched the status filter ($STATUS_FILTER). Skipping orchestration."
  exit 0
fi

if [[ "$added" == "0" && "$CONSUME_APPROVED" != "true" ]]; then
  echo "No new corpus samples to add and consume mode is disabled. Skipping orchestration."
  exit 0
fi

if [[ -z "$PR_BODY" ]]; then
  PR_BODY=$(cat <<EOF
This pull request was orchestrated from the not-banana feedback inbox.

- Feedback path: ${FEEDBACK_PATH}
- Corpus path: ${CORPUS_PATH}
- Status filter: ${STATUS_FILTER}
- Eligible entries: ${eligible}
- Processed entries: ${processed}
- Added samples: ${added}
- Duplicate samples: ${duplicate}
- Invalid entries: ${invalid}
- Oversight-invalid entries: ${oversight_invalid}
- Human review required: ${REQUIRE_HUMAN_REVIEW}
- Minimum human reviewers: ${MIN_HUMAN_REVIEWERS}
- Apply report path: ${APPLY_REPORT}
EOF
)
fi

apply_args=(
  python scripts/apply-not-banana-feedback.py
  --corpus "$CORPUS_PATH"
  --feedback "$FEEDBACK_PATH"
  --status-filter "$STATUS_FILTER"
  --max-entries "$MAX_ENTRIES"
  --report "$APPLY_REPORT"
)

if [[ "$CONSUME_APPROVED" == "true" ]]; then
  apply_args+=(--consume)
else
  apply_args+=(--no-consume)
fi

if [[ "$STRICT_VALIDATION" == "true" ]]; then
  apply_args+=(--strict)
fi

if [[ "$REQUIRE_HUMAN_REVIEW" == "true" ]]; then
  apply_args+=(--require-human-review)
else
  apply_args+=(--no-require-human-review)
fi

apply_args+=(--minimum-human-reviewers "$MIN_HUMAN_REVIEWERS")

printf -v APPLY_COMMAND '%q ' "${apply_args[@]}"
APPLY_COMMAND="${APPLY_COMMAND% }"

export BANANA_TRIAGE_ID="$TRIAGE_ID"
export BANANA_TRIAGE_CHANGE_COMMAND="$APPLY_COMMAND"
export BANANA_BASE_BRANCH="$BASE_BRANCH"
export BANANA_BRANCH_PREFIX="$BRANCH_PREFIX"
export BANANA_COMMIT_MESSAGE="$COMMIT_MESSAGE"
export BANANA_PR_TITLE="$PR_TITLE"
export BANANA_PR_BODY="$PR_BODY"
export BANANA_DRAFT_PR="$DRAFT_PR"
export BANANA_PR_LABELS="$PR_LABELS"
export BANANA_PR_REVIEWERS="$PR_REVIEWERS"
export BANANA_AGENT_CONTRIBUTOR="$AGENT_CONTRIBUTOR"

bash scripts/workflow-orchestrate-triaged-item-pr.sh

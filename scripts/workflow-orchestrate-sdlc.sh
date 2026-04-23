#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

RUN_ID="${GITHUB_RUN_ID:-local-run}"
RUN_ATTEMPT="${GITHUB_RUN_ATTEMPT:-1}"
SDLC_ID="${BANANA_SDLC_ID:-banana-sdlc}"
PLAN_JSON="${BANANA_SDLC_INCREMENT_PLAN_JSON:-}"
PLAN_PATH="${BANANA_SDLC_INCREMENT_PLAN_PATH:-}"
DEFAULT_BASE_BRANCH="${BANANA_BASE_BRANCH:-main}"
DEFAULT_BRANCH_PREFIX="${BANANA_BRANCH_PREFIX:-sdlc}"
DEFAULT_DRAFT_PR="${BANANA_DRAFT_PR:-true}"
DEFAULT_LABELS="${BANANA_PR_LABELS:-automation,sdlc,triaged-item,requires-human-approval}"
DEFAULT_REVIEWERS="${BANANA_PR_REVIEWERS:-}"
SKIP_NO_CHANGES="${BANANA_SDLC_SKIP_NO_CHANGES:-true}"
CONTINUE_ON_ERROR="${BANANA_SDLC_CONTINUE_ON_ERROR:-false}"
ENABLE_WIKI_SYNC="${BANANA_SDLC_ENABLE_WIKI_SYNC:-true}"
WIKI_DRY_RUN="${BANANA_WIKI_DRY_RUN:-false}"
WIKI_PUSH="${BANANA_WIKI_PUSH:-true}"
WIKI_STRICT="${BANANA_WIKI_STRICT:-false}"
WIKI_COMMIT_MESSAGE="${BANANA_WIKI_COMMIT_MESSAGE:-docs(wiki): sync automated SDLC documentation snapshots}"
WIKI_REMOTE_URL="${BANANA_WIKI_REMOTE_URL:-https://github.com/LahkLeKey/Banana.wiki.git}"
WIKI_ENFORCE_CANONICAL_REMOTE="${BANANA_ENFORCE_CANONICAL_WIKI_REMOTE:-true}"
OUTPUT_DIR="${BANANA_SDLC_OUTPUT_DIR:-artifacts/sdlc-orchestration}"
SUMMARY_PATH="${BANANA_SDLC_SUMMARY_PATH:-${OUTPUT_DIR}/summary-${RUN_ID}-attempt-${RUN_ATTEMPT}.json}"

mkdir -p "$OUTPUT_DIR"

if [[ "${BANANA_SDLC_VALIDATE_AI_CONTRACTS:-true}" == "true" ]]; then
  echo "Validating AI customization and wiki-sync contracts..."
  python scripts/validate-ai-contracts.py > "${OUTPUT_DIR}/ai-contract-report-${RUN_ID}-attempt-${RUN_ATTEMPT}.json"
fi

PLAN_INPUT_PATH="${OUTPUT_DIR}/plan-${RUN_ID}-attempt-${RUN_ATTEMPT}.json"
PLAN_ROWS_PATH="${OUTPUT_DIR}/plan-rows-${RUN_ID}-attempt-${RUN_ATTEMPT}.tsv"
WIKI_REPORT_PATH="${OUTPUT_DIR}/wiki-sync-${RUN_ID}-attempt-${RUN_ATTEMPT}.json"

if [[ -n "$PLAN_JSON" ]]; then
  printf '%s\n' "$PLAN_JSON" > "$PLAN_INPUT_PATH"
elif [[ -n "$PLAN_PATH" ]]; then
  cp "$PLAN_PATH" "$PLAN_INPUT_PATH"
else
  cat > "$PLAN_INPUT_PATH" <<'EOF'
[
  {
    "id": "feedback-corpus",
    "change_command": "python scripts/apply-not-banana-feedback.py --feedback data/not-banana/feedback/inbox.json --corpus data/not-banana/corpus.json --status-filter approved --consume --require-human-review --minimum-human-reviewers 1 --report artifacts/not-banana-feedback/sdlc-apply-report.json",
    "commit_message": "chore(feedback): apply approved feedback into corpus",
    "pr_title": "triage(feedback): apply approved inbox updates",
    "pr_body": "Automated SDLC increment: apply approved not-banana feedback into corpus.",
    "labels": "automation,sdlc,triaged-item,requires-human-approval,feedback-loop"
  },
  {
    "id": "training-docs",
    "change_command": "python scripts/train-not-banana-model.py --corpus data/not-banana/corpus.json --output artifacts/not-banana-model --training-profile ci --session-mode single --max-sessions 1 && mkdir -p src/native/core/domain/generated && cp artifacts/not-banana-model/banana_signal_tokens.h src/native/core/domain/generated/banana_signal_tokens.h",
    "commit_message": "chore(model): refresh generated not-banana vocabulary header",
    "pr_title": "triage(model): refresh generated not-banana vocabulary",
    "pr_body": "Automated SDLC increment: retrain not-banana vocabulary and refresh generated native header.",
    "labels": "automation,sdlc,triaged-item,requires-human-approval,model-training"
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

  if ! (
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
    export BANANA_SKIP_IF_NO_CHANGES="$SKIP_NO_CHANGES"
    export BANANA_PR_OUTPUT_PATH="$increment_report_path"
    bash scripts/workflow-orchestrate-triaged-item-pr.sh
  ); then
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

python - "$SUMMARY_PATH" "$OUTPUT_DIR" "$WIKI_REPORT_PATH" <<'PY'
import json
import pathlib
import sys

summary_path = pathlib.Path(sys.argv[1])
output_dir = pathlib.Path(sys.argv[2])
wiki_report_path = pathlib.Path(sys.argv[3])

increment_reports = []
for report_path in sorted(output_dir.glob("increment-*.json")):
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

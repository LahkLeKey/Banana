#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

AGENTS=(
  api-pipeline-agent
  csharp-api-agent
  react-ui-agent
  electron-agent
  workflow-agent
  integration-agent
  compose-runtime-agent
  banana-classifier-agent
)

OUTPUT_DIR="artifacts/agent-smoke"
mkdir -p "$OUTPUT_DIR"

for agent in "${AGENTS[@]}"; do
  report_path="${OUTPUT_DIR}/${agent}.json"

  BANANA_TRIAGE_ID="smoke-${agent}" \
  BANANA_TRIAGE_CHANGE_COMMAND=":" \
  BANANA_BASE_BRANCH="main" \
  BANANA_BRANCH_PREFIX="triage-smoke" \
  BANANA_COMMIT_MESSAGE="chore(smoke): ${agent} attribution" \
  BANANA_DRAFT_PR="true" \
  BANANA_PR_LABELS="automation,triaged-item,requires-human-approval,speckit-driven,agent:${agent}" \
  BANANA_PR_REVIEWERS="" \
  BANANA_LOCAL_DRY_RUN="true" \
  BANANA_PR_OUTPUT_PATH="$report_path" \
  BANANA_AGENT_CONTRIBUTOR="$agent" \
  BANANA_REQUIRED_HUMAN_REVIEWER="LahkLeKey" \
  bash scripts/workflow-orchestrate-triaged-item-pr.sh

done

python - "$OUTPUT_DIR" <<'PY'
import json
import pathlib
import sys

output_dir = pathlib.Path(sys.argv[1])
reports = sorted(output_dir.glob("*.json"))
if len(reports) < 6:
    raise SystemExit(f"Expected at least 6 agent smoke reports, found {len(reports)}")

required_reviewer = "LahkLeKey"
for report in reports:
    payload = json.loads(report.read_text(encoding="utf-8"))
    status = payload.get("status")
    if status not in {"dry-run", "skipped"}:
        raise SystemExit(f"Unexpected status in {report.name}: {status}")

    if status == "skipped":
        reason = payload.get("reason", "")
        if reason != "non-impact/doc-only changes":
            raise SystemExit(f"Unexpected skipped reason in {report.name}: {reason}")

    contributor_slug = payload.get("automation_contributor_slug", "")
    contributor_label = payload.get("automation_contributor_label", "")
    token_source = payload.get("author_token_source", "")
    reviewer = payload.get("required_human_reviewer", "")

    if not contributor_slug:
        raise SystemExit(f"Missing contributor slug in {report.name}")

    if contributor_label != f"contributor:{contributor_slug}":
        raise SystemExit(f"Unexpected contributor label in {report.name}: {contributor_label}")

    if token_source != "github-token":
        raise SystemExit(f"Unexpected token source in {report.name}: {token_source}")

    if reviewer != required_reviewer:
        raise SystemExit(
            f"Unexpected required reviewer in {report.name}: {reviewer} (expected {required_reviewer})"
        )

print(f"Validated {len(reports)} spec-driven agent smoke reports.")
PY

echo "Spec-driven agent smoke test completed successfully."
#!/usr/bin/env bash
# Open a follow-up evidence PR with refreshed neuro artifacts (feature 050 T012).
#
# Wraps scripts/workflow-orchestrate-triaged-item-pr.sh so we reuse the same
# branch/PR conventions, label policy, and human-approval safeguards.
#
# Required env:
#   BANANA_NEURO_MODEL    banana | not-banana | ripeness
#   BANANA_NEURO_TRIGGER  e.g. git-event | consolidation-nrem | consolidation-rem
#
# Optional env (with sensible defaults):
#   BANANA_PR_LABELS, BANANA_BASE_BRANCH, BANANA_BRANCH_PREFIX
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT_DIR"

MODEL="${BANANA_NEURO_MODEL:?BANANA_NEURO_MODEL is required}"
TRIGGER="${BANANA_NEURO_TRIGGER:?BANANA_NEURO_TRIGGER is required}"
NOW_UTC="$(date -u +%Y-%m-%dT%H:%M:%SZ)"
ARTIFACT_DIR="artifacts/training/${MODEL}"
EVIDENCE_PATH="${ARTIFACT_DIR}/evidence-${TRIGGER}.json"

mkdir -p "$ARTIFACT_DIR"

python - "$EVIDENCE_PATH" "$MODEL" "$TRIGGER" "$NOW_UTC" <<'PY'
import json
import os
import sys
from pathlib import Path

evidence_path, model, trigger, now_utc = sys.argv[1:5]
artifact_root = Path("artifacts/training") / model

def _maybe(path: Path) -> object | None:
    if not path.exists():
        return None
    try:
        return json.loads(path.read_text(encoding="utf-8"))
    except Exception:
        return {"_unreadable": str(path)}

payload = {
    "schema_version": 1,
    "generated_at_utc": now_utc,
    "model": model,
    "trigger": trigger,
    "run_id": os.environ.get("GITHUB_RUN_ID"),
    "run_attempt": os.environ.get("GITHUB_RUN_ATTEMPT"),
    "workflow": os.environ.get("GITHUB_WORKFLOW"),
    "metrics": _maybe(artifact_root / "local" / "metrics.json"),
    "neuro_trace": _maybe(artifact_root / "local" / "neuro-trace.json"),
    "consolidation_index": sorted(
        str(p.relative_to(Path.cwd())) for p in (artifact_root / "consolidation").glob("*/consolidation-report.json")
    ) if (artifact_root / "consolidation").exists() else [],
}
Path(evidence_path).write_text(
    json.dumps(payload, indent=2, sort_keys=True) + "\n", encoding="utf-8"
)
print(f"Wrote evidence: {evidence_path}")
PY

export BANANA_TRIAGE_ID="${BANANA_TRIAGE_ID:-neuro-${MODEL}-${TRIGGER}}"
export BANANA_BRANCH_PREFIX="${BANANA_BRANCH_PREFIX:-neuro/${MODEL}}"
export BANANA_COMMIT_MESSAGE="${BANANA_COMMIT_MESSAGE:-feat(neuro): refresh ${MODEL} ${TRIGGER} evidence}"
export BANANA_PR_TITLE="${BANANA_PR_TITLE:-feat(neuro): ${MODEL} ${TRIGGER} evidence ${NOW_UTC}}"
export BANANA_PR_LABELS="${BANANA_PR_LABELS:-automation,model-training,speckit-driven,agent:banana-classifier-agent}"
export BANANA_DRAFT_PR="${BANANA_DRAFT_PR:-true}"
export BANANA_REQUIRE_REAL_UPDATES="${BANANA_REQUIRE_REAL_UPDATES:-false}"
export BANANA_TRIAGE_CHANGE_COMMAND="${BANANA_TRIAGE_CHANGE_COMMAND:-:}"

bash scripts/workflow-orchestrate-triaged-item-pr.sh

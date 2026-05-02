#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

MAX_SPECS="${BANANA_SDLC_CI_IDEA_BUDGET:-3}"
REPO="${BANANA_GITHUB_REPO:-LahkLeKey/Banana}"
OUTPUT_PATH="${BANANA_SDLC_CI_FOLLOWUP_OUTPUT_PATH:-artifacts/sdlc-orchestration/ci-followup-summary.json}"
PREFIX="${BANANA_SDLC_CI_FOLLOWUP_PREFIX:-ci-auto-debug}"

if ! command -v gh >/dev/null 2>&1; then
  echo "gh CLI is required" >&2
  exit 1
fi

mkdir -p "$(dirname "$OUTPUT_PATH")"
TMP_RUNS_JSON="${BANANA_SDLC_CI_TMP_RUNS_JSON:-$(dirname "$OUTPUT_PATH")/ci-runs-${RANDOM}-${RANDOM}.json}"

BANANA_CI_REPO="$REPO" python - <<'PY' > "$TMP_RUNS_JSON"
import json
import os
import subprocess
import sys

repo = os.environ.get("BANANA_CI_REPO", "LahkLeKey/Banana")

cmd = [
    "gh", "run", "list",
    "--limit", "50",
  "--repo", repo,
    "--json", "databaseId,name,conclusion,displayTitle,headBranch,url"
]
try:
    raw = subprocess.check_output(cmd, text=True)
except subprocess.CalledProcessError:
    print("[]")
    sys.exit(0)
print(raw)
PY

created_specs=()
created_count=0

while IFS=$'\t' read -r run_id run_name run_title run_url; do
  [[ -z "$run_id" ]] && continue
  if [[ "$created_count" -ge "$MAX_SPECS" ]]; then
    break
  fi

  next_number="$(python - <<'PY'
import pathlib
import re

spec_dir = pathlib.Path('.specify/specs')
max_n = 0
for p in spec_dir.iterdir():
    if not p.is_dir():
        continue
    m = re.match(r'^(\d+)-', p.name)
    if m:
        max_n = max(max_n, int(m.group(1)))
print(max_n + 1)
PY
)"

  slug_title="$(printf '%s' "$run_name" | tr '[:upper:]' '[:lower:]' | sed -E 's/[^a-z0-9]+/-/g; s/^-+//; s/-+$//')"
  [[ -z "$slug_title" ]] && slug_title="workflow-failure"
  spec_slug="$(printf '%03d-%s-%s' "$next_number" "$PREFIX" "$slug_title")"
  spec_dir=".specify/specs/${spec_slug}"

  mkdir -p "$spec_dir"
  cat > "$spec_dir/spec.md" <<EOF
# Feature Specification: CI Auto-Debug Follow-up for ${run_name}

**Feature Branch**: `${spec_slug}`
**Created**: $(date +%Y-%m-%d)
**Status**: Stub (autonomous CI follow-up)
**Wave**: stabilization
**Domain**: workflow / quality
**Depends on**: none

## Problem Statement

Autonomous orchestrator detected a failed CI run requiring follow-up stabilization work.

- Run id: ${run_id}
- Workflow: ${run_name}
- Title: ${run_title}
- URL: ${run_url}

## In Scope *(mandatory)*

- Reproduce the failure locally or in a scoped CI rerun.
- Identify root cause and implement smallest safe fix.
- Add or update guardrails/tests to prevent recurrence.
- Capture evidence paths and closure status in PR description.

## Out of Scope *(mandatory)*

- Unrelated refactors outside failure remediation.
- Broad platform changes without a direct link to this failure.

## Notes for the planner

- Start with failure logs and changed files in the triggering run.
- Prioritize deterministic fixes and explicit regression coverage.
EOF

  created_specs+=("$spec_slug")
  created_count=$((created_count + 1))
done < <(python - "$TMP_RUNS_JSON" <<'PY'
import json
from pathlib import Path
import sys

runs = json.loads(Path(sys.argv[1]).read_text(encoding='utf-8'))
for run in runs:
    if str(run.get('conclusion')) != 'failure':
        continue
    print(f"{run.get('databaseId','')}\t{run.get('name','')}\t{run.get('displayTitle','')}\t{run.get('url','')}")
PY
)

python - "$OUTPUT_PATH" "$created_count" "${created_specs[*]:-}" <<'PY'
import json
import pathlib
import sys

out = pathlib.Path(sys.argv[1])
count = int(sys.argv[2])
specs = [s for s in sys.argv[3].split() if s]
payload = {
    "status": "ok",
    "created_spec_count": count,
    "created_specs": specs,
}
out.parent.mkdir(parents=True, exist_ok=True)
out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
print(json.dumps(payload, indent=2))
PY

rm -f "$TMP_RUNS_JSON"

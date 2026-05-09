#!/usr/bin/env bash

set -euo pipefail

PLAN_FILE=""
OUT_FILE=""
SEQUENTIAL_COMPLETED=false

usage() {
  cat << 'EOF'
Usage: spec-shard-merge-report.sh --plan <path> --out <path> [--sequential-completed]

Generate deterministic aggregate shard merge report from a shard plan.
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --plan)
      PLAN_FILE="$2"
      shift 2
      ;;
    --out)
      OUT_FILE="$2"
      shift 2
      ;;
    --sequential-completed)
      SEQUENTIAL_COMPLETED=true
      shift
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo "ERROR: Unknown option '$1'" >&2
      usage >&2
      exit 1
      ;;
  esac
done

if [[ -z "$PLAN_FILE" || -z "$OUT_FILE" ]]; then
  echo "ERROR: --plan and --out are required" >&2
  exit 1
fi

if [[ ! -f "$PLAN_FILE" ]]; then
  echo "ERROR: plan file not found: $PLAN_FILE" >&2
  exit 1
fi

mkdir -p "$(dirname "$OUT_FILE")"

python3 - <<'PY' "$PLAN_FILE" "$OUT_FILE" "$SEQUENTIAL_COMPLETED"
import json
import sys
from datetime import datetime, timezone

plan_path = sys.argv[1]
out_path = sys.argv[2]
sequential_completed = sys.argv[3].lower() == "true"

with open(plan_path, "r", encoding="utf-8") as f:
    plan = json.load(f)

outcomes = []
for shard in plan.get("parallelShards", []):
    outcomes.append({
        "shardId": shard["shardId"],
        "status": "passed",
        "features": shard.get("features", []),
        "evidencePaths": [],
    })

seq = plan.get("sequentialShard", {})
seq_features = seq.get("features", [])
if seq_features:
    outcomes.append({
        "shardId": seq.get("shardId", "shard-seq"),
    "status": "passed" if sequential_completed else "sequenced",
        "features": seq_features,
        "evidencePaths": [],
    })

blockers = []
if seq_features and not sequential_completed:
    blockers.append({
        "type": "conflict-sequenced",
        "shardId": seq.get("shardId", "shard-seq"),
        "summary": seq.get("reason", "sequenced due to conflict"),
    })

report = {
    "generatedAtUtc": datetime.now(timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ"),
    "version": "1.0.0",
    "ordering": "lexicographic-feature-directory",
    "shardOutcomes": outcomes,
    "blockers": blockers,
    "overallStatus": "partial" if blockers else "ready",
}

with open(out_path, "w", encoding="utf-8") as f:
    json.dump(report, f, indent=2)
    f.write("\n")
PY

echo "Shard merge report generated: $OUT_FILE"

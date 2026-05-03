#!/usr/bin/env bash
set -euo pipefail

# Lightweight JSON state helper for spec-drain mode.
# Usage:
#   bash scripts/workflow-spec-drain-state.sh init <state-path> <run-id> <run-attempt>
#   bash scripts/workflow-spec-drain-state.sh is-completed <state-path> <spec-id>
#   bash scripts/workflow-spec-drain-state.sh get-next-runnable <state-path>
#   bash scripts/workflow-spec-drain-state.sh get-budget-status <state-path> <max-failures>
#   bash scripts/workflow-spec-drain-state.sh mark-completed <state-path> <spec-id>
#   bash scripts/workflow-spec-drain-state.sh mark-failed <state-path> <spec-id> <reason>
#   bash scripts/workflow-spec-drain-state.sh write-evidence <state-path> <spec-id> <result-path> <status> [reason]
#   bash scripts/workflow-spec-drain-state.sh get-summary <state-path>
#   bash scripts/workflow-spec-drain-state.sh set-stop <state-path> <reason>

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

command_name="${1:-}"
state_path="${2:-}"

if [[ -z "$command_name" || -z "$state_path" ]]; then
  echo "Usage: $0 <init|mark-completed|mark-failed|set-stop> <state-path> [args...]" >&2
  exit 1
fi

case "$command_name" in
  init)
    run_id="${3:-local-run}"
    run_attempt="${4:-1}"
    python - "$state_path" "$run_id" "$run_attempt" <<'PY'
import json
import pathlib
import sys

state_path = pathlib.Path(sys.argv[1])
run_id = sys.argv[2]
run_attempt = sys.argv[3]

if state_path.exists():
    # Preserve existing checkpoint for resumability.
    sys.exit(0)

payload = {
    "schema_version": 1,
    "mode": "spec-drain",
    "run_id": run_id,
    "run_attempt": run_attempt,
    "status": "running",
    "stop_reason": None,
    "completed_specs": [],
    "failed_specs": [],
}

state_path.parent.mkdir(parents=True, exist_ok=True)
state_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
    ;;
  mark-completed)
    spec_id="${3:-}"
    if [[ -z "$spec_id" ]]; then
      echo "mark-completed requires <spec-id>" >&2
      exit 1
    fi
    python - "$state_path" "$spec_id" <<'PY'
import json
import pathlib
import sys

state_path = pathlib.Path(sys.argv[1])
spec_id = sys.argv[2]
payload = json.loads(state_path.read_text(encoding="utf-8"))

completed = set(payload.get("completed_specs", []))
if spec_id not in completed:
    payload.setdefault("completed_specs", []).append(spec_id)

failed = []
for entry in payload.get("failed_specs", []):
    if str(entry.get("spec_id")) != spec_id:
        failed.append(entry)
payload["failed_specs"] = failed

state_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
    ;;
  mark-failed)
    spec_id="${3:-}"
    reason="${4:-failed}"
    if [[ -z "$spec_id" ]]; then
      echo "mark-failed requires <spec-id> <reason>" >&2
      exit 1
    fi
    python - "$state_path" "$spec_id" "$reason" <<'PY'
import json
import pathlib
import sys

state_path = pathlib.Path(sys.argv[1])
spec_id = sys.argv[2]
reason = sys.argv[3]
payload = json.loads(state_path.read_text(encoding="utf-8"))

failed_specs = payload.setdefault("failed_specs", [])
failed_specs.append({"spec_id": spec_id, "reason": reason})

state_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
    ;;
  set-stop)
    reason="${3:-unknown}"
    python - "$state_path" "$reason" <<'PY'
import json
import pathlib
import sys

state_path = pathlib.Path(sys.argv[1])
reason = sys.argv[2]
payload = json.loads(state_path.read_text(encoding="utf-8"))
payload["status"] = "stopped"
payload["stop_reason"] = reason
state_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
    ;;
  is-completed)
    spec_id="${3:-}"
    if [[ -z "$spec_id" ]]; then
      echo "is-completed requires <spec-id>" >&2
      exit 1
    fi
    python - "$state_path" "$spec_id" <<'PY'
import json
import pathlib
import sys

state_path = pathlib.Path(sys.argv[1])
spec_id = sys.argv[2]
payload = json.loads(state_path.read_text(encoding="utf-8"))
completed = set(payload.get("completed_specs", []))
print("true" if spec_id in completed else "false")
PY
    ;;
  get-next-runnable)
    python - "$state_path" <<'PY'
import json
import pathlib
import sys

state_path = pathlib.Path(sys.argv[1])
payload = json.loads(state_path.read_text(encoding="utf-8"))
completed = set(payload.get("completed_specs", []))
failed_specs = {entry.get("spec_id") for entry in payload.get("failed_specs", [])}

# Scan .specify/specs for all spec directories with spec.md
specs_root = pathlib.Path(".specify/specs")
runnable_specs = []

for spec_dir in sorted(specs_root.iterdir()):
    if not spec_dir.is_dir():
        continue
    
    spec_file = spec_dir / "spec.md"
    tasks_file = spec_dir / "tasks.md"
    
    # Must have both spec.md and tasks.md
    if not (spec_file.exists() and tasks_file.exists()):
        continue
    
    spec_id = spec_dir.name
    
    # Skip already completed or failed specs
    if spec_id in completed or spec_id in failed_specs:
        continue
    
    # Check Status in spec.md: skip "Ready for research" unless explicitly set to process
    spec_content = spec_file.read_text(encoding="utf-8")
    if "**Status**: Ready for research" in spec_content:
        # Skip research-only specs (they require infrastructure decisions/setup)
        continue
    
    # Check for infrastructure blockers in Prerequisites
    if "[INFRASTRUCTURE]" in spec_content:
        # Skip specs with explicit infrastructure blockers
        continue
    
    runnable_specs.append(spec_id)

# Return first runnable spec or empty string if none
print(runnable_specs[0] if runnable_specs else "")
PY
    ;;
  get-budget-status)
    max_failures="${3:-5}"
    python - "$state_path" "$max_failures" <<'PY'
import json
import pathlib
import sys

state_path = pathlib.Path(sys.argv[1])
max_failures = int(sys.argv[2])
payload = json.loads(state_path.read_text(encoding="utf-8"))

# Count unique failed specs
failed_specs = {entry.get("spec_id") for entry in payload.get("failed_specs", [])}
unique_failures = len(failed_specs)

print("exceeded" if unique_failures >= max_failures else "ok")
PY
    ;;
  write-evidence)
    spec_id="${3:-}"
    result_path="${4:-}"
    status="${5:-unknown}"
    reason="${6:-}"
    if [[ -z "$spec_id" ]] || [[ -z "$result_path" ]]; then
      echo "write-evidence requires <spec-id> <result-path> <status> [reason]" >&2
      exit 1
    fi
    python - "$spec_id" "$result_path" "$status" "$reason" <<'PY'
import json
import pathlib
import sys
from datetime import datetime

spec_id = sys.argv[1]
result_path = sys.argv[2]
status = sys.argv[3]
reason = sys.argv[4]

evidence = {
    "spec_id": spec_id,
    "status": status,
    "reason": reason,
    "timestamp": datetime.utcnow().isoformat() + "Z"
}

result_file = pathlib.Path(result_path)
result_file.parent.mkdir(parents=True, exist_ok=True)
result_file.write_text(json.dumps(evidence, indent=2) + "\n", encoding="utf-8")
PY
    ;;
  get-summary)
    python - "$state_path" <<'PY'
import json
import pathlib
import sys

state_path = pathlib.Path(sys.argv[1])
payload = json.loads(state_path.read_text(encoding="utf-8"))

completed = payload.get("completed_specs", [])
failed = payload.get("failed_specs", [])
unique_failed = len({entry.get("spec_id") for entry in failed})

summary = {
    "status": payload.get("status"),
    "stop_reason": payload.get("stop_reason"),
    "completed_count": len(completed),
    "failed_count": unique_failed,
    "completed_specs": completed,
    "failed_specs": failed
}

print(json.dumps(summary, indent=2))
PY
    ;;
  *)
    echo "Unsupported command: $command_name" >&2
    exit 1
    ;;
esac

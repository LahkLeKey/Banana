#!/usr/bin/env bash
set -euo pipefail

# Lightweight JSON state helper for spec-drain mode.
# Usage:
#   bash scripts/workflow-spec-drain-state.sh init <state-path> <run-id> <run-attempt>
#   bash scripts/workflow-spec-drain-state.sh mark-completed <state-path> <spec-id>
#   bash scripts/workflow-spec-drain-state.sh mark-failed <state-path> <spec-id> <reason>
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
  *)
    echo "Unsupported command: $command_name" >&2
    exit 1
    ;;
esac

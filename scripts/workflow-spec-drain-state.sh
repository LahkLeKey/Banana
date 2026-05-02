#!/usr/bin/env bash
set -euo pipefail

# Lightweight JSON state helper for spec-drain mode.
# Usage:
#   bash scripts/workflow-spec-drain-state.sh init <state-path> <run-id> <run-attempt>
#   bash scripts/workflow-spec-drain-state.sh mark-completed <state-path> <spec-id>
#   bash scripts/workflow-spec-drain-state.sh mark-failed <state-path> <spec-id> <reason>
#   bash scripts/workflow-spec-drain-state.sh set-stop <state-path> <reason>
#   bash scripts/workflow-spec-drain-state.sh get-next-runnable <state-path>
#   bash scripts/workflow-spec-drain-state.sh is-completed <state-path> <spec-id>
#   bash scripts/workflow-spec-drain-state.sh get-budget-status <state-path> <max-failures>
#   bash scripts/workflow-spec-drain-state.sh write-evidence <state-path> <spec-id> <evidence-path> <status> [reason]
#   bash scripts/workflow-spec-drain-state.sh get-summary <state-path>

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

command_name="${1:-}"
state_path="${2:-}"

if [[ -z "$command_name" || -z "$state_path" ]]; then
  echo "Usage: $0 <command> <state-path> [args...]" >&2
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
    # Preserve existing checkpoint for resumability (T016 startup recovery).
    try:
        existing = json.loads(state_path.read_text(encoding="utf-8"))
        if existing.get("mode") == "spec-drain":
            sys.exit(0)
    except (ValueError, KeyError):
        # File is empty or corrupt — overwrite it.
        pass

payload = {
    "schema_version": 2,
    "mode": "spec-drain",
    "run_id": run_id,
    "run_attempt": run_attempt,
    "status": "running",
    "stop_reason": None,
    "completed_specs": [],
    "failed_specs": [],
    "failure_counts": {},
    "evidence": {},
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

# Remove from failed list if it was previously failed (retry succeeded).
payload["failed_specs"] = [
    entry for entry in payload.get("failed_specs", [])
    if str(entry.get("spec_id")) != spec_id
]

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
import datetime

state_path = pathlib.Path(sys.argv[1])
spec_id = sys.argv[2]
reason = sys.argv[3]
payload = json.loads(state_path.read_text(encoding="utf-8"))

# Track failure counts for budget (T006).
counts = payload.setdefault("failure_counts", {})
counts[spec_id] = counts.get(spec_id, 0) + 1

failed_specs = payload.setdefault("failed_specs", [])
# Remove stale entries for same spec_id before appending latest.
failed_specs = [e for e in failed_specs if str(e.get("spec_id")) != spec_id]
failed_specs.append({
    "spec_id": spec_id,
    "reason": reason,
    "attempts": counts[spec_id],
    "failed_at": datetime.datetime.utcnow().isoformat() + "Z",
})
payload["failed_specs"] = failed_specs

state_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
    ;;
  set-stop)
    reason="${3:-unknown}"
    python - "$state_path" "$reason" <<'PY'
import json
import pathlib
import sys
import datetime

state_path = pathlib.Path(sys.argv[1])
reason = sys.argv[2]
payload = json.loads(state_path.read_text(encoding="utf-8"))
payload["status"] = "stopped"
payload["stop_reason"] = reason
payload["stopped_at"] = datetime.datetime.utcnow().isoformat() + "Z"
state_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
    ;;
  get-next-runnable)
    # Returns the next runnable spec ID, or empty string if none.
    python - "$state_path" <<'PY'
import json
import pathlib
import re
import sys

state_path = pathlib.Path(sys.argv[1])
payload = json.loads(state_path.read_text(encoding="utf-8"))

completed = set(payload.get("completed_specs", []))
failed = {str(e.get("spec_id")) for e in payload.get("failed_specs", [])}

spec_dirs = []
for path in sorted(pathlib.Path(".specify/specs").glob("*")):
    if not path.is_dir():
        continue
    name = path.name
    if not re.match(r"^[0-9]{3,}-", name):
        continue
    if (path / "spec.md").exists() and (path / "tasks.md").exists():
        spec_dirs.append(name)

for spec in spec_dirs:
    if spec not in completed and spec not in failed:
        print(spec)
        sys.exit(0)

# No runnable spec found.
print("")
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
  get-budget-status)
    # Returns "ok" or "exceeded" based on total unique failed specs vs max.
    max_failures="${3:-5}"
    python - "$state_path" "$max_failures" <<'PY'
import json
import pathlib
import sys

state_path = pathlib.Path(sys.argv[1])
max_failures = int(sys.argv[2])
payload = json.loads(state_path.read_text(encoding="utf-8"))

unique_failures = len(set(str(e.get("spec_id")) for e in payload.get("failed_specs", [])))
if unique_failures >= max_failures:
    print("exceeded")
else:
    print("ok")
PY
    ;;
  write-evidence)
    # Writes per-spec evidence artifact and records path in state.
    spec_id="${3:-}"
    evidence_path="${4:-}"
    status="${5:-unknown}"
    reason="${6:-}"
    if [[ -z "$spec_id" || -z "$evidence_path" ]]; then
      echo "write-evidence requires <spec-id> <evidence-path> <status> [reason]" >&2
      exit 1
    fi
    python - "$state_path" "$spec_id" "$evidence_path" "$status" "$reason" <<'PY'
import json
import pathlib
import sys
import datetime

state_path = pathlib.Path(sys.argv[1])
spec_id = sys.argv[2]
evidence_path = pathlib.Path(sys.argv[3])
status = sys.argv[4]
reason = sys.argv[5] if len(sys.argv) > 5 else ""

payload = json.loads(state_path.read_text(encoding="utf-8"))

evidence_record = {
    "spec_id": spec_id,
    "status": status,
    "reason": reason,
    "written_at": datetime.datetime.utcnow().isoformat() + "Z",
    "evidence_path": str(evidence_path),
}

evidence_path.parent.mkdir(parents=True, exist_ok=True)
evidence_path.write_text(json.dumps(evidence_record, indent=2) + "\n", encoding="utf-8")

# Record in state.
payload.setdefault("evidence", {})[spec_id] = str(evidence_path)
state_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY
    ;;
  get-summary)
    python - "$state_path" <<'PY'
import json
import pathlib
import re
import sys

state_path = pathlib.Path(sys.argv[1])
payload = json.loads(state_path.read_text(encoding="utf-8"))

spec_dirs = []
for path in sorted(pathlib.Path(".specify/specs").glob("*")):
    if not path.is_dir():
        continue
    name = path.name
    if not re.match(r"^[0-9]{3,}-", name):
        continue
    if (path / "spec.md").exists():
        spec_dirs.append(name)

completed = set(payload.get("completed_specs", []))
failed_specs = payload.get("failed_specs", [])
failed_ids = {str(e.get("spec_id")) for e in failed_specs}
runnable = [s for s in spec_dirs if s not in completed and s not in failed_ids]

summary = {
    "status": payload.get("status", "unknown"),
    "stop_reason": payload.get("stop_reason"),
    "total_specs": len(spec_dirs),
    "completed": len(completed),
    "failed": len(failed_ids),
    "remaining_runnable": len(runnable),
    "runnable_specs": runnable,
    "completed_specs": sorted(completed),
    "failed_specs": failed_specs,
    "evidence": payload.get("evidence", {}),
}
print(json.dumps(summary, indent=2))
PY
    ;;
  *)
    echo "Unsupported command: $command_name" >&2
    exit 1
    ;;
esac

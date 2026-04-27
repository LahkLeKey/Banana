#!/usr/bin/env bash
# Spec 041 — repeat-run profile reproducibility validator.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: scripts/validate-compose-run-profiles.sh --profile <name> [--attempts <n>] [--artifact-root <path>] [--service <name>]
EOF
}

profile=""
service=""
attempts="${BANANA_PROFILE_REPRO_ATTEMPTS:-3}"
artifact_root="${BANANA_PROFILE_REPRO_ARTIFACT_ROOT:-.artifacts/compose-repro}"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --profile)
      profile="${2:-}"
      shift 2
      ;;
    --service)
      service="${2:-}"
      shift 2
      ;;
    --attempts)
      attempts="${2:-3}"
      shift 2
      ;;
    --artifact-root)
      artifact_root="${2:-}"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown argument: $1" >&2
      usage
      exit 2
      ;;
  esac
done

if [[ -z "$profile" ]]; then
  echo "[banana] --profile is required" >&2
  usage
  exit 2
fi

if ! [[ "$attempts" =~ ^[0-9]+$ ]] || (( attempts < 1 )); then
  echo "[banana] --attempts must be a positive integer" >&2
  exit 2
fi

profile_dir="$artifact_root/$profile"
mkdir -p "$profile_dir"
result_file="$profile_dir/reproducibility-result.json"

attempt_records=()
overall_status="pass"

for ((i=1; i<=attempts; i++)); do
  started_at="$(date -u +"%Y-%m-%dT%H:%M:%SZ")"
  if [[ -n "$service" ]]; then
    service_args=(--service "$service")
  else
    service_args=()
  fi

  up_exit=0
  ready_exit=0
  down_exit=0
  reason="success"

  if ! bash scripts/compose-run-profile.sh --profile "$profile" --action up "${service_args[@]}"; then
    up_exit=$?
    reason="startup_failed"
  fi

  if (( up_exit == 0 )); then
    if ! bash scripts/compose-profile-ready.sh --profile "$profile" "${service_args[@]}"; then
      ready_exit=$?
      reason="readiness_failed"
    fi
  fi

  if ! bash scripts/compose-run-profile.sh --profile "$profile" --action down; then
    down_exit=$?
    if [[ "$reason" == "success" ]]; then
      reason="teardown_failed"
    fi
  fi

  finished_at="$(date -u +"%Y-%m-%dT%H:%M:%SZ")"
  status="pass"
  if (( up_exit != 0 || ready_exit != 0 || down_exit != 0 )); then
    status="fail"
    overall_status="fail"
  fi

  attempt_records+=("$i|$status|$reason|$up_exit|$ready_exit|$down_exit|$started_at|$finished_at")
done

python_bin="python3"
if ! command -v "$python_bin" >/dev/null 2>&1; then
  python_bin="python"
fi

if ! command -v "$python_bin" >/dev/null 2>&1; then
  echo "[banana] python runtime not found to write reproducibility result JSON" >&2
  exit 1
fi

records_file="$(mktemp)"
trap 'rm -f "$records_file"' EXIT

{
  printf '%s\n' "$profile"
  printf '%s\n' "$attempts"
  printf '%s\n' "$overall_status"
  for row in "${attempt_records[@]}"; do
    printf '%s\n' "$row"
  done
} > "$records_file"

"$python_bin" - "$result_file" "$records_file" <<'PY'
import json
import sys
from pathlib import Path

result_path = Path(sys.argv[1])
records_path = Path(sys.argv[2])
lines = [line.rstrip("\n") for line in records_path.read_text(encoding="utf-8").splitlines()]
profile = lines[0]
attempt_count = int(lines[1])
overall_status = lines[2]
rows = lines[3:]
attempts = []

for row in rows:
    idx, status, reason, up_exit, ready_exit, down_exit, started_at, finished_at = row.split("|", 7)
    attempts.append(
        {
            "attempt_index": int(idx),
            "status": status,
            "reason": reason,
            "startup_exit_code": int(up_exit),
            "readiness_exit_code": int(ready_exit),
            "teardown_exit_code": int(down_exit),
            "started_at": started_at,
            "finished_at": finished_at,
        }
    )

payload = {
    "profile": profile,
    "attempt_count": attempt_count,
    "status": overall_status,
    "attempts": attempts,
}
result_path.parent.mkdir(parents=True, exist_ok=True)
result_path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
PY

if [[ "$overall_status" != "pass" ]]; then
  echo "[banana] reproducibility validation failed for profile '$profile'" >&2
  echo "[banana] see $result_file" >&2
  exit 1
fi

echo "[banana] reproducibility validation passed for profile '$profile'"
echo "[banana] result: $result_file"

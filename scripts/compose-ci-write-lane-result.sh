#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: compose-ci-write-lane-result.sh --lane <lane> [--status <pass|fail|skip>] [--stage <stage>] [--reason-code <reason>] [--exit-code <code>] [--failing-service <service>] [--profile <profile>] [--started-at <iso-8601>] [--finished-at <iso-8601>] [--diagnostics-bundle-path <relative-path>] [--parity-gate-file <path>] [--artifact-root <path>]
EOF
}

json_escape() {
  local input="$1"
  input="${input//\\/\\\\}"
  input="${input//\"/\\\"}"
  input="${input//$'\n'/\\n}"
  printf '%s' "$input"
}

lane=""
status=""
stage=""
reason_code=""
exit_code=""
failing_service=""
profile="${BANANA_CI_LANE_PROFILE:-}"
started_at="${BANANA_CI_LANE_STARTED_AT:-}"
finished_at="${BANANA_CI_LANE_FINISHED_AT:-}"
diagnostics_bundle_path="${BANANA_CI_LANE_DIAGNOSTICS_BUNDLE_PATH:-}"
parity_gate_file="${BANANA_CI_PARITY_GATE_FILE:-}"
artifact_root="${BANANA_CI_ARTIFACT_ROOT:-.artifacts/compose-ci}"
schema_version="${BANANA_CI_LANE_SCHEMA_VERSION:-1}"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --lane)
      lane="$2"
      shift 2
      ;;
    --status)
      status="$2"
      shift 2
      ;;
    --stage)
      stage="$2"
      shift 2
      ;;
    --reason-code)
      reason_code="$2"
      shift 2
      ;;
    --exit-code)
      exit_code="$2"
      shift 2
      ;;
    --failing-service)
      failing_service="$2"
      shift 2
      ;;
    --profile)
      profile="$2"
      shift 2
      ;;
    --started-at)
      started_at="$2"
      shift 2
      ;;
    --finished-at)
      finished_at="$2"
      shift 2
      ;;
    --diagnostics-bundle-path)
      diagnostics_bundle_path="$2"
      shift 2
      ;;
    --parity-gate-file)
      parity_gate_file="$2"
      shift 2
      ;;
    --artifact-root)
      artifact_root="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Unknown argument: $1"
      usage
      exit 2
      ;;
  esac
done

if [[ -z "$lane" ]]; then
  echo "--lane is required"
  usage
  exit 2
fi

lane_dir="$artifact_root/$lane"
mkdir -p "$lane_dir"

stage_file="$lane_dir/stage.txt"
reason_file="$lane_dir/reason-code.txt"
exit_file="$lane_dir/exit-code.txt"
result_file="$lane_dir/lane-result.json"

if [[ -z "$stage" && -f "$stage_file" ]]; then
  stage="$(<"$stage_file")"
fi

if [[ -z "$reason_code" && -f "$reason_file" ]]; then
  reason_code="$(<"$reason_file")"
fi

if [[ -z "$exit_code" && -f "$exit_file" ]]; then
  exit_code="$(<"$exit_file")"
fi

if [[ -z "$exit_code" ]]; then
  exit_code="1"
fi

if [[ -z "$stage" ]]; then
  stage="unknown"
fi

if [[ -z "$status" ]]; then
  if [[ "$exit_code" == "0" ]]; then
    status="pass"
  else
    status="fail"
  fi
fi

if [[ -z "$reason_code" ]]; then
  if [[ "$status" == "pass" ]]; then
    reason_code="success"
  elif [[ "$status" == "skip" ]]; then
    reason_code="skipped"
  else
    reason_code="unknown_error"
  fi
fi

if [[ -n "$failing_service" ]]; then
  failing_service_json="\"$(json_escape "$failing_service")\""
else
  failing_service_json="null"
fi

if [[ -n "$profile" ]]; then
  profile_json="\"$(json_escape "$profile")\""
else
  profile_json="null"
fi

if [[ -n "$started_at" ]]; then
  started_at_json="\"$(json_escape "$started_at")\""
else
  started_at_json="null"
fi

if [[ -n "$finished_at" ]]; then
  finished_at_json="\"$(json_escape "$finished_at")\""
else
  finished_at_json="null"
fi

if [[ -n "$diagnostics_bundle_path" ]]; then
  diagnostics_bundle_path_json="\"$(json_escape "$diagnostics_bundle_path")\""
else
  diagnostics_bundle_path_json="null"
fi

now_utc="$(date -u +%Y-%m-%dT%H:%M:%SZ)"

parity_gate_json="null"
if [[ "$lane" == "api-parity" && -n "$parity_gate_file" && -f "$parity_gate_file" ]]; then
  python_bin=""
  if command -v python3 >/dev/null 2>&1; then
    python_bin="python3"
  elif command -v python >/dev/null 2>&1; then
    python_bin="python"
  fi

  if [[ -n "$python_bin" ]]; then
    parity_gate_json="$($python_bin - "$parity_gate_file" <<'PY'
import json
import sys

path = sys.argv[1]
with open(path, encoding='utf-8') as handle:
    payload = json.load(handle)

summary = payload.get('unresolved_summary', {}) if isinstance(payload, dict) else {}
result = {
    'decision': payload.get('decision') if isinstance(payload, dict) else None,
    'unresolved_summary': {
        'total_findings': int(summary.get('total_findings', 0) or 0),
        'missing_route': int(summary.get('missing_route', 0) or 0),
        'status_mismatch': int(summary.get('status_mismatch', 0) or 0),
        'shape_mismatch': int(summary.get('shape_mismatch', 0) or 0),
    },
}
print(json.dumps(result))
PY
    )"
  fi
fi

cat > "$result_file" <<EOF
{
  "schema_version": $schema_version,
  "lane_name": "$(json_escape "$lane")",
  "profile": $profile_json,
  "status": "$(json_escape "$status")",
  "stage": "$(json_escape "$stage")",
  "reason_code": "$(json_escape "$reason_code")",
  "exit_code": $exit_code,
  "failing_service": $failing_service_json,
  "started_at": $started_at_json,
  "finished_at": $finished_at_json,
  "diagnostics_bundle_path": $diagnostics_bundle_path_json,
  "parity_gate": $parity_gate_json,
  "recorded_at_utc": "$(json_escape "$now_utc")"
}
EOF

echo "$status" > "$lane_dir/status.txt"
echo "$stage" > "$stage_file"
echo "$reason_code" > "$reason_file"
echo "$exit_code" > "$exit_file"

echo "Wrote lane result to $result_file"

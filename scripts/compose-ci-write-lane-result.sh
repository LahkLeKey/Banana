#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: compose-ci-write-lane-result.sh --lane <lane> [--status <pass|fail|skip>] [--stage <stage>] [--reason-code <reason>] [--exit-code <code>] [--failing-service <service>] [--artifact-root <path>]
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

now_utc="$(date -u +%Y-%m-%dT%H:%M:%SZ)"

cat > "$result_file" <<EOF
{
  "schema_version": $schema_version,
  "lane_name": "$(json_escape "$lane")",
  "status": "$(json_escape "$status")",
  "stage": "$(json_escape "$stage")",
  "reason_code": "$(json_escape "$reason_code")",
  "exit_code": $exit_code,
  "failing_service": $failing_service_json,
  "recorded_at_utc": "$(json_escape "$now_utc")"
}
EOF

echo "$status" > "$lane_dir/status.txt"
echo "$stage" > "$stage_file"
echo "$reason_code" > "$reason_file"
echo "$exit_code" > "$exit_file"

echo "Wrote lane result to $result_file"

#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: compose-ci-preflight.sh --lane <lane> [--profile <compose-profile>] [--entry-script <path>] [--artifact-root <path>]
EOF
}

lane=""
profile=""
entry_script=""
artifact_root="${BANANA_CI_ARTIFACT_ROOT:-.artifacts/compose-ci}"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --lane)
      lane="$2"
      shift 2
      ;;
    --profile)
      profile="$2"
      shift 2
      ;;
    --entry-script)
      entry_script="$2"
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
diagnostics_dir="$lane_dir/diagnostics"
mkdir -p "$diagnostics_dir"

stage_file="$lane_dir/stage.txt"
reason_file="$lane_dir/reason-code.txt"
exit_file="$lane_dir/exit-code.txt"
report_file="$diagnostics_dir/preflight.txt"

{
  echo "lane=$lane"
  echo "profile=${profile:-n/a}"
  echo "entry_script=${entry_script:-n/a}"
  echo "timestamp_utc=$(date -u +%Y-%m-%dT%H:%M:%SZ)"
} > "$report_file"

failures=0

record_ok() {
  echo "ok:$1" >> "$report_file"
}

record_warn() {
  echo "warn:$1" >> "$report_file"
}

record_error() {
  echo "error:$1" >> "$report_file"
  failures=$((failures + 1))
}

require_command() {
  local cmd="$1"
  if command -v "$cmd" >/dev/null 2>&1; then
    record_ok "command:$cmd"
  else
    record_error "missing-command:$cmd"
  fi
}

require_command bash
require_command docker

if docker compose version >> "$report_file" 2>&1; then
  record_ok "docker-compose-v2"
else
  record_error "docker-compose-v2-unavailable"
fi

if [[ -n "$entry_script" ]]; then
  if [[ -f "$entry_script" ]]; then
    record_ok "script-exists:$entry_script"
  else
    record_error "script-missing:$entry_script"
  fi

  if [[ -f "$entry_script" && ! -r "$entry_script" ]]; then
    record_error "script-not-readable:$entry_script"
  fi

  if [[ -f "$entry_script" && ! -x "$entry_script" ]]; then
    record_error "script-not-executable:$entry_script"
  fi

  if [[ -f "$entry_script" ]] && grep -q $'\r' "$entry_script"; then
    record_error "script-contains-crlf:$entry_script"
  fi
fi

if [[ -n "$profile" ]]; then
  if docker compose --profile "$profile" config --services >> "$report_file" 2>&1; then
    record_ok "profile-config:$profile"
  else
    record_error "profile-config-invalid:$profile"
  fi
fi

if (( failures > 0 )); then
  echo "preflight" > "$stage_file"
  echo "preflight_contract_violation" > "$reason_file"
  echo "1" > "$exit_file"
  echo "Preflight failed for lane '$lane'. See $report_file"
  exit 1
fi

echo "preflight" > "$stage_file"
echo "success" > "$reason_file"
echo "0" > "$exit_file"
echo "Preflight passed for lane '$lane'."

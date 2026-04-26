#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: validate-compose-ci-lane-contract.sh --lane <lane> [--artifact-root <path>]
EOF
}

lane=""
artifact_root="${BANANA_CI_ARTIFACT_ROOT:-.artifacts/compose-ci}"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --lane)
      lane="$2"
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
result_file="$lane_dir/lane-result.json"
manifest_file="$lane_dir/artifact-manifest.json"
stage_file="$lane_dir/stage.txt"
reason_file="$lane_dir/reason-code.txt"
exit_file="$lane_dir/exit-code.txt"
preflight_file="$lane_dir/diagnostics/preflight.txt"
compose_ps_file="$lane_dir/diagnostics/compose-ps.txt"

required_files=(
  "$result_file"
  "$manifest_file"
  "$stage_file"
  "$reason_file"
  "$exit_file"
  "$preflight_file"
  "$compose_ps_file"
)

for file in "${required_files[@]}"; do
  if [[ ! -f "$file" ]]; then
    echo "Contract validation failed for lane '$lane': missing required file $file"
    exit 1
  fi
done

python_bin=""
if command -v python3 >/dev/null 2>&1; then
  python_bin="python3"
elif command -v python >/dev/null 2>&1; then
  python_bin="python"
else
  echo "Contract validation failed for lane '$lane': python runtime not found"
  exit 1
fi

"$python_bin" - "$lane" "$result_file" "$manifest_file" "$stage_file" "$reason_file" "$exit_file" <<'PY'
import json
import sys
from datetime import datetime
from pathlib import Path

lane = sys.argv[1]
result_file = Path(sys.argv[2])
manifest_file = Path(sys.argv[3])
stage_file = Path(sys.argv[4])
reason_file = Path(sys.argv[5])
exit_file = Path(sys.argv[6])
lane_dir = result_file.parent
artifact_root = lane_dir.parent

errors = []

with result_file.open("r", encoding="utf-8") as f:
    result = json.load(f)

with manifest_file.open("r", encoding="utf-8") as f:
    manifest = json.load(f)

stage_value = stage_file.read_text(encoding="utf-8").strip()
reason_value = reason_file.read_text(encoding="utf-8").strip()
exit_value_raw = exit_file.read_text(encoding="utf-8").strip()

try:
    exit_value = int(exit_value_raw)
except ValueError:
    errors.append(f"exit-code.txt is not an integer: {exit_value_raw}")
    exit_value = None

allowed_status = {"pass", "fail", "skip"}
allowed_publication = {"uploaded", "fallback-uploaded", "skipped", "failed"}

status = result.get("status")
reason_code = result.get("reason_code")
stage = result.get("stage")
result_exit = result.get("exit_code")
lane_name = result.get("lane_name")

if lane_name != lane:
    errors.append(f"lane_result lane_name '{lane_name}' does not match lane '{lane}'")

if status not in allowed_status:
    errors.append(f"lane status '{status}' is invalid")

if stage != stage_value:
    errors.append("lane-result stage does not match stage.txt")

if reason_code != reason_value:
    errors.append("lane-result reason_code does not match reason-code.txt")

if exit_value is not None and result_exit != exit_value:
    errors.append("lane-result exit_code does not match exit-code.txt")

if status == "pass" and result_exit != 0:
    errors.append("pass status must have exit_code 0")

if status == "fail" and result_exit == 0:
    errors.append("fail status must have non-zero exit_code")

if status == "skip" and (not reason_code or reason_code == "success"):
    errors.append("skip status must include an explicit non-success reason_code")

if result_exit and reason_code == "success":
    errors.append("non-zero exit_code cannot use reason_code 'success'")

publication_status = manifest.get("publication_status")
if publication_status not in allowed_publication:
    errors.append(f"artifact manifest publication_status '{publication_status}' is invalid")

if manifest.get("lane_name") != lane:
    errors.append("artifact manifest lane_name does not match lane")

missing_required = manifest.get("missing_required", [])
if missing_required and publication_status != "failed":
    errors.append("artifact manifest has missing_required paths but publication_status is not 'failed'")

if status == "fail" and publication_status == "uploaded" and missing_required:
    errors.append("failed lane cannot claim uploaded artifact state with missing required paths")


def parse_iso8601(field_name: str, value: object):
  if value is None:
    return None

  if not isinstance(value, str) or not value.strip():
    errors.append(f"lane-result {field_name} must be a non-empty string when present")
    return None

  normalized = value.strip()
  if normalized.endswith("Z"):
    normalized = normalized[:-1] + "+00:00"

  try:
    return datetime.fromisoformat(normalized)
  except ValueError:
    errors.append(f"lane-result {field_name} '{value}' is not a valid ISO-8601 timestamp")
    return None


profile = result.get("profile")
if profile is not None and (not isinstance(profile, str) or not profile.strip()):
  errors.append("lane-result profile must be a non-empty string when present")

started_at = parse_iso8601("started_at", result.get("started_at"))
finished_at = parse_iso8601("finished_at", result.get("finished_at"))
if started_at is not None and finished_at is not None and finished_at < started_at:
  errors.append("lane-result finished_at must be greater than or equal to started_at")

diagnostics_bundle_path = result.get("diagnostics_bundle_path")
if diagnostics_bundle_path is not None:
  if not isinstance(diagnostics_bundle_path, str) or not diagnostics_bundle_path.strip():
    errors.append("lane-result diagnostics_bundle_path must be a non-empty string when present")
  else:
    normalized = diagnostics_bundle_path.strip().replace("\\", "/")
    if normalized.startswith("/") or ":" in normalized:
      errors.append("lane-result diagnostics_bundle_path must be a relative path")
    else:
      candidate_lane = (lane_dir / normalized).resolve()
      candidate_root = (artifact_root / normalized).resolve()
      if not candidate_lane.exists() and not candidate_root.exists():
        errors.append(
          "lane-result diagnostics_bundle_path "
          f"'{diagnostics_bundle_path}' does not exist under lane artifact root"
        )

if errors:
    print(f"Contract validation failed for lane '{lane}':")
    for err in errors:
        print(f"- {err}")
    sys.exit(1)

print(f"Contract validation passed for lane '{lane}'.")
PY

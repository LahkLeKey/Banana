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
coverage_result_file="${BANANA_COVERAGE_ARTIFACT_ROOT:-.artifacts/coverage}/$lane/coverage-lane-result.json"
stage_file="$lane_dir/stage.txt"
reason_file="$lane_dir/reason-code.txt"
exit_file="$lane_dir/exit-code.txt"
preflight_file="$lane_dir/diagnostics/preflight.txt"
compose_ps_file="$lane_dir/diagnostics/compose-ps.txt"
repro_file="$lane_dir/reproducibility-result.json"
require_profile_repro="${BANANA_REQUIRE_PROFILE_REPRO:-false}"

required_files=(
  "$result_file"
  "$manifest_file"
  "$stage_file"
  "$reason_file"
  "$exit_file"
  "$preflight_file"
  "$compose_ps_file"
)

if [[ "$require_profile_repro" == "true" ]]; then
  required_files+=("$repro_file")
fi

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

"$python_bin" - "$lane" "$result_file" "$manifest_file" "$stage_file" "$reason_file" "$exit_file" "$coverage_result_file" "$repro_file" "$require_profile_repro" <<'PY'
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
coverage_result_file = Path(sys.argv[7])
repro_file = Path(sys.argv[8])
require_profile_repro = sys.argv[9].lower() == "true"
lane_dir = result_file.parent
artifact_root = lane_dir.parent
spec_root = Path(".specify/specs/003-coverage-80-rehydration")

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

def validate_coverage_result(path: Path):
  if not path.exists():
    return

  try:
    coverage = json.loads(path.read_text(encoding="utf-8"))
  except Exception as exc:
    errors.append(f"coverage result {path} is not valid JSON: {exc}")
    return

  required_fields = [
    "lane_id",
    "domain",
    "layer",
    "status",
    "threshold_percent",
    "denominator_policy_version",
    "evidence_bundle_path",
  ]

  for field in required_fields:
    if field not in coverage:
      errors.append(f"coverage result missing required field '{field}'")

  coverage_status = coverage.get("status")
  if coverage_status not in {"pass", "fail", "skip", "not-applicable"}:
    errors.append(f"coverage result status '{coverage_status}' is invalid")

  coverage_layer = coverage.get("layer")
  if coverage_layer not in {"unit", "integration", "e2e"}:
    errors.append(f"coverage result layer '{coverage_layer}' is invalid")

  failure_class = coverage.get("failure_class")
  if failure_class is not None and failure_class not in {
    "threshold_violation",
    "coverage_contract_violation",
    "preflight_contract_violation",
    "flake_contract_violation",
  }:
    errors.append(f"coverage result failure_class '{failure_class}' is invalid")

  if coverage_layer == "e2e":
    e2e_manifest = coverage.get("e2e_flow_manifest")
    if not isinstance(e2e_manifest, dict):
      errors.append("coverage result e2e_flow_manifest must be an object for e2e lanes")
    else:
      channel = e2e_manifest.get("channel")
      if channel not in {"api-react", "electron", "mobile"}:
        errors.append(f"e2e_flow_manifest channel '{channel}' is invalid")

      required_flow_count = e2e_manifest.get("required_flow_count")
      declared_flows = e2e_manifest.get("declared_flows")
      exercised_flow_ids = e2e_manifest.get("exercised_flow_ids")
      exercised_percent = e2e_manifest.get("exercised_percent")

      if not isinstance(declared_flows, list) or not declared_flows:
        errors.append("e2e_flow_manifest declared_flows must be a non-empty array")
      if not isinstance(required_flow_count, int):
        errors.append("e2e_flow_manifest required_flow_count must be an integer")
      elif isinstance(declared_flows, list) and required_flow_count != len(declared_flows):
        errors.append("e2e_flow_manifest required_flow_count must equal declared_flows length")
      if not isinstance(exercised_flow_ids, list):
        errors.append("e2e_flow_manifest exercised_flow_ids must be an array")
      if not isinstance(exercised_percent, (int, float)):
        errors.append("e2e_flow_manifest exercised_percent must be numeric")

  evidence_path = coverage.get("evidence_bundle_path")
  if isinstance(evidence_path, str):
    normalized = evidence_path.replace("\\", "/")
    if normalized.startswith("/") or ":" in normalized:
      errors.append("coverage result evidence_bundle_path must be relative")
    if normalized == ".." or normalized.startswith("../") or "/../" in normalized:
      errors.append("coverage result evidence_bundle_path cannot traverse parent directories")
  else:
    errors.append("coverage result evidence_bundle_path must be a string")

  exceptions = coverage.get("exceptions")
  if exceptions is not None:
    if not isinstance(exceptions, dict):
      errors.append("coverage result exceptions must be an object when present")
    else:
      if not isinstance(exceptions.get("valid"), bool):
        errors.append("coverage result exceptions.valid must be boolean")
      if not isinstance(exceptions.get("active_count"), int):
        errors.append("coverage result exceptions.active_count must be integer")
      if not isinstance(exceptions.get("expired_count"), int):
        errors.append("coverage result exceptions.expired_count must be integer")


validate_coverage_result(coverage_result_file)

def validate_reproducibility(path: Path):
  if not path.exists():
    if require_profile_repro:
      errors.append(f"required reproducibility result missing: {path}")
    return

  try:
    repro = json.loads(path.read_text(encoding="utf-8"))
  except Exception as exc:
    errors.append(f"reproducibility result {path} is not valid JSON: {exc}")
    return

  if repro.get("status") not in {"pass", "fail"}:
    errors.append(f"reproducibility result status '{repro.get('status')}' is invalid")

  attempts = repro.get("attempts")
  if not isinstance(attempts, list) or not attempts:
    errors.append("reproducibility result attempts must be a non-empty array")
    return

  for idx, attempt in enumerate(attempts, start=1):
    if not isinstance(attempt, dict):
      errors.append(f"reproducibility attempt #{idx} must be an object")
      continue
    if attempt.get("status") not in {"pass", "fail"}:
      errors.append(f"reproducibility attempt #{idx} has invalid status '{attempt.get('status')}'")


validate_reproducibility(repro_file)

def validate_spec_parity():
  data_model = spec_root / "data-model.md"
  contract = spec_root / "contracts/coverage-lane-contract.md"

  required_tokens = [
    "threshold_violation",
    "coverage_contract_violation",
    "preflight_contract_violation",
    "flake_contract_violation",
    "unit",
    "integration",
    "e2e",
  ]

  for doc_path in (data_model, contract):
    if not doc_path.exists():
      errors.append(f"spec parity file missing: {doc_path}")
      continue

    text = doc_path.read_text(encoding="utf-8", errors="replace")
    for token in required_tokens:
      if token not in text:
        errors.append(f"spec parity token '{token}' missing in {doc_path}")


validate_spec_parity()

if errors:
    print(f"Contract validation failed for lane '{lane}':")
    for err in errors:
        print(f"- {err}")
    sys.exit(1)

print(f"Contract validation passed for lane '{lane}'.")
PY

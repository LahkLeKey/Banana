#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: coverage-normalize-lane-result.sh \
  --lane-id <lane-id> \
  --domain <domain> \
  --layer <unit|integration|e2e> \
  --status <pass|fail|skip|not-applicable> \
  [--measured-percent <number>] \
  [--threshold-percent <number>] \
  [--failure-class <threshold_violation|coverage_contract_violation|preflight_contract_violation|flake_contract_violation>] \
  [--started-at <iso-8601>] \
  [--finished-at <iso-8601>] \
  [--interop-entry-point <symbol>] \
  [--exceptions-report <path>] \
  [--e2e-channel <api-react|electron|mobile>] \
  [--manifest-version <version>] \
  [--declared-flow <flow-id>]... \
  [--exercised-flow-id <flow-id>]... \
  [--flow-manifest-output <path>] \
  [--evidence-bundle-path <relative-path>] \
  [--summary-output <path>] \
  [--output <path>]
EOF
}

json_escape() {
  local input="$1"
  input="${input//\\/\\\\}"
  input="${input//\"/\\\"}"
  input="${input//$'\n'/\\n}"
  printf '%s' "$input"
}

lane_id=""
domain=""
layer=""
status=""
measured_percent=""
threshold_percent="${BANANA_COVERAGE_THRESHOLD_PERCENT:-80}"
failure_class=""
started_at="${BANANA_CI_LANE_STARTED_AT:-}"
finished_at="${BANANA_CI_LANE_FINISHED_AT:-}"
policy_version="${BANANA_COVERAGE_POLICY_VERSION:-v1}"
artifact_root="${BANANA_COVERAGE_ARTIFACT_ROOT:-.artifacts/coverage}"
evidence_bundle_path=""
output=""
interop_entry_points=()
exceptions_report=""
summary_output=""
e2e_channel=""
manifest_version="v1"
declared_flows=()
exercised_flow_ids=()
flow_manifest_output=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --lane-id)
      lane_id="$2"
      shift 2
      ;;
    --domain)
      domain="$2"
      shift 2
      ;;
    --layer)
      layer="$2"
      shift 2
      ;;
    --status)
      status="$2"
      shift 2
      ;;
    --measured-percent)
      measured_percent="$2"
      shift 2
      ;;
    --threshold-percent)
      threshold_percent="$2"
      shift 2
      ;;
    --failure-class)
      failure_class="$2"
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
    --interop-entry-point)
      interop_entry_points+=("$2")
      shift 2
      ;;
    --exceptions-report)
      exceptions_report="$2"
      shift 2
      ;;
    --e2e-channel)
      e2e_channel="$2"
      shift 2
      ;;
    --manifest-version)
      manifest_version="$2"
      shift 2
      ;;
    --declared-flow)
      declared_flows+=("$2")
      shift 2
      ;;
    --exercised-flow-id)
      exercised_flow_ids+=("$2")
      shift 2
      ;;
    --flow-manifest-output)
      flow_manifest_output="$2"
      shift 2
      ;;
    --evidence-bundle-path)
      evidence_bundle_path="$2"
      shift 2
      ;;
    --output)
      output="$2"
      shift 2
      ;;
    --summary-output)
      summary_output="$2"
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

if [[ -z "$lane_id" || -z "$domain" || -z "$layer" || -z "$status" ]]; then
  echo "--lane-id, --domain, --layer, and --status are required"
  usage
  exit 2
fi

case "$layer" in
  unit|integration|e2e) ;;
  *)
    echo "Invalid --layer '$layer'"
    exit 2
    ;;
esac

case "$status" in
  pass|fail|skip|not-applicable) ;;
  *)
    echo "Invalid --status '$status'"
    exit 2
    ;;
esac

if [[ -z "$output" ]]; then
  output="$artifact_root/$lane_id/coverage-lane-result.json"
fi

if [[ -z "$summary_output" ]]; then
  summary_output="$(dirname "$output")/coverage-lane-summary.txt"
fi

if [[ -z "$flow_manifest_output" ]]; then
  flow_manifest_output="$(dirname "$output")/e2e-flow-manifest.json"
fi

if [[ -z "$evidence_bundle_path" ]]; then
  evidence_bundle_path="$artifact_root/$lane_id"
fi

if [[ "$evidence_bundle_path" == /* || "$evidence_bundle_path" == *:* ]]; then
  echo "--evidence-bundle-path must be relative"
  exit 2
fi

evidence_path_normalized="${evidence_bundle_path//\\//}"
if [[ "$evidence_path_normalized" == ../* || "$evidence_path_normalized" == */../* || "$evidence_path_normalized" == ".." ]]; then
  echo "--evidence-bundle-path cannot contain parent-directory traversal"
  exit 2
fi

if [[ -n "$failure_class" ]]; then
  case "$failure_class" in
    threshold_violation|coverage_contract_violation|preflight_contract_violation|flake_contract_violation) ;;
    *)
      echo "Invalid --failure-class '$failure_class'"
      exit 2
      ;;
  esac
fi

deficit_percent=""

if [[ -n "$measured_percent" ]]; then
  deficit_percent="$(python3 - "$measured_percent" "$threshold_percent" <<'PY'
import sys
measured = float(sys.argv[1])
threshold = float(sys.argv[2])
print(f"{max(0.0, threshold - measured):.4f}")
PY
)"
fi

if [[ "$status" == "fail" && -z "$failure_class" ]]; then
  measured_below_threshold="0"
  if [[ -n "$measured_percent" ]]; then
    measured_below_threshold="$(python3 - "$measured_percent" "$threshold_percent" <<'PY'
import sys
print("1" if float(sys.argv[1]) < float(sys.argv[2]) else "0")
PY
    )"
  fi

  if [[ "$measured_below_threshold" == "1" ]]; then
    failure_class="threshold_violation"
  else
    failure_class="coverage_contract_violation"
  fi
fi

if [[ "$status" != "fail" ]]; then
  failure_class=""
fi

e2e_manifest_json="null"
if [[ "$layer" == "e2e" ]]; then
  if [[ -z "$e2e_channel" ]]; then
    case "$lane_id" in
      api-react-e2e|compose-tests)
        e2e_channel="api-react"
        ;;
      electron-e2e|compose-electron)
        e2e_channel="electron"
        ;;
      mobile-e2e|e2e-smoke)
        e2e_channel="mobile"
        ;;
      *)
        e2e_channel="api-react"
        ;;
    esac
  fi

  if [[ ${#declared_flows[@]} -eq 0 ]]; then
    case "$e2e_channel" in
      api-react)
        declared_flows=(api-health api-banana react-home react-search react-render)
        ;;
      electron)
        declared_flows=(electron-bootstrap electron-smoke api-bridge desktop-render)
        ;;
      mobile)
        declared_flows=(mobile-bootstrap mobile-health mobile-banana)
        ;;
      *)
        declared_flows=(generic-e2e-flow)
        ;;
    esac
  fi

  required_flow_count=${#declared_flows[@]}
  exercised_flow_count=${#exercised_flow_ids[@]}

  if [[ -z "$measured_percent" ]]; then
    measured_percent="$(python3 - "$required_flow_count" "$exercised_flow_count" <<'PY'
import sys
required = int(sys.argv[1])
exercised = int(sys.argv[2])
if required <= 0:
    print("0.00")
else:
    print(f"{(exercised / required) * 100:.2f}")
PY
)"
  fi

  flow_meets_threshold="$(python3 - "$measured_percent" "$threshold_percent" <<'PY'
import sys
print("1" if float(sys.argv[1]) >= float(sys.argv[2]) else "0")
PY
)"

  if [[ "$flow_meets_threshold" != "1" && "$status" != "not-applicable" && "$status" != "skip" ]]; then
    status="fail"
    failure_class="threshold_violation"
  elif [[ "$status" == "pass" ]]; then
    failure_class=""
  fi

  declared_flows_json="["
  for idx in "${!declared_flows[@]}"; do
    flow_id="$(json_escape "${declared_flows[$idx]}")"
    if [[ $idx -gt 0 ]]; then
      declared_flows_json+=", "
    fi
    declared_flows_json+="{\"flow_id\": \"$flow_id\", \"description\": \"$flow_id\"}"
  done
  declared_flows_json+="]"

  exercised_flow_ids_json="["
  for idx in "${!exercised_flow_ids[@]}"; do
    flow_id="$(json_escape "${exercised_flow_ids[$idx]}")"
    if [[ $idx -gt 0 ]]; then
      exercised_flow_ids_json+=", "
    fi
    exercised_flow_ids_json+="\"$flow_id\""
  done
  exercised_flow_ids_json+="]"

  mkdir -p "$(dirname "$flow_manifest_output")"
  cat > "$flow_manifest_output" <<EOF
{
  "channel": "$(json_escape "$e2e_channel")",
  "manifest_version": "$(json_escape "$manifest_version")",
  "declared_flows": $declared_flows_json,
  "required_flow_count": $required_flow_count,
  "exercised_flow_ids": $exercised_flow_ids_json,
  "exercised_percent": $measured_percent
}
EOF

  e2e_manifest_json="$(python3 - "$flow_manifest_output" <<'PY'
import json
import sys
print(json.dumps(json.load(open(sys.argv[1], encoding='utf-8'))))
PY
)"
fi

exceptions_valid="true"
exceptions_active_count="0"
exceptions_expired_count="0"
exceptions_source="null"
if [[ -n "$exceptions_report" && -f "$exceptions_report" ]]; then
  read -r exceptions_valid exceptions_active_count exceptions_expired_count exceptions_source_raw < <(
    python3 - "$exceptions_report" <<'PY'
import json
import sys

payload = json.load(open(sys.argv[1], encoding="utf-8"))
valid = "true" if payload.get("valid", True) else "false"
active = str(payload.get("active_count", 0))
expired = str(payload.get("expired_count", 0))
source = payload.get("source", "")
print(valid, active, expired, source)
PY
  )

  exceptions_source_raw="$(printf '%s' "$exceptions_source_raw" | tr -d '\r\n')"
  exceptions_source="\"$(json_escape "$exceptions_source_raw")\""
fi

if [[ -n "$measured_percent" ]]; then
  deficit_percent="$(python3 - "$measured_percent" "$threshold_percent" <<'PY'
import sys
measured = float(sys.argv[1])
threshold = float(sys.argv[2])
print(f"{max(0.0, threshold - measured):.4f}")
PY
)"
fi

output_dir="$(dirname "$output")"
mkdir -p "$output_dir"
mkdir -p "$evidence_path_normalized"

failure_class_json="null"
if [[ -n "$failure_class" ]]; then
  failure_class_json="\"$(json_escape "$failure_class")\""
fi

measured_json="null"
if [[ -n "$measured_percent" ]]; then
  measured_json="$measured_percent"
fi

deficit_json="null"
if [[ -n "$deficit_percent" ]]; then
  deficit_json="$deficit_percent"
fi

started_json="null"
if [[ -n "$started_at" ]]; then
  started_json="\"$(json_escape "$started_at")\""
fi

finished_json="null"
if [[ -n "$finished_at" ]]; then
  finished_json="\"$(json_escape "$finished_at")\""
fi

interop_entry_points_json="[]"
if [[ ${#interop_entry_points[@]} -gt 0 ]]; then
  interop_entry_points_json="["
  for idx in "${!interop_entry_points[@]}"; do
    entry="$(json_escape "${interop_entry_points[$idx]}")"
    if [[ $idx -gt 0 ]]; then
      interop_entry_points_json+=", "
    fi
    interop_entry_points_json+="\"$entry\""
  done
  interop_entry_points_json+="]"
fi

cat > "$output" <<EOF
{
  "schema_version": 1,
  "lane_id": "$(json_escape "$lane_id")",
  "domain": "$(json_escape "$domain")",
  "layer": "$(json_escape "$layer")",
  "status": "$(json_escape "$status")",
  "failure_class": $failure_class_json,
  "measured_percent": $measured_json,
  "threshold_percent": $threshold_percent,
  "deficit_percent": $deficit_json,
  "denominator_policy_version": "$(json_escape "$policy_version")",
  "started_at": $started_json,
  "finished_at": $finished_json,
  "interop_entry_points": $interop_entry_points_json,
  "e2e_flow_manifest": $e2e_manifest_json,
  "evidence_bundle_path": "$(json_escape "$evidence_path_normalized")",
  "exceptions": {
    "valid": $exceptions_valid,
    "active_count": $exceptions_active_count,
    "expired_count": $exceptions_expired_count,
    "source": $exceptions_source
  }
}
EOF

cat > "$summary_output" <<EOF
Coverage Lane Summary
lane_id=$lane_id
domain=$domain
layer=$layer
status=$status
failure_class=${failure_class:-none}
threshold_percent=$threshold_percent
measured_percent=${measured_percent:-n/a}
deficit_percent=${deficit_percent:-n/a}
exceptions_active_count=$exceptions_active_count
exceptions_expired_count=$exceptions_expired_count
e2e_channel=${e2e_channel:-n/a}
flow_manifest_output=${flow_manifest_output:-n/a}
evidence_bundle_path=$evidence_path_normalized
EOF

echo "Wrote normalized coverage lane result to $output"

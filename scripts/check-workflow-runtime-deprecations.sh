#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: check-workflow-runtime-deprecations.sh [--workflow <path>] [--exceptions <path>] [--output <path>] [--text-output <path>]
EOF
}

workflow_path=".github/workflows/compose-ci.yml"
exceptions_path=".github/workflows/runtime-compatibility-exceptions.yml"
output_path=".artifacts/compose-ci/runtime-compatibility/runtime-compatibility.json"
text_output_path=".artifacts/compose-ci/runtime-compatibility/runtime-compatibility.txt"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --workflow)
      workflow_path="$2"
      shift 2
      ;;
    --exceptions)
      exceptions_path="$2"
      shift 2
      ;;
    --output)
      output_path="$2"
      shift 2
      ;;
    --text-output)
      text_output_path="$2"
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

mkdir -p "$(dirname "$output_path")"
mkdir -p "$(dirname "$text_output_path")"

status="pass"
force_node24="false"
exceptions_registry_present="false"
exceptions_count=0

if [[ ! -f "$workflow_path" ]]; then
  status="fail"
  workflow_exists="false"
else
  workflow_exists="true"
  if grep -Eq '^\s*FORCE_JAVASCRIPT_ACTIONS_TO_NODE24:\s*"?true"?\s*$' "$workflow_path"; then
    force_node24="true"
  else
    status="fail"
  fi
fi

if [[ -f "$exceptions_path" ]]; then
  exceptions_registry_present="true"
  exceptions_count="$(grep -Ec '^\s*-\s*id:\s*' "$exceptions_path" || true)"
else
  status="fail"
fi

now_utc="$(date -u +%Y-%m-%dT%H:%M:%SZ)"

cat > "$output_path" <<EOF
{
  "schema_version": 1,
  "workflow_path": "$workflow_path",
  "exceptions_path": "$exceptions_path",
  "generated_at_utc": "$now_utc",
  "status": "$status",
  "workflow_exists": $workflow_exists,
  "force_javascript_actions_to_node24": $force_node24,
  "exceptions_registry_present": $exceptions_registry_present,
  "exceptions_count": $exceptions_count
}
EOF

{
  echo "Workflow Runtime Compatibility"
  echo ""
  echo "generated_at_utc=$now_utc"
  echo "workflow_path=$workflow_path"
  echo "exceptions_path=$exceptions_path"
  echo "status=$status"
  echo "workflow_exists=$workflow_exists"
  echo "force_javascript_actions_to_node24=$force_node24"
  echo "exceptions_registry_present=$exceptions_registry_present"
  echo "exceptions_count=$exceptions_count"
} > "$text_output_path"

cat "$text_output_path"

if [[ "$status" != "pass" ]]; then
  exit 1
fi

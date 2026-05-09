#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(CDPATH="" cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

CONFIDENCE=""
THRESHOLD="80"
STEP="go-copilot-start"
NOTES="startup gate"
HEARTBEAT_PATH=""

usage() {
  cat << 'EOF'
Usage: spec-orchestration-preflight.sh --confidence <0-100> [--threshold <0-100>] [--step <text>] [--notes <text>] [--heartbeat <path>]

Run mandatory orchestration preflight with single-responsibility composition:
1) extension health preflight
2) confidence gate and heartbeat
3) active feature resolution evidence

Exit codes:
  0  preflight complete and autonomous continuation allowed
  42 confidence below threshold (human checkpoint required)
  2  extension health gate failure

Options:
  --confidence <n>     Confidence percentage for next orchestration step (required)
  --threshold <n>      Confidence threshold (default: 80)
  --step <text>        Heartbeat step label (default: go-copilot-start)
  --notes <text>       Additional gate notes (default: startup gate)
  --heartbeat <path>   Optional heartbeat path override
  --help, -h           Show this help text
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --confidence)
      CONFIDENCE="$2"
      shift 2
      ;;
    --threshold)
      THRESHOLD="$2"
      shift 2
      ;;
    --step)
      STEP="$2"
      shift 2
      ;;
    --notes)
      NOTES="$2"
      shift 2
      ;;
    --heartbeat)
      HEARTBEAT_PATH="$2"
      shift 2
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo "ERROR: Unknown option '$1'" >&2
      usage >&2
      exit 1
      ;;
  esac
done

if [[ -z "$CONFIDENCE" ]]; then
  echo "ERROR: --confidence is required" >&2
  usage >&2
  exit 1
fi

if [[ -n "$HEARTBEAT_PATH" ]]; then
  heartbeat_args=(--heartbeat "$HEARTBEAT_PATH")
else
  heartbeat_args=()
fi

# Infrastructure checks: extension-health preflight must pass before orchestration proceeds.
"$SCRIPT_DIR/spec-extension-preflight.sh" --update-first --json

# Domain policy enforcement: confidence threshold with heartbeat evidence.
"$SCRIPT_DIR/spec-confidence-gate.sh" \
  --confidence "$CONFIDENCE" \
  --threshold "$THRESHOLD" \
  --step "$STEP" \
  --notes "$NOTES" \
  "${heartbeat_args[@]}"

# Application orchestration evidence: print resolved feature path for caller traceability.
_paths_output=$(get_feature_paths) || { echo "ERROR: Failed to resolve feature paths" >&2; exit 1; }
eval "$_paths_output"
unset _paths_output

echo "Preflight resolved active feature: $FEATURE_DIR"

#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(CDPATH="" cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

CONFIDENCE=""
STEP=""
THRESHOLD="80"
NOTES=""
HEARTBEAT_PATH=""

usage() {
  cat << 'EOF'
Usage: spec-confidence-gate.sh --confidence <0-100> --step <text> [--threshold <0-100>] [--notes <text>] [--heartbeat <path>]

Enforce Spec Kit confidence gate and append heartbeat entry.

Exit codes:
  0  confidence >= threshold, continue
  42 confidence < threshold, human input required

Options:
  --confidence <n>     Confidence percentage for next step
  --step <text>        Step identifier or description
  --threshold <n>      Confidence threshold (default: 80)
  --notes <text>       Optional notes
  --heartbeat <path>   Optional heartbeat log path override
  --help, -h           Show this help text
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --confidence)
      CONFIDENCE="$2"
      shift 2
      ;;
    --step)
      STEP="$2"
      shift 2
      ;;
    --threshold)
      THRESHOLD="$2"
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

if [[ -z "$CONFIDENCE" || -z "$STEP" ]]; then
  echo "ERROR: --confidence and --step are required" >&2
  usage >&2
  exit 1
fi

if ! [[ "$CONFIDENCE" =~ ^[0-9]+$ ]] || (( CONFIDENCE < 0 || CONFIDENCE > 100 )); then
  echo "ERROR: --confidence must be an integer in [0, 100]" >&2
  exit 1
fi

if ! [[ "$THRESHOLD" =~ ^[0-9]+$ ]] || (( THRESHOLD < 0 || THRESHOLD > 100 )); then
  echo "ERROR: --threshold must be an integer in [0, 100]" >&2
  exit 1
fi

if [[ -n "$HEARTBEAT_PATH" ]]; then
  heartbeat_args=(--heartbeat "$HEARTBEAT_PATH")
else
  heartbeat_args=()
fi

if (( CONFIDENCE < THRESHOLD )); then
  "$SCRIPT_DIR/spec-heartbeat-log.sh" \
    --step "$STEP" \
    --confidence "$CONFIDENCE" \
    --decision "pause" \
    --checkpoint "yes" \
    --notes "Below threshold ${THRESHOLD}%. ${NOTES}" \
    "${heartbeat_args[@]}"

  echo "CONFIDENCE GATE BLOCKED: ${CONFIDENCE}% is below ${THRESHOLD}%" >&2
  echo "Human input required before continuing this orchestration step." >&2
  exit 42
fi

"$SCRIPT_DIR/spec-heartbeat-log.sh" \
  --step "$STEP" \
  --confidence "$CONFIDENCE" \
  --decision "continue" \
  --checkpoint "no" \
  --notes "At or above threshold ${THRESHOLD}%. ${NOTES}" \
  "${heartbeat_args[@]}"

echo "CONFIDENCE GATE PASS: ${CONFIDENCE}% >= ${THRESHOLD}%"

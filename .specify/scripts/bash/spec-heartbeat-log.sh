#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(CDPATH="" cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

STEP=""
CONFIDENCE=""
DECISION=""
CHECKPOINT=""
NOTES=""
HEARTBEAT_PATH=""

usage() {
  cat << 'EOF'
Usage: spec-heartbeat-log.sh --step <text> --confidence <0-100> --decision <continue|pause> --checkpoint <yes|no> [--notes <text>] [--heartbeat <path>]

Append a confidence heartbeat entry to the active feature heartbeat log.

Options:
  --step <text>        Step identifier or description
  --confidence <0-100> Confidence percentage
  --decision <value>   Decision outcome (continue/pause/custom)
  --checkpoint <yes|no>Whether human checkpoint was required
  --notes <text>       Optional notes
  --heartbeat <path>   Optional override heartbeat log path
  --help, -h           Show this help text
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --step)
      STEP="$2"
      shift 2
      ;;
    --confidence)
      CONFIDENCE="$2"
      shift 2
      ;;
    --decision)
      DECISION="$2"
      shift 2
      ;;
    --checkpoint)
      CHECKPOINT="$2"
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

if [[ -z "$STEP" || -z "$CONFIDENCE" || -z "$DECISION" || -z "$CHECKPOINT" ]]; then
  echo "ERROR: --step, --confidence, --decision, and --checkpoint are required" >&2
  usage >&2
  exit 1
fi

if ! [[ "$CONFIDENCE" =~ ^[0-9]+$ ]] || (( CONFIDENCE < 0 || CONFIDENCE > 100 )); then
  echo "ERROR: --confidence must be an integer in [0, 100]" >&2
  exit 1
fi

if [[ -z "$HEARTBEAT_PATH" ]]; then
  _paths_output=$(get_feature_paths) || { echo "ERROR: Failed to resolve feature paths" >&2; exit 1; }
  eval "$_paths_output"
  unset _paths_output
  HEARTBEAT_PATH="$FEATURE_DIR/heartbeat-log.md"
fi

if [[ ! -f "$HEARTBEAT_PATH" ]]; then
  cat > "$HEARTBEAT_PATH" <<'EOF'
# Confidence Heartbeat Log

## Contract

- Record one entry per major orchestration step.
- Include confidence percentage and continuation decision.
- Any value below 80% requires human checkpoint before execution continues.

## Entries

| Timestamp (UTC) | Step | Confidence | Decision | Human Checkpoint | Notes |
|---|---|---:|---|---|---|
EOF
fi

timestamp="$(date -u +"%Y-%m-%dT%H:%M:%SZ")"
printf '| %s | %s | %s | %s | %s | %s |\n' "$timestamp" "$STEP" "$CONFIDENCE" "$DECISION" "$CHECKPOINT" "$NOTES" >> "$HEARTBEAT_PATH"

echo "Heartbeat logged: $HEARTBEAT_PATH"

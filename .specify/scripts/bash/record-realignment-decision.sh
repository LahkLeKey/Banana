#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(CDPATH="" cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

FINDING_ID=""
DECISION_TYPE=""
RATIONALE=""
FOLLOW_UP_DESTINATION=""
DECISION_OWNER=""
OUTPUT_PATH=""
JSON_MODE=false

usage() {
    cat << 'EOF'
Usage: record-realignment-decision.sh --finding-id <id> --decision-type <retain|defer|split|reject> --rationale <text> --decision-owner <owner> [--follow-up-destination <path>] [--output <path>] [--json]

Record one auditable drift realignment decision in markdown and jsonl logs.

Options:
  --finding-id <id>             Drift finding id (required)
  --decision-type <type>        retain|defer|split|reject (required)
  --rationale <text>            Human-readable rationale (required)
  --decision-owner <owner>      Decision owner (required)
  --follow-up-destination <x>   Required for defer and split
  --output <path>               Markdown log path (default: <feature>/decision-log.md)
  --json                        Emit JSON summary
  --help, -h                    Show this help text
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --finding-id)
            FINDING_ID="$2"
            shift 2
            ;;
        --decision-type)
            DECISION_TYPE="$2"
            shift 2
            ;;
        --rationale)
            RATIONALE="$2"
            shift 2
            ;;
        --follow-up-destination)
            FOLLOW_UP_DESTINATION="$2"
            shift 2
            ;;
        --decision-owner)
            DECISION_OWNER="$2"
            shift 2
            ;;
        --output)
            OUTPUT_PATH="$2"
            shift 2
            ;;
        --json)
            JSON_MODE=true
            shift
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

if [[ -z "$FINDING_ID" || -z "$DECISION_TYPE" || -z "$RATIONALE" || -z "$DECISION_OWNER" ]]; then
    echo "ERROR: --finding-id, --decision-type, --rationale, and --decision-owner are required" >&2
    usage >&2
    exit 1
fi

case "$DECISION_TYPE" in
    retain|defer|split|reject)
        ;;
    *)
        echo "ERROR: Invalid decision type '$DECISION_TYPE'" >&2
        exit 1
        ;;
esac

if [[ "$DECISION_TYPE" == "defer" || "$DECISION_TYPE" == "split" ]]; then
    if [[ -z "$FOLLOW_UP_DESTINATION" ]]; then
        echo "ERROR: --follow-up-destination is required for decision type '$DECISION_TYPE'" >&2
        exit 1
    fi
fi

if [[ -z "$OUTPUT_PATH" ]]; then
    _paths_output=$(get_feature_paths) || { echo "ERROR: Failed to resolve feature paths" >&2; exit 1; }
    eval "$_paths_output"
    unset _paths_output
    OUTPUT_PATH="$FEATURE_DIR/decision-log.md"
fi

JSONL_PATH="${OUTPUT_PATH%.md}.jsonl"
TIMESTAMP_UTC="$(date -u +"%Y-%m-%dT%H:%M:%SZ")"

mkdir -p "$(dirname "$OUTPUT_PATH")"

if [[ ! -f "$OUTPUT_PATH" ]]; then
    cat > "$OUTPUT_PATH" << 'EOF'
# Drift Realignment Decision Log

| Timestamp (UTC) | Finding ID | Decision | Owner | Follow-up Destination | Rationale |
| --- | --- | --- | --- | --- | --- |
EOF
fi

safe_rationale=${RATIONALE//|/\\|}
safe_follow_up=${FOLLOW_UP_DESTINATION//|/\\|}

printf '| %s | %s | %s | %s | %s | %s |\n' \
    "$TIMESTAMP_UTC" \
    "$FINDING_ID" \
    "$DECISION_TYPE" \
    "$DECISION_OWNER" \
    "${safe_follow_up:-n/a}" \
    "$safe_rationale" >> "$OUTPUT_PATH"

if has_jq; then
    jq -cn \
        --arg finding_id "$FINDING_ID" \
        --arg decision_type "$DECISION_TYPE" \
        --arg rationale "$RATIONALE" \
        --arg follow_up_destination "$FOLLOW_UP_DESTINATION" \
        --arg decision_owner "$DECISION_OWNER" \
        --arg decision_timestamp_utc "$TIMESTAMP_UTC" \
        '{finding_id:$finding_id,decision_type:$decision_type,rationale:$rationale,follow_up_destination:$follow_up_destination,decision_owner:$decision_owner,decision_timestamp_utc:$decision_timestamp_utc}' >> "$JSONL_PATH"
else
    printf '{"finding_id":"%s","decision_type":"%s","rationale":"%s","follow_up_destination":"%s","decision_owner":"%s","decision_timestamp_utc":"%s"}\n' \
        "$(json_escape "$FINDING_ID")" \
        "$(json_escape "$DECISION_TYPE")" \
        "$(json_escape "$RATIONALE")" \
        "$(json_escape "$FOLLOW_UP_DESTINATION")" \
        "$(json_escape "$DECISION_OWNER")" \
        "$(json_escape "$TIMESTAMP_UTC")" >> "$JSONL_PATH"
fi

if $JSON_MODE; then
    if has_jq; then
        jq -cn \
            --arg output_path "$OUTPUT_PATH" \
            --arg jsonl_path "$JSONL_PATH" \
            --arg finding_id "$FINDING_ID" \
            --arg decision_type "$DECISION_TYPE" \
            --arg decision_owner "$DECISION_OWNER" \
            --arg decision_timestamp_utc "$TIMESTAMP_UTC" \
            '{output_path:$output_path,jsonl_path:$jsonl_path,finding_id:$finding_id,decision_type:$decision_type,decision_owner:$decision_owner,decision_timestamp_utc:$decision_timestamp_utc}'
    else
        printf '{"output_path":"%s","jsonl_path":"%s","finding_id":"%s","decision_type":"%s","decision_owner":"%s","decision_timestamp_utc":"%s"}\n' \
            "$(json_escape "$OUTPUT_PATH")" \
            "$(json_escape "$JSONL_PATH")" \
            "$(json_escape "$FINDING_ID")" \
            "$(json_escape "$DECISION_TYPE")" \
            "$(json_escape "$DECISION_OWNER")" \
            "$(json_escape "$TIMESTAMP_UTC")"
    fi
else
    echo "Recorded realignment decision for $FINDING_ID at $TIMESTAMP_UTC"
    echo "Markdown log: $OUTPUT_PATH"
    echo "JSONL log: $JSONL_PATH"
fi

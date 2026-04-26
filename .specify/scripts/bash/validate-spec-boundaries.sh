#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(CDPATH="" cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

JSON_MODE=false
WARN_ONLY=false
SPEC_PATH=""

usage() {
    cat << 'EOF'
Usage: validate-spec-boundaries.sh [--spec <path>] [--json] [--warn-only]

Validate that a feature spec has explicit and non-empty scope-boundary sections.

Options:
  --spec <path>   Override spec path (default: active feature spec)
  --json          Emit JSON summary
  --warn-only     Do not fail even when validation fails
  --help, -h      Show this help text
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --spec)
            SPEC_PATH="$2"
            shift 2
            ;;
        --json)
            JSON_MODE=true
            shift
            ;;
        --warn-only)
            WARN_ONLY=true
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

if [[ -z "$SPEC_PATH" ]]; then
    _paths_output=$(get_feature_paths) || { echo "ERROR: Failed to resolve feature paths" >&2; exit 1; }
    eval "$_paths_output"
    unset _paths_output
    SPEC_PATH="$FEATURE_SPEC"
fi

if [[ ! -f "$SPEC_PATH" ]]; then
    echo "ERROR: Spec file not found: $SPEC_PATH" >&2
    exit 1
fi

declare -a required_headings=("In Scope" "Out of Scope" "Assumptions")
declare -a missing_headings=()
declare -a empty_sections=()

for heading in "${required_headings[@]}"; do
    line_num=$(grep -n -E "^##[[:space:]]+${heading}([[:space:]]|$)" "$SPEC_PATH" | head -n1 | cut -d: -f1 || true)
    if [[ -z "$line_num" ]]; then
        missing_headings+=("$heading")
        continue
    fi

    if ! awk -v start="$line_num" '
        NR <= start { next }
        /^##[[:space:]]+/ { exit }
        $0 ~ /^[[:space:]]*$/ { next }
        $0 ~ /^[[:space:]]*<!--/ { next }
        { found=1; exit }
        END { exit(found ? 0 : 1) }
    ' "$SPEC_PATH"; then
        empty_sections+=("$heading")
    fi
done

status="pass"
if (( ${#missing_headings[@]} > 0 || ${#empty_sections[@]} > 0 )); then
    status="fail"
fi

if $JSON_MODE; then
    if has_jq; then
        missing_json=$(printf '%s\n' "${missing_headings[@]:-}" | sed '/^$/d' | jq -R . | jq -s .)
        empty_json=$(printf '%s\n' "${empty_sections[@]:-}" | sed '/^$/d' | jq -R . | jq -s .)
        jq -cn \
            --arg spec_path "$SPEC_PATH" \
            --arg status "$status" \
            --argjson missing_headings "$missing_json" \
            --argjson empty_sections "$empty_json" \
            '{spec_path:$spec_path,status:$status,missing_headings:$missing_headings,empty_sections:$empty_sections}'
    else
        printf '{"spec_path":"%s","status":"%s","missing_headings_count":%s,"empty_sections_count":%s}\n' \
            "$(json_escape "$SPEC_PATH")" \
            "$(json_escape "$status")" \
            "${#missing_headings[@]}" \
            "${#empty_sections[@]}"
    fi
else
    echo "Spec boundary validation: $status"
    echo "Spec path: $SPEC_PATH"
    if (( ${#missing_headings[@]} > 0 )); then
        echo "Missing sections: ${missing_headings[*]}"
    fi
    if (( ${#empty_sections[@]} > 0 )); then
        echo "Empty sections: ${empty_sections[*]}"
    fi
fi

if [[ "$status" == "fail" && "$WARN_ONLY" != "true" ]]; then
    exit 1
fi

exit 0

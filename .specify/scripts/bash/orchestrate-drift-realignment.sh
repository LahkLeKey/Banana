#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(CDPATH="" cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

JSON_MODE=false
WARN_ONLY=false
SKIP_TRACEABILITY=false
SKIP_WIKI_CHECK=false
SPEC_PATH=""
TASKS_PATH=""
TRACE_REPORT_PATH=""

usage() {
    cat << 'EOF'
Usage: orchestrate-drift-realignment.sh [--spec <path>] [--tasks <path>] [--trace-report <path>] [--skip-traceability] [--skip-wiki-check] [--warn-only] [--json]

Run local drift-realignment validations in sequence:
1) spec boundary validation
2) task traceability validation
3) wiki allowlist/mirror parity dry-run check

Options:
  --spec <path>          Override spec path (default: active feature spec)
  --tasks <path>         Override tasks path (default: active feature tasks)
  --trace-report <path>  Override traceability report output path
  --skip-traceability    Skip task traceability validation
  --skip-wiki-check      Skip wiki allowlist/parity dry-run check
  --warn-only            Continue and return success even when a check fails
  --json                 Emit JSON summary
  --help, -h             Show this help text
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --spec)
            SPEC_PATH="$2"
            shift 2
            ;;
        --tasks)
            TASKS_PATH="$2"
            shift 2
            ;;
        --trace-report)
            TRACE_REPORT_PATH="$2"
            shift 2
            ;;
        --skip-traceability)
            SKIP_TRACEABILITY=true
            shift
            ;;
        --skip-wiki-check)
            SKIP_WIKI_CHECK=true
            shift
            ;;
        --warn-only)
            WARN_ONLY=true
            shift
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

_paths_output=$(get_feature_paths) || { echo "ERROR: Failed to resolve feature paths" >&2; exit 1; }
eval "$_paths_output"
unset _paths_output

if [[ -z "$SPEC_PATH" ]]; then
    SPEC_PATH="$FEATURE_SPEC"
fi
if [[ -z "$TASKS_PATH" ]]; then
    TASKS_PATH="$TASKS"
fi
if [[ -z "$TRACE_REPORT_PATH" ]]; then
    TRACE_REPORT_PATH="$FEATURE_DIR/traceability-report.md"
fi

overall_status="pass"
spec_status="pass"
traceability_status="skipped"
wiki_status="skipped"

if ! bash "$SCRIPT_DIR/validate-spec-boundaries.sh" --spec "$SPEC_PATH"; then
    spec_status="fail"
    overall_status="fail"
fi

if [[ "$SKIP_TRACEABILITY" != "true" ]]; then
    if [[ -f "$TASKS_PATH" ]]; then
        traceability_status="pass"
        if ! bash "$SCRIPT_DIR/validate-task-traceability.sh" --spec "$SPEC_PATH" --tasks "$TASKS_PATH" --report "$TRACE_REPORT_PATH"; then
            traceability_status="fail"
            overall_status="fail"
        fi
    else
        traceability_status="skipped"
    fi
fi

if [[ "$SKIP_WIKI_CHECK" != "true" ]]; then
    if [[ -f "$REPO_ROOT/scripts/wiki-consume-into-specify.sh" ]]; then
        wiki_status="pass"
        if ! (cd "$REPO_ROOT" && bash scripts/wiki-consume-into-specify.sh --dry-run >/dev/null); then
            wiki_status="fail"
            overall_status="fail"
        fi
    else
        wiki_status="skipped"
    fi
fi

if [[ "$overall_status" == "fail" && "$WARN_ONLY" == "true" ]]; then
    overall_status="warn"
fi

if $JSON_MODE; then
    if has_jq; then
        jq -cn \
            --arg overall_status "$overall_status" \
            --arg spec_status "$spec_status" \
            --arg traceability_status "$traceability_status" \
            --arg wiki_status "$wiki_status" \
            --arg spec_path "$SPEC_PATH" \
            --arg tasks_path "$TASKS_PATH" \
            --arg traceability_report "$TRACE_REPORT_PATH" \
            '{overall_status:$overall_status,spec_status:$spec_status,traceability_status:$traceability_status,wiki_status:$wiki_status,spec_path:$spec_path,tasks_path:$tasks_path,traceability_report:$traceability_report}'
    else
        printf '{"overall_status":"%s","spec_status":"%s","traceability_status":"%s","wiki_status":"%s","spec_path":"%s","tasks_path":"%s","traceability_report":"%s"}\n' \
            "$(json_escape "$overall_status")" \
            "$(json_escape "$spec_status")" \
            "$(json_escape "$traceability_status")" \
            "$(json_escape "$wiki_status")" \
            "$(json_escape "$SPEC_PATH")" \
            "$(json_escape "$TASKS_PATH")" \
            "$(json_escape "$TRACE_REPORT_PATH")"
    fi
else
    echo "Drift realignment orchestration status: $overall_status"
    echo "Spec boundary check: $spec_status"
    echo "Task traceability check: $traceability_status"
    echo "Wiki parity check: $wiki_status"
    echo "Spec path: $SPEC_PATH"
    echo "Tasks path: $TASKS_PATH"
    echo "Traceability report: $TRACE_REPORT_PATH"
fi

if [[ "$overall_status" == "fail" ]]; then
    exit 1
fi

exit 0

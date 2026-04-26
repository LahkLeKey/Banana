#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(CDPATH="" cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

JSON_MODE=false
WARN_ONLY=false
SPEC_PATH=""
TASKS_PATH=""
REPORT_PATH=""

usage() {
    cat << 'EOF'
Usage: validate-task-traceability.sh [--spec <path>] [--tasks <path>] [--report <path>] [--json] [--warn-only]

Validate task-to-requirement and task-to-user-story traceability.

Options:
  --spec <path>     Override spec path (default: active feature spec)
  --tasks <path>    Override tasks path (default: active feature tasks)
  --report <path>   Write markdown report (default: <feature>/traceability-report.md)
  --json            Emit JSON summary
  --warn-only       Do not fail even when drift findings exist
  --help, -h        Show this help text
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
        --report)
            REPORT_PATH="$2"
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

_paths_output=$(get_feature_paths) || { echo "ERROR: Failed to resolve feature paths" >&2; exit 1; }
eval "$_paths_output"
unset _paths_output

if [[ -z "$SPEC_PATH" ]]; then
    SPEC_PATH="$FEATURE_SPEC"
fi
if [[ -z "$TASKS_PATH" ]]; then
    TASKS_PATH="$TASKS"
fi
if [[ -z "$REPORT_PATH" ]]; then
    REPORT_PATH="$FEATURE_DIR/traceability-report.md"
fi

if [[ ! -f "$SPEC_PATH" ]]; then
    echo "ERROR: Spec file not found: $SPEC_PATH" >&2
    exit 1
fi
if [[ ! -f "$TASKS_PATH" ]]; then
    echo "ERROR: Tasks file not found: $TASKS_PATH" >&2
    exit 1
fi

declare -A requirement_lookup=()
declare -A story_lookup=()
declare -A task_lookup=()

declare -a requirement_ids=()
while IFS= read -r req; do
    [[ -z "$req" ]] && continue
    requirement_lookup["$req"]=1
    requirement_ids+=("$req")
done < <(grep -oE 'FR-[0-9]{3}' "$SPEC_PATH" | sort -u)

declare -a story_ids=()
while IFS= read -r story; do
    [[ -z "$story" ]] && continue
    story_lookup["$story"]=1
    story_ids+=("$story")
done < <(grep -oE '^### User Story [0-9]+' "$SPEC_PATH" | awk '{print "US"$4}' | sort -u)

while IFS= read -r line; do
    if [[ "$line" =~ ^-[[:space:]]\[[[:space:]xX]\][[:space:]](T[0-9]{3})[[:space:]] ]]; then
        task_lookup["${BASH_REMATCH[1]}"]=1
    fi
done < "$TASKS_PATH"

declare -a findings=()
declare -a trace_map=()

findings_count=0
total_tasks=0
mapped_tasks=0

add_finding() {
    local category="$1"
    local task_id="$2"
    local details="$3"
    findings_count=$((findings_count + 1))
    local finding_id
    finding_id=$(printf 'DF-%03d' "$findings_count")
    findings+=("$finding_id|$category|$task_id|$details")
}

while IFS= read -r line; do
    if [[ ! "$line" =~ ^-[[:space:]]\[[[:space:]xX]\][[:space:]](T[0-9]{3})[[:space:]](.*)$ ]]; then
        continue
    fi

    total_tasks=$((total_tasks + 1))
    task_id="${BASH_REMATCH[1]}"
    task_body="${BASH_REMATCH[2]}"

    task_has_reference=false
    task_has_valid_reference=false
    task_invalid_reference=false
    primary_ref_type=""
    primary_ref_id=""

    while IFS= read -r fr; do
        [[ -z "$fr" ]] && continue
        task_has_reference=true
        if [[ -n "${requirement_lookup[$fr]:-}" ]]; then
            task_has_valid_reference=true
            if [[ -z "$primary_ref_id" ]]; then
                primary_ref_type="requirement"
                primary_ref_id="$fr"
            fi
        else
            task_invalid_reference=true
            add_finding "out-of-scope" "$task_id" "references unknown requirement $fr"
        fi
    done < <(printf '%s\n' "$task_body" | grep -oE 'FR-[0-9]{3}' | sort -u || true)

    while IFS= read -r us; do
        [[ -z "$us" ]] && continue
        task_has_reference=true
        if [[ -n "${story_lookup[$us]:-}" ]]; then
            task_has_valid_reference=true
            if [[ -z "$primary_ref_id" ]]; then
                primary_ref_type="user_story"
                primary_ref_id="$us"
            fi
        else
            task_invalid_reference=true
            add_finding "out-of-scope" "$task_id" "references unknown user story $us"
        fi
    done < <(printf '%s\n' "$task_body" | grep -oE '\[US[0-9]+\]' | tr -d '[]' | sort -u || true)

    if printf '%s\n' "$task_body" | grep -qi 'depends on'; then
        while IFS= read -r dep; do
            [[ -z "$dep" ]] && continue
            if [[ "$dep" == "$task_id" ]]; then
                continue
            fi
            if [[ -z "${task_lookup[$dep]:-}" ]]; then
                add_finding "dependency-mismatch" "$task_id" "depends on unknown task $dep"
            fi
        done < <(printf '%s\n' "$task_body" | grep -oE 'T[0-9]{3}' | sort -u || true)
    fi

    if [[ "$task_has_reference" != "true" ]]; then
        add_finding "missing-traceability" "$task_id" "missing FR-* or [US*] linkage"
    elif [[ "$task_has_valid_reference" != "true" && "$task_invalid_reference" == "true" ]]; then
        add_finding "missing-traceability" "$task_id" "contains only invalid references"
    elif [[ "$task_has_valid_reference" == "true" ]]; then
        mapped_tasks=$((mapped_tasks + 1))
        trace_map+=("$task_id|$primary_ref_type|$primary_ref_id|approved")
    fi
done < "$TASKS_PATH"

mkdir -p "$(dirname "$REPORT_PATH")"
{
    echo "# Traceability Report"
    echo
    echo "- Spec: $SPEC_PATH"
    echo "- Tasks: $TASKS_PATH"
    echo "- Total tasks: $total_tasks"
    echo "- Mapped tasks: $mapped_tasks"
    echo "- Drift findings: $findings_count"
    echo
    echo "## Traceability Map"
    echo
    echo "| Task ID | Reference Type | Reference ID | Status |"
    echo "| --- | --- | --- | --- |"
    if (( ${#trace_map[@]} == 0 )); then
        echo "| _none_ | _none_ | _none_ | deferred |"
    else
        for entry in "${trace_map[@]}"; do
            IFS='|' read -r task_id ref_type ref_id status <<< "$entry"
            echo "| $task_id | $ref_type | $ref_id | $status |"
        done
    fi
    echo
    echo "## Drift Findings"
    echo
    echo "| Finding ID | Category | Task ID | Details |"
    echo "| --- | --- | --- | --- |"
    if (( ${#findings[@]} == 0 )); then
        echo "| _none_ | _none_ | _none_ | No drift findings |"
    else
        for entry in "${findings[@]}"; do
            IFS='|' read -r finding_id category task_id details <<< "$entry"
            details=${details//|/\\|}
            echo "| $finding_id | $category | $task_id | $details |"
        done
    fi
} > "$REPORT_PATH"

status="pass"
if (( findings_count > 0 )); then
    status="fail"
fi

if $JSON_MODE; then
    if has_jq; then
        jq -cn \
            --arg status "$status" \
            --arg spec_path "$SPEC_PATH" \
            --arg tasks_path "$TASKS_PATH" \
            --arg report_path "$REPORT_PATH" \
            --argjson total_tasks "$total_tasks" \
            --argjson mapped_tasks "$mapped_tasks" \
            --argjson findings_count "$findings_count" \
            '{status:$status,spec_path:$spec_path,tasks_path:$tasks_path,report_path:$report_path,total_tasks:$total_tasks,mapped_tasks:$mapped_tasks,findings_count:$findings_count}'
    else
        printf '{"status":"%s","spec_path":"%s","tasks_path":"%s","report_path":"%s","total_tasks":%s,"mapped_tasks":%s,"findings_count":%s}\n' \
            "$(json_escape "$status")" \
            "$(json_escape "$SPEC_PATH")" \
            "$(json_escape "$TASKS_PATH")" \
            "$(json_escape "$REPORT_PATH")" \
            "$total_tasks" \
            "$mapped_tasks" \
            "$findings_count"
    fi
else
    echo "Task traceability validation: $status"
    echo "Spec path: $SPEC_PATH"
    echo "Tasks path: $TASKS_PATH"
    echo "Report: $REPORT_PATH"
    echo "Total tasks: $total_tasks"
    echo "Mapped tasks: $mapped_tasks"
    echo "Drift findings: $findings_count"
fi

if [[ "$status" == "fail" && "$WARN_ONLY" != "true" ]]; then
    exit 1
fi

exit 0

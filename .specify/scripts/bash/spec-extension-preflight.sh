#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(CDPATH="" cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/common.sh"

AUTO_REMEDIATE=false
DEFER_FILE=""
JSON_MODE=false
UPDATE_FIRST=false
HEARTBEAT_PATH=""
HEARTBEAT_STEP=""
HEARTBEAT_CONFIDENCE=""

usage() {
  cat << 'EOF'
Usage: spec-extension-preflight.sh [--auto-remediate] [--defer-file <path>] [--update-first] [--json] [--heartbeat <path>] [--heartbeat-step <text>] [--heartbeat-confidence <0-100>]

Validate Spec Kit extension health before orchestration.

Policy:
- Corrupted required extension state blocks orchestration.
- If --auto-remediate is enabled, the script attempts remove+add repair.
- If deferment is needed, --defer-file must include OWNER= and EXPIRY= entries.

Options:
  --auto-remediate      Attempt remediation for corrupted extensions
  --defer-file <path>   Approved deferment record with OWNER and EXPIRY
  --update-first        Run 'specify extension update' before health checks
  --json                Emit JSON summary
  --heartbeat <path>    Optional heartbeat log path override
  --heartbeat-step <t>  Optional heartbeat step label
  --heartbeat-confidence <n> Optional confidence value for heartbeat entry
  --help, -h            Show this help text
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --auto-remediate)
      AUTO_REMEDIATE=true
      shift
      ;;
    --defer-file)
      DEFER_FILE="$2"
      shift 2
      ;;
    --update-first)
      UPDATE_FIRST=true
      shift
      ;;
    --json)
      JSON_MODE=true
      shift
      ;;
    --heartbeat)
      HEARTBEAT_PATH="$2"
      shift 2
      ;;
    --heartbeat-step)
      HEARTBEAT_STEP="$2"
      shift 2
      ;;
    --heartbeat-confidence)
      HEARTBEAT_CONFIDENCE="$2"
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

if ! command -v specify >/dev/null 2>&1; then
  echo "ERROR: specify CLI not found" >&2
  exit 1
fi

if [[ "$UPDATE_FIRST" == "true" ]]; then
  PYTHONIOENCODING=utf-8 PYTHONUTF8=1 specify extension update >/dev/null 2>&1 || true
fi

collect_corrupted_from_registry() {
  local repo_root="$1"
  local registry_file="$repo_root/.specify/extensions/.registry"

  if [[ ! -f "$registry_file" ]]; then
    return 0
  fi

  if command -v python3 >/dev/null 2>&1; then
    python3 - <<'PY' "$repo_root" "$registry_file"
import json
import os
import sys

repo_root = sys.argv[1]
registry_path = sys.argv[2]

with open(registry_path, "r", encoding="utf-8") as f:
    data = json.load(f)

extensions = data.get("extensions", {})
for ext_id, meta in extensions.items():
    if not meta.get("enabled", False):
        continue

    ext_dir = os.path.join(repo_root, ".specify", "extensions", ext_id)
    manifest = os.path.join(ext_dir, "extension.yml")
    if not os.path.isdir(ext_dir) or not os.path.isfile(manifest):
        print(ext_id)
        continue

    registered_commands = meta.get("registered_commands", {})
    has_registered = False
    for command_group in registered_commands.values():
        if command_group:
            has_registered = True
            break

    if not has_registered and not meta.get("registered_skills", []):
        print(ext_id)
PY
    return
  fi

  # Fallback without python3/jq: minimal manifest existence checks for known enabled IDs.
  if command -v jq >/dev/null 2>&1; then
    while IFS= read -r ext_id; do
      [[ -z "$ext_id" ]] && continue
      if [[ ! -d "$repo_root/.specify/extensions/$ext_id" || ! -f "$repo_root/.specify/extensions/$ext_id/extension.yml" ]]; then
        echo "$ext_id"
      fi
    done < <(jq -r '.extensions | to_entries[] | select(.value.enabled == true) | .key' "$registry_file")
  fi
}

_paths_output=$(get_feature_paths) || { echo "ERROR: Failed to resolve feature paths" >&2; exit 1; }
eval "$_paths_output"
unset _paths_output

# Best-effort CLI probe (can fail on some Windows encodings due to rich unicode output).
list_output="$(PYTHONIOENCODING=utf-8 PYTHONUTF8=1 specify extension list 2>&1 || true)"

mapfile -t corrupted_from_list < <(printf '%s\n' "$list_output" | awk '
  /^  ✗ / {
    line=$0
    sub(/^  ✗ /, "", line)
    sub(/ \(.*/, "", line)
    print line
  }
')

mapfile -t corrupted_from_registry < <(collect_corrupted_from_registry "$REPO_ROOT")

declare -A seen_corrupted=()
for ext in "${corrupted_from_list[@]}" "${corrupted_from_registry[@]}"; do
  [[ -n "$ext" ]] || continue
  seen_corrupted["$ext"]=1
done

corrupted_ids=()
for ext in "${!seen_corrupted[@]}"; do
  corrupted_ids+=("$ext")
done

status="pass"
action="none"

if (( ${#corrupted_ids[@]} > 0 )); then
  status="fail"
  if [[ "$AUTO_REMEDIATE" == "true" ]]; then
    action="auto-remediate"
    for ext in "${corrupted_ids[@]}"; do
      printf 'y\n' | specify extension remove "$ext" >/dev/null
      specify extension add "$ext" >/dev/null
    done

    mapfile -t still_bad < <(collect_corrupted_from_registry "$REPO_ROOT")

    if (( ${#still_bad[@]} == 0 )); then
      status="pass"
      action="repaired"
    else
      corrupted_ids=("${still_bad[@]}")
      status="fail"
      action="repair-failed"
    fi
  fi

  if [[ "$status" == "fail" && -n "$DEFER_FILE" ]]; then
    if [[ ! -f "$DEFER_FILE" ]]; then
      echo "ERROR: defer file not found: $DEFER_FILE" >&2
      exit 1
    fi

    if grep -q '^OWNER=' "$DEFER_FILE" && grep -q '^EXPIRY=' "$DEFER_FILE"; then
      status="deferred"
      action="deferred"
    else
      echo "ERROR: defer file must include OWNER= and EXPIRY=" >&2
      exit 1
    fi
  fi
fi

if [[ "$JSON_MODE" == "true" ]]; then
  if has_jq; then
    ids_json=$(printf '%s\n' "${corrupted_ids[@]:-}" | sed '/^$/d' | jq -R . | jq -s .)
    jq -cn --arg status "$status" --arg action "$action" --argjson corrupted "$ids_json" \
      '{status:$status,action:$action,corrupted_extensions:$corrupted}'
  else
    printf '{"status":"%s","action":"%s","corrupted_count":%d}\n' "$status" "$action" "${#corrupted_ids[@]}"
  fi
else
  echo "Extension preflight status: $status"
  echo "Action: $action"
  if (( ${#corrupted_ids[@]} > 0 )); then
    echo "Corrupted extensions: ${corrupted_ids[*]}"
  fi
fi

if [[ "$status" == "fail" ]]; then
  if [[ -n "$HEARTBEAT_STEP" ]]; then
    heartbeat_args=(--step "$HEARTBEAT_STEP" --decision "pause" --checkpoint "yes" --notes "Extension preflight failed: $action")
    if [[ -n "$HEARTBEAT_CONFIDENCE" ]]; then
      heartbeat_args+=(--confidence "$HEARTBEAT_CONFIDENCE")
    else
      heartbeat_args+=(--confidence "0")
    fi
    if [[ -n "$HEARTBEAT_PATH" ]]; then
      heartbeat_args+=(--heartbeat "$HEARTBEAT_PATH")
    fi
    "$SCRIPT_DIR/spec-heartbeat-log.sh" "${heartbeat_args[@]}"
  fi
  echo "ERROR: Extension health gate failed" >&2
  exit 2
fi

if [[ -n "$HEARTBEAT_STEP" ]]; then
  heartbeat_args=(--step "$HEARTBEAT_STEP" --decision "continue" --checkpoint "no" --notes "Extension preflight passed")
  if [[ -n "$HEARTBEAT_CONFIDENCE" ]]; then
    heartbeat_args+=(--confidence "$HEARTBEAT_CONFIDENCE")
  else
    heartbeat_args+=(--confidence "100")
  fi
  if [[ -n "$HEARTBEAT_PATH" ]]; then
    heartbeat_args+=(--heartbeat "$HEARTBEAT_PATH")
  fi
  "$SCRIPT_DIR/spec-heartbeat-log.sh" "${heartbeat_args[@]}"
fi

exit 0

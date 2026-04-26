#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

UV_TOOL_BIN="${BANANA_UV_TOOL_BIN:-/c/Users/Zephr/.local/bin}"
if [[ -d "$UV_TOOL_BIN" ]]; then
  export PATH="$UV_TOOL_BIN:$PATH"
fi

WORKFLOW_ID_DEFAULT="${BANANA_SPECIFY_WORKFLOW_ID:-drift-realignment}"
INTEGRATION_DEFAULT="${BANANA_SPECIFY_INTEGRATION:-copilot}"

usage() {
  cat <<'EOF'
Usage: bash scripts/workflow-specify.sh <command> [options]

Commands:
  doctor
    Ensure Spec Kit preflight passes and report parsed feature/workflow state.

  status
    Show parsed active feature directory and installed workflow IDs.

  paths
    Show parsed repository, branch, and feature paths from check-prerequisites.

  run-workflow <spec text> [workflow-id] [integration]
    Run specify workflow with non-interactive inputs.

  run-full <spec text>
    Shortcut for run-workflow using defaults:
      workflow-id = BANANA_SPECIFY_WORKFLOW_ID (default drift-realignment)
      integration = BANANA_SPECIFY_INTEGRATION (default copilot)

Environment variables:
  BANANA_UV_TOOL_BIN           uv tool bin dir (default /c/Users/Zephr/.local/bin)
  BANANA_SPECIFY_WORKFLOW_ID   default workflow id for run-full
  BANANA_SPECIFY_INTEGRATION   default integration for run-full

Examples:
  bash scripts/workflow-specify.sh doctor
  bash scripts/workflow-specify.sh status
  bash scripts/workflow-specify.sh run-full "Realign API coverage triage"
  bash scripts/workflow-specify.sh run-workflow "Improve branch hooks" speckit copilot
EOF
}

require_specify() {
  if ! command -v specify >/dev/null 2>&1; then
    echo "ERROR: specify CLI is not on PATH." >&2
    echo "Run: export PATH=\"${UV_TOOL_BIN}:\$PATH\"" >&2
    echo "Then install or verify: uv tool install specify-cli --from git+https://github.com/github/spec-kit.git@v0.8.1" >&2
    exit 1
  fi
}

require_python() {
  if command -v python3 >/dev/null 2>&1; then
    echo "python3"
    return 0
  fi

  if command -v python >/dev/null 2>&1; then
    echo "python"
    return 0
  fi

  echo "ERROR: python3/python is required for parser steps." >&2
  exit 1
}

ensure_prereqs() {
  bash scripts/workflow-ensure-speckit.sh
  require_specify
}

print_paths_json() {
  local python_bin
  python_bin="$(require_python)"
  local payload
  payload="$(bash .specify/scripts/bash/check-prerequisites.sh --json --paths-only)"
  "$python_bin" - "$payload" <<'PY'
import json
import sys

data = json.loads(sys.argv[1])
print("Repo Root    :", data.get("REPO_ROOT", ""))
print("Branch       :", data.get("BRANCH", ""))
print("Feature Dir  :", data.get("FEATURE_DIR", ""))
print("Spec File    :", data.get("FEATURE_SPEC", ""))
print("Plan File    :", data.get("IMPL_PLAN", ""))
print("Tasks File   :", data.get("TASKS", ""))
PY
}

print_status_summary() {
  local python_bin
  python_bin="$(require_python)"
  "$python_bin" <<'PY'
import json
from pathlib import Path

feature_path = Path(".specify/feature.json")
registry_path = Path(".specify/workflows/workflow-registry.json")

active_feature = "(missing .specify/feature.json)"
if feature_path.exists():
    try:
        active_feature = json.loads(feature_path.read_text(encoding="utf-8")).get("feature_directory", "(unset)")
    except Exception as exc:  # pragma: no cover
        active_feature = f"(parse error: {exc})"

workflow_ids = []
if registry_path.exists():
    try:
        workflow_ids = sorted((json.loads(registry_path.read_text(encoding="utf-8")).get("workflows") or {}).keys())
    except Exception:
        workflow_ids = []

print("Active feature directory:", active_feature)
print("Installed workflow IDs:", ", ".join(workflow_ids) if workflow_ids else "(none)")
PY
}

run_workflow() {
  local spec_text="$1"
  local workflow_id="$2"
  local integration="$3"

  if [[ -z "$spec_text" ]]; then
    echo "ERROR: spec text is required." >&2
    exit 1
  fi

  specify workflow run "$workflow_id" \
    -i "spec=$spec_text" \
    -i "integration=$integration"
}

cmd="${1:-}"
if [[ -z "$cmd" ]]; then
  usage
  exit 1
fi
shift || true

case "$cmd" in
  doctor)
    ensure_prereqs
    specify check
    echo
    echo "-- Parsed Paths --"
    print_paths_json
    echo
    echo "-- Parsed Status --"
    print_status_summary
    echo
    echo "-- Available Workflows --"
    specify workflow list
    ;;
  status)
    ensure_prereqs
    print_status_summary
    ;;
  paths)
    ensure_prereqs
    print_paths_json
    ;;
  run-workflow)
    ensure_prereqs
    if [[ $# -lt 1 ]]; then
      echo "ERROR: run-workflow requires <spec text>." >&2
      usage
      exit 1
    fi
    spec_text="$1"
    workflow_id="${2:-$WORKFLOW_ID_DEFAULT}"
    integration="${3:-$INTEGRATION_DEFAULT}"
    run_workflow "$spec_text" "$workflow_id" "$integration"
    ;;
  run-full)
    ensure_prereqs
    if [[ $# -lt 1 ]]; then
      echo "ERROR: run-full requires <spec text>." >&2
      usage
      exit 1
    fi
    spec_text="$1"
    run_workflow "$spec_text" "$WORKFLOW_ID_DEFAULT" "$INTEGRATION_DEFAULT"
    ;;
  help|-h|--help)
    usage
    ;;
  *)
    echo "ERROR: Unknown command '$cmd'" >&2
    usage
    exit 1
    ;;
esac

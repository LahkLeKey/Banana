#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

event_name="${GITHUB_EVENT_NAME:-}"
base_ref=""
head_ref=""

if [[ "$event_name" == "pull_request" ]]; then
  base_ref="${PR_BASE_SHA:-${GITHUB_BASE_REF:-}}"
  head_ref="${PR_HEAD_SHA:-${GITHUB_SHA:-}}"
elif [[ "$event_name" == "push" ]]; then
  base_ref="${PUSH_BEFORE_SHA:-}"
  head_ref="${PUSH_SHA:-${GITHUB_SHA:-}}"
fi

if [[ -z "$base_ref" || -z "$head_ref" || "$base_ref" =~ ^0+$ ]]; then
  if git rev-parse --verify HEAD~1 >/dev/null 2>&1; then
    base_ref="HEAD~1"
    head_ref="HEAD"
  else
    echo "[031-guard] no comparable git range available; skipping guard"
    exit 0
  fi
fi

if ! git rev-parse --verify "$base_ref" >/dev/null 2>&1; then
  echo "[031-guard] base ref '$base_ref' not found; skipping guard"
  exit 0
fi

if ! git rev-parse --verify "$head_ref" >/dev/null 2>&1; then
  echo "[031-guard] head ref '$head_ref' not found; skipping guard"
  exit 0
fi

changed_files="$(git diff --name-only "$base_ref" "$head_ref" || true)"

if [[ -z "$changed_files" ]]; then
  echo "[031-guard] no changed files in range $base_ref..$head_ref"
  exit 0
fi

declare -a violations=()

while IFS= read -r path; do
  [[ -z "$path" ]] && continue

  if [[ "$path" =~ ^\.specify/specs/([0-9]{3})-([^/]+)/ ]]; then
    spec_num="${BASH_REMATCH[1]}"
    spec_slug="${BASH_REMATCH[2]}"
    spec_num_int=$((10#$spec_num))

    # Legacy cohesion-lane slices are superseded and must not be extended.
    if (( spec_num_int >= 15 && spec_num_int <= 30 )); then
      violations+=("$path (superseded slice ${spec_num}-${spec_slug} touched)")
      continue
    fi

    # New coherent/cohesion slices must not fork from the 031 ownership path.
    if (( spec_num_int > 31 )); then
      if [[ "$spec_slug" =~ (coherent|cohesion|continuity|fusion|synthesis|replay|convergence|reconciliation|observation|game-world) ]]; then
        violations+=("$path (new coherent/cohesion slice ${spec_num}-${spec_slug} detected; extend 031 instead)")
      fi
    fi
  fi
done <<< "$changed_files"

if (( ${#violations[@]} > 0 )); then
  echo "[031-guard] blocked: coherent-world work must extend .specify/specs/031-unified-coherent-world"
  printf ' - %s\n' "${violations[@]}"
  exit 1
fi

echo "[031-guard] pass: no superseded/new coherent-lane slice drift detected"

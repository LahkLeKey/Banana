#!/usr/bin/env bash
# Pre-flight: scan .github/workflows/orchestrate-*.yml and the shell scripts
# they invoke for (bash|python|cp) <path> references that point inside the
# repository (scripts/, src/, data/, docs/, .specify/) and exit non-zero if
# any of those paths are missing on disk.
#
# Wired into the AI-contracts CI lane so future "trim" PRs cannot delete a
# referenced asset without a failing PR check (see spec 049).
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

WORKFLOW_GLOB=".github/workflows/orchestrate-*.yml"
PREFIXES_REGEX='(scripts/|src/|data/|docs/|\.specify/scripts/)'

# Collect orchestrator workflow files.
mapfile -t WORKFLOWS < <(ls $WORKFLOW_GLOB 2>/dev/null || true)
if [[ "${#WORKFLOWS[@]}" -eq 0 ]]; then
  echo "validate-workflow-dependencies: no workflow files matched $WORKFLOW_GLOB"
  exit 0
fi

declare -A SEEN_REFS=()
declare -a MISSING=()
declare -a SCRIPTS_TO_SCAN=()

extract_refs_from_file() {
  # Print one "<source>|<path>" line per matched reference.
  local source_label="$1"
  local file="$2"
  [[ -f "$file" ]] || return 0
  # Strip comments, then match (bash|python|python3|cp) <path>.
  grep -oE "(bash|python3?|cp) +${PREFIXES_REGEX}[A-Za-z0-9_./-]+(\.(sh|py|json|h|c|cli))?" "$file" 2>/dev/null \
    | awk -v s="$source_label" '{ for(i=2;i<=NF;i++){print s "|" $i; break } }' \
    | sort -u
}

# Pass 1: extract refs from workflow YAMLs.
for wf in "${WORKFLOWS[@]}"; do
  while IFS= read -r line; do
    [[ -z "$line" ]] && continue
    src="${line%%|*}"
    ref="${line#*|}"
    SEEN_REFS["$src|$ref"]=1
    if [[ "$ref" == scripts/*.sh || "$ref" == .specify/scripts/* ]]; then
      SCRIPTS_TO_SCAN+=("$ref")
    fi
  done < <(extract_refs_from_file "$wf" "$wf")
done

# Pass 2: scan invoked shell scripts (deduplicated).
declare -A SCANNED=()
queue=("${SCRIPTS_TO_SCAN[@]+"${SCRIPTS_TO_SCAN[@]}"}")
while [[ "${#queue[@]}" -gt 0 ]]; do
  next="${queue[0]}"
  queue=("${queue[@]:1}")
  [[ -n "${SCANNED[$next]:-}" ]] && continue
  SCANNED["$next"]=1
  [[ -f "$next" ]] || continue
  while IFS= read -r line; do
    [[ -z "$line" ]] && continue
    src="${line%%|*}"
    ref="${line#*|}"
    SEEN_REFS["$src|$ref"]=1
    if [[ "$ref" == scripts/*.sh || "$ref" == .specify/scripts/* ]]; then
      queue+=("$ref")
    fi
  done < <(extract_refs_from_file "$next" "$next")
done

# Pass 3: classify each reference.
total=0
for key in "${!SEEN_REFS[@]}"; do
  total=$((total + 1))
  src="${key%%|*}"
  ref="${key#*|}"
  if [[ ! -e "$ref" ]]; then
    MISSING+=("$src -> $ref: missing")
  fi
done

if [[ "${#MISSING[@]}" -gt 0 ]]; then
  printf '%s\n' "${MISSING[@]}" | sort -u
  echo
  echo "validate-workflow-dependencies: ${#MISSING[@]} missing reference(s) across ${#WORKFLOWS[@]} orchestrator workflow(s) (${total} total references checked)"
  exit 1
fi

echo "validate-workflow-dependencies: ok (${total} references checked across ${#WORKFLOWS[@]} workflow(s))"
exit 0

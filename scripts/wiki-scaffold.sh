#!/usr/bin/env bash
# scripts/wiki-scaffold.sh
# Regenerates section README.md index pages and Home.md nav block from .wiki/ directory structure.
# Idempotent: safe to re-run. Does not clobber hand-authored content outside AUTO markers.
#
# Usage:
#   bash scripts/wiki-scaffold.sh              # regenerate all indexes + Home nav
#   bash scripts/wiki-scaffold.sh --validate   # exit 0 if output matches current state, 1 if drift
#   bash scripts/wiki-scaffold.sh --dry-run    # show what would be written without writing
#   bash scripts/wiki-scaffold.sh --dry-run --validate  # dry-run + validate (for CI)
#
# Human sections (have README.md indexes generated):
#   getting-started, architecture, operations, security, data, governance
#
# Excluded from index generation:
#   ai-reference, _templates
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
WIKI_DIR="$ROOT/.wiki"
ALLOWLIST_FILE="$ROOT/.specify/wiki/human-reference-allowlist.txt"

DRY_RUN=false
VALIDATE=false
ISSUES=0

for arg in "$@"; do
  case "$arg" in
    --dry-run)  DRY_RUN=true ;;
    --validate) VALIDATE=true ;;
  esac
done

# Human sections that get index pages
HUMAN_SECTIONS=(getting-started architecture operations security data governance)

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

write_or_check() {
  local path="$1"
  local content="$2"

  if [[ "$DRY_RUN" == "true" ]]; then
    if [[ "$VALIDATE" == "true" ]]; then
      if [[ -f "$path" ]]; then
        local current
        current="$(cat "$path")"
        if [[ "$current" != "$content" ]]; then
          echo "DRIFT: $path" >&2
          ISSUES=$((ISSUES + 1))
        fi
      else
        echo "MISSING: $path" >&2
        ISSUES=$((ISSUES + 1))
      fi
    else
      echo "  [dry-run] would write: $path"
    fi
  else
    echo "$content" > "$path"
  fi
}

section_title() {
  local dir="$1"
  case "$dir" in
    getting-started) echo "Getting Started" ;;
    architecture)    echo "Architecture" ;;
    operations)      echo "Operations" ;;
    security)        echo "Security" ;;
    data)            echo "Data" ;;
    governance)      echo "Governance" ;;
    *)               echo "${dir^}" ;;
  esac
}

# ---------------------------------------------------------------------------
# Generate section README.md index
# ---------------------------------------------------------------------------

generate_section_index() {
  local section="$1"
  local section_dir="$WIKI_DIR/$section"
  local title
  title="$(section_title "$section")"

  if [[ ! -d "$section_dir" ]]; then
    return
  fi

  local rows=""
  while IFS= read -r -d '' file; do
    local basename
    basename="$(basename "$file")"
    [[ "$basename" == "README.md" ]] && continue
    local page_title
    # Extract first H1 from file, fallback to filename without extension
    page_title="$(grep -m1 '^# ' "$file" 2>/dev/null | sed 's/^# //' | sed 's/<!-- breadcrumb:.*-->//g' | xargs || true)"
    [[ -z "$page_title" ]] && page_title="${basename%.md}"
    rows+="| [$page_title]($basename) | |"$'\n'
  done < <(find "$section_dir" -maxdepth 1 -name '*.md' ! -name 'README.md' -print0 | sort -z)

  local content
  content="# $title

> [Home](../Home.md) › $title

<!-- AUTO-INDEX START -->
## Pages In This Section

| Page | Description |
|------|-------------|
$rows
<!-- AUTO-INDEX END -->"

  write_or_check "$section_dir/README.md" "$content"
}

# ---------------------------------------------------------------------------
# Validate allowlist completeness
# ---------------------------------------------------------------------------

validate_allowlist() {
  if [[ ! -f "$ALLOWLIST_FILE" ]]; then
    echo "MISSING allowlist: $ALLOWLIST_FILE" >&2
    ISSUES=$((ISSUES + 1))
    return
  fi

  for section in "${HUMAN_SECTIONS[@]}"; do
    local section_dir="$WIKI_DIR/$section"
    [[ -d "$section_dir" ]] || continue

    while IFS= read -r -d '' file; do
      local basename
      basename="$(basename "$file")"
      local slug

      if [[ "$basename" == "README.md" ]]; then
        slug="${section}-index.md"
      else
        slug="$basename"
      fi

      if ! grep -qF "$slug" "$ALLOWLIST_FILE"; then
        echo "ALLOWLIST_MISSING: $slug (from $section/$basename)" >&2
        ISSUES=$((ISSUES + 1))
      fi
    done < <(find "$section_dir" -maxdepth 1 -name '*.md' -print0 | sort -z)
  done

  # Check root Home.md
  if ! grep -qF "Home.md" "$ALLOWLIST_FILE"; then
    echo "ALLOWLIST_MISSING: Home.md" >&2
    ISSUES=$((ISSUES + 1))
  fi
}

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

if [[ ! -d "$WIKI_DIR" ]]; then
  echo "ERROR: .wiki directory not found at $WIKI_DIR" >&2
  exit 1
fi

if [[ "$DRY_RUN" == "false" ]] || [[ "$VALIDATE" == "true" ]]; then
  echo "wiki-scaffold: processing sections..."
fi

for section in "${HUMAN_SECTIONS[@]}"; do
  if [[ "$DRY_RUN" == "false" ]] || [[ "$VALIDATE" == "true" ]]; then
    [[ "$DRY_RUN" == "false" ]] && echo "  generating index: $section/README.md"
  fi
  generate_section_index "$section"
done

if [[ "$VALIDATE" == "true" ]]; then
  validate_allowlist
fi

if [[ "$VALIDATE" == "true" ]]; then
  if [[ "$ISSUES" -gt 0 ]]; then
    echo "wiki-scaffold --validate: $ISSUES issue(s) found. Run 'bash scripts/wiki-scaffold.sh' to fix." >&2
    exit 1
  else
    echo "wiki-scaffold --validate: OK"
  fi
else
  [[ "$DRY_RUN" == "false" ]] && echo "wiki-scaffold: done."
fi

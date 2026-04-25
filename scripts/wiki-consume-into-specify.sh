#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

SOURCE_DIR="${BANANA_WIKI_SOURCE_DIR:-.wiki}"
TARGET_DIR="${BANANA_SPECIFY_WIKI_DIR:-.specify/wiki/human-reference}"
PRUNE="${BANANA_SPECIFY_WIKI_PRUNE:-true}"
DRY_RUN="${BANANA_SPECIFY_WIKI_DRY_RUN:-false}"
ALLOWLIST_FILE="${BANANA_WIKI_ALLOWLIST_FILE:-.specify/wiki/human-reference-allowlist.txt}"
ENFORCE_ALLOWLIST="${BANANA_WIKI_ALLOWLIST_ENFORCE:-true}"

usage() {
  cat <<'EOF'
Usage: scripts/wiki-consume-into-specify.sh [--dry-run] [--no-prune] [--source <path>] [--target <path>] [--allowlist <path>] [--no-allowlist-enforce]

Consumes markdown pages from the local wiki checkout into .specify so
.specify remains canonical for agent workflows.

Options:
  --dry-run         Show planned actions without writing files
  --no-prune        Keep target markdown files even if missing in source
  --source <path>   Override source wiki directory (default: .wiki)
  --target <path>   Override target directory (default: .specify/wiki/human-reference)
  --allowlist <path> Override wiki allowlist path (default: .specify/wiki/human-reference-allowlist.txt)
  --no-allowlist-enforce
                    Skip allowlist enforcement (default: enforce)
  --help, -h        Show this help message
EOF
}

while [[ $# -gt 0 ]]; do
  case "$1" in
    --dry-run)
      DRY_RUN=true
      shift
      ;;
    --no-prune)
      PRUNE=false
      shift
      ;;
    --source)
      SOURCE_DIR="$2"
      shift 2
      ;;
    --target)
      TARGET_DIR="$2"
      shift 2
      ;;
    --allowlist)
      ALLOWLIST_FILE="$2"
      shift 2
      ;;
    --no-allowlist-enforce)
      ENFORCE_ALLOWLIST=false
      shift
      ;;
    --help|-h)
      usage
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      usage
      exit 1
      ;;
  esac
done

if [[ ! -d "$SOURCE_DIR" ]]; then
  echo "ERROR: Source wiki directory not found: $SOURCE_DIR" >&2
  exit 1
fi

if [[ "$ENFORCE_ALLOWLIST" == "true" && ! -f "$ALLOWLIST_FILE" ]]; then
  echo "ERROR: Wiki allowlist file not found: $ALLOWLIST_FILE" >&2
  exit 1
fi

if [[ "$DRY_RUN" != "true" ]]; then
  mkdir -p "$TARGET_DIR"
fi

source_count=0
copied_count=0
pruned_count=0

declare -A source_map=()
declare -A allowlist_map=()
declare -a source_files=()
declare -a unexpected_files=()
declare -a missing_files=()

if [[ "$ENFORCE_ALLOWLIST" == "true" ]]; then
  while IFS= read -r allow_entry || [[ -n "$allow_entry" ]]; do
    allow_entry="${allow_entry//$'\r'/}"
    allow_entry="$(echo "$allow_entry" | sed 's/^[[:space:]]*//;s/[[:space:]]*$//')"

    if [[ -z "$allow_entry" || "$allow_entry" == \#* ]]; then
      continue
    fi

    if [[ "$allow_entry" == .wiki/* ]]; then
      allow_entry="${allow_entry#.wiki/}"
    fi

    allowlist_map["$allow_entry"]=1
  done < "$ALLOWLIST_FILE"

  if [[ ${#allowlist_map[@]} -eq 0 ]]; then
    echo "ERROR: Wiki allowlist is empty: $ALLOWLIST_FILE" >&2
    exit 1
  fi
fi

while IFS= read -r -d '' src; do
  rel="${src#${SOURCE_DIR}/}"
  source_files+=("$src")
  source_map["$rel"]=1
  source_count=$((source_count + 1))

  if [[ "$ENFORCE_ALLOWLIST" == "true" && -z "${allowlist_map[$rel]:-}" ]]; then
    unexpected_files+=("$rel")
  fi

done < <(find "$SOURCE_DIR" -type f -name '*.md' ! -path "$SOURCE_DIR/.git/*" -print0)

if [[ "$ENFORCE_ALLOWLIST" == "true" ]]; then
  for rel in "${!allowlist_map[@]}"; do
    if [[ -z "${source_map[$rel]:-}" ]]; then
      missing_files+=("$rel")
    fi
  done

  if [[ ${#unexpected_files[@]} -gt 0 || ${#missing_files[@]} -gt 0 ]]; then
    echo "ERROR: .wiki markdown set does not match allowlist: $ALLOWLIST_FILE" >&2

    if [[ ${#unexpected_files[@]} -gt 0 ]]; then
      echo "Unexpected .wiki markdown files:" >&2
      while IFS= read -r rel; do
        echo "  - $rel" >&2
      done < <(printf '%s\n' "${unexpected_files[@]}" | sort)
    fi

    if [[ ${#missing_files[@]} -gt 0 ]]; then
      echo "Missing allowlisted .wiki markdown files:" >&2
      while IFS= read -r rel; do
        echo "  - $rel" >&2
      done < <(printf '%s\n' "${missing_files[@]}" | sort)
    fi

    exit 1
  fi
fi

for src in "${source_files[@]}"; do
  rel="${src#${SOURCE_DIR}/}"
  dst="${TARGET_DIR}/${rel}"

  if [[ "$DRY_RUN" == "true" ]]; then
    echo "[dry-run] copy $src -> $dst"
  else
    mkdir -p "$(dirname "$dst")"
    cp "$src" "$dst"
  fi

  copied_count=$((copied_count + 1))
done

if [[ "$PRUNE" == "true" && -d "$TARGET_DIR" ]]; then
  while IFS= read -r -d '' dst; do
    rel="${dst#${TARGET_DIR}/}"
    if [[ -z "${source_map[$rel]:-}" ]]; then
      if [[ "$DRY_RUN" == "true" ]]; then
        echo "[dry-run] prune $dst"
      else
        rm -f "$dst"
      fi
      pruned_count=$((pruned_count + 1))
    fi
  done < <(find "$TARGET_DIR" -type f -name '*.md' -print0)
fi

echo "source_markdown_files=$source_count"
echo "copied_markdown_files=$copied_count"
echo "pruned_markdown_files=$pruned_count"
echo "source_dir=$SOURCE_DIR"
echo "target_dir=$TARGET_DIR"
echo "allowlist_file=$ALLOWLIST_FILE"
echo "allowlist_enforced=$ENFORCE_ALLOWLIST"

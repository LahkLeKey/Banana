#!/usr/bin/env bash
# update-readme-screenshots.sh
# Idempotently regenerates the <!-- GALLERY_START --> … <!-- GALLERY_END -->
# section in README.md from PNGs committed under docs/screenshots/.
#
# Usage:
#   bash scripts/update-readme-screenshots.sh [--dry-run]
#
# The script replaces everything between the two HTML comment markers.
# Running it twice with the same inputs produces identical output (idempotent).
set -euo pipefail

DRY_RUN=0
for arg in "$@"; do
  [[ "$arg" == "--dry-run" ]] && DRY_RUN=1
done

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SCREENSHOTS_DIR="$REPO_ROOT/docs/screenshots"
README="$REPO_ROOT/README.md"

if [[ ! -f "$README" ]]; then
  echo "ERROR: README.md not found at $README" >&2
  exit 1
fi

# Verify markers exist
if ! grep -q "<!-- GALLERY_START -->" "$README"; then
  echo "ERROR: <!-- GALLERY_START --> marker missing from README.md" >&2
  exit 1
fi
if ! grep -q "<!-- GALLERY_END -->" "$README"; then
  echo "ERROR: <!-- GALLERY_END --> marker missing from README.md" >&2
  exit 1
fi

# Build gallery lines from PNG files alphabetically
gallery_lines=""
mapfile -t pngs < <(find "$SCREENSHOTS_DIR" -maxdepth 1 -name "*.png" | sort)

if [[ ${#pngs[@]} -eq 0 ]]; then
  echo "INFO: No PNG files found in docs/screenshots/ — gallery will be empty."
else
  gallery_lines+=$'\n'
  for png_path in "${pngs[@]}"; do
    filename="$(basename "$png_path")"
    name="${filename%.png}"
    # Convert dashes to title case for the label
    label="$(echo "$name" | sed 's/-/ /g' | awk '{for(i=1;i<=NF;i++) $i=toupper(substr($i,1,1)) substr($i,2)}1')"
    gallery_lines+="| ![${label}](docs/screenshots/${filename}) |"$'\n'
  done
  gallery_lines+=$'\n'
fi

# Replace content between markers using Python for safe multi-line replacement
README_PATH="$README" GALLERY_LINES="$gallery_lines" DRY_RUN="$DRY_RUN" \
  python3 -c "
import sys, re, os

readme_path = os.environ['README_PATH']
gallery = os.environ.get('GALLERY_LINES', '')
dry_run = os.environ.get('DRY_RUN', '0') == '1'

with open(readme_path, 'r', encoding='utf-8') as f:
    content = f.read()

pattern = r'(<!-- GALLERY_START -->).*?(<!-- GALLERY_END -->)'
replacement = r'\g<1>' + gallery + r'\g<2>'
new_content = re.sub(pattern, replacement, content, flags=re.DOTALL)

if new_content == content:
    print('INFO: Gallery section unchanged.')
    sys.exit(0)

if dry_run:
    print('DRY RUN: Would update README.md gallery section.')
    sys.exit(0)

with open(readme_path, 'w', encoding='utf-8') as f:
    f.write(new_content)

print('OK: README.md gallery section updated.')
"

#!/usr/bin/env bash
set -euo pipefail

# Validates a spec.md file for completeness before it enters the drain queue.
# Usage:
#   bash scripts/validate-spec-quality.sh --spec <path-to-spec.md>
#
# Exit codes:
#   0  Spec passes all quality checks.
#   1  Spec has unfilled placeholders, missing sections, or other quality issues.

SPEC_PATH=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --spec)
      SPEC_PATH="${2:-}"
      shift 2
      ;;
    *)
      echo "Unknown option: $1" >&2
      echo "Usage: $0 --spec <path-to-spec.md>" >&2
      exit 1
      ;;
  esac
done

if [[ -z "$SPEC_PATH" ]]; then
  echo "Error: --spec <path> is required." >&2
  exit 1
fi

if [[ ! -f "$SPEC_PATH" ]]; then
  echo "Error: spec file not found: $SPEC_PATH" >&2
  exit 1
fi

python - "$SPEC_PATH" <<'PY'
import re
import sys
import pathlib

spec_path = pathlib.Path(sys.argv[1])
content = spec_path.read_text(encoding="utf-8")
lines = content.splitlines()

errors = []

# --- Placeholder patterns (FR-4: detect at least 5) ---
placeholder_patterns = [
    (r"\[ACTION REQUIRED\]", "Unfilled ACTION REQUIRED placeholder"),
    (r"\[FEATURE NAME\]", "Spec title still contains placeholder [FEATURE NAME]"),
    (r"\[###-feature-name\]", "Feature branch still contains placeholder"),
    (r"\[DATE\]", "Creation date still contains [DATE] placeholder"),
    (r"\[In-scope outcome \d+\]", "In Scope section has unfilled outcome placeholder"),
    (r"\[Out-of-scope item \d+\]", "Out of Scope section has unfilled item placeholder"),
    (r"\[Brief Title\]", "User story has unfilled [Brief Title] placeholder"),
    (r"\[Placeholder\]", "Generic [Placeholder] text found"),
    (r"\[Describe this user journey\]", "User story body still contains placeholder"),
    (r"\[initial state\]", "Acceptance scenario still contains [initial state] placeholder"),
]

for i, line in enumerate(lines, 1):
    for pattern, message in placeholder_patterns:
        # Strip backtick code spans before pattern matching to avoid false positives
        # where a spec describes the placeholder pattern itself in inline code.
        line_without_code = re.sub(r'`[^`]*`', '', line)
        if re.search(pattern, line_without_code, re.IGNORECASE):
            errors.append(f"Line {i}: {message}")

# --- Required sections ---
required_sections = [
    ("## Problem Statement", "Missing ## Problem Statement section"),
    ("## In Scope", "Missing ## In Scope section"),
    ("## Out of Scope", "Missing ## Out of Scope section"),
    ("## Success Criteria", "Missing ## Success Criteria section"),
]

for heading, message in required_sections:
    if heading.lower() not in content.lower():
        errors.append(message)

# --- In Scope must have at least one bullet ---
in_scope_match = re.search(
    r"## In Scope.*?\n(.*?)(?=\n##|\Z)", content, re.DOTALL | re.IGNORECASE
)
if in_scope_match:
    in_scope_body = in_scope_match.group(1)
    bullets = [l.strip() for l in in_scope_body.splitlines() if l.strip().startswith("- ")]
    if not bullets:
        errors.append("In Scope section has no bullet items")

# --- Out of Scope must have at least one bullet ---
out_scope_match = re.search(
    r"## Out of Scope.*?\n(.*?)(?=\n##|\Z)", content, re.DOTALL | re.IGNORECASE
)
if out_scope_match:
    out_scope_body = out_scope_match.group(1)
    bullets = [l.strip() for l in out_scope_body.splitlines() if l.strip().startswith("- ")]
    if not bullets:
        errors.append("Out of Scope section has no bullet items")

# --- Success Criteria must not be empty ---
sc_match = re.search(
    r"## Success Criteria.*?\n(.*?)(?=\n##|\Z)", content, re.DOTALL | re.IGNORECASE
)
if sc_match:
    sc_body = sc_match.group(1).strip()
    if len(sc_body) < 20:
        errors.append("Success Criteria section appears empty or too brief")

# --- Report ---
if errors:
    print(f"[validate-spec-quality] FAIL: {spec_path}")
    for err in errors:
        print(f"  - {err}")
    sys.exit(1)
else:
    print(f"[validate-spec-quality] PASS: {spec_path}")
    sys.exit(0)
PY

#!/usr/bin/env bash
# 019 thin wrapper around scripts/validate-spec-tasks-parity.py
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$HERE/.." && pwd)"

# Pick the first Python 3 available. Honor BANANA_PYTHON for explicit override.
pick_python() {
  if [[ -n "${BANANA_PYTHON:-}" ]]; then echo "$BANANA_PYTHON"; return; fi
  for candidate in python3 python py; do
    if command -v "$candidate" >/dev/null 2>&1; then
      ver=$("$candidate" -c 'import sys; print(sys.version_info[0])' 2>/dev/null || echo 0)
      if [[ "$ver" == "3" ]]; then echo "$candidate"; return; fi
    fi
  done
  echo "ERROR: no python3 found in PATH" >&2
  exit 2
}
PY=$(pick_python)
exec "$PY" "$HERE/validate-spec-tasks-parity.py" "$@"

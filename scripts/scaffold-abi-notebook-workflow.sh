#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SOURCE_ROOT="src/native"
OUTPUT_PATH="artifacts/notebooks/native-c-catalog.ipynb"
MAX_LINES_PER_FILE="${MAX_LINES_PER_FILE:-240}"

cd "$ROOT_DIR"

echo "[abi-notebook] source root: $SOURCE_ROOT"
echo "[abi-notebook] output path: $OUTPUT_PATH"
echo "[abi-notebook] max lines per file: $MAX_LINES_PER_FILE"

if command -v python3 >/dev/null 2>&1; then
  PYTHON_BIN="python3"
elif command -v python >/dev/null 2>&1; then
  PYTHON_BIN="python"
else
  echo "[abi-notebook] error: python3/python not found on PATH" >&2
  exit 1
fi

"$PYTHON_BIN" scripts/export-native-c-to-notebook.py \
  --source-root "$SOURCE_ROOT" \
  --output "$OUTPUT_PATH" \
  --max-lines-per-file "$MAX_LINES_PER_FILE"

echo "[abi-notebook] done"

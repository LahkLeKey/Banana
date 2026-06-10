#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SOURCE_ROOT="src/native"
OUTPUT_PATH="notebooks/native-c-catalog.ipynb"
MANIFEST_PATH="notebooks/catalog-index.json"
ARTIFACT_NOTEBOOK_PATH="artifacts/notebooks/native-c-catalog.ipynb"
ARTIFACT_MANIFEST_PATH="artifacts/notebooks/catalog-index.json"
REACT_PUBLIC_NOTEBOOK_PATH="src/typescript/react/public/notebooks/native-c-catalog.ipynb"
REACT_PUBLIC_MANIFEST_PATH="src/typescript/react/public/notebooks/catalog-index.json"
MAX_LINES_PER_FILE="${MAX_LINES_PER_FILE:-240}"

cd "$ROOT_DIR"

echo "[abi-notebook] source root: $SOURCE_ROOT"
echo "[abi-notebook] output path: $OUTPUT_PATH"
echo "[abi-notebook] manifest path: $MANIFEST_PATH"
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
  --manifest-output "$MANIFEST_PATH" \
  --max-lines-per-file "$MAX_LINES_PER_FILE"

mkdir -p "$(dirname "$ARTIFACT_NOTEBOOK_PATH")" "$(dirname "$REACT_PUBLIC_NOTEBOOK_PATH")"
cp "$OUTPUT_PATH" "$ARTIFACT_NOTEBOOK_PATH"
cp "$OUTPUT_PATH" "$REACT_PUBLIC_NOTEBOOK_PATH"
cp "$MANIFEST_PATH" "$ARTIFACT_MANIFEST_PATH"
cp "$MANIFEST_PATH" "$REACT_PUBLIC_MANIFEST_PATH"

echo "[abi-notebook] published notebook: $ARTIFACT_NOTEBOOK_PATH"
echo "[abi-notebook] published notebook: $REACT_PUBLIC_NOTEBOOK_PATH"
echo "[abi-notebook] published manifest: $ARTIFACT_MANIFEST_PATH"
echo "[abi-notebook] published manifest: $REACT_PUBLIC_MANIFEST_PATH"

echo "[abi-notebook] done"

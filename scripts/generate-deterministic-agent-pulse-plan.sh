#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

CATALOG_PATH="docs/automation/agent-pulse/autonomous-self-training-default-increments.json"
OUTPUT_PATH="artifacts/sdlc-orchestration/deterministic-agent-pulse-plan.json"
MODEL_JSON_PATH="artifacts/sdlc-orchestration/deterministic-agent-pulse-model.json"
MODEL_BINARY_PATH="build/native-tools/banana_agent_pulse_model_cli"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --catalog)
      CATALOG_PATH="$2"
      shift 2
      ;;
    --output)
      OUTPUT_PATH="$2"
      shift 2
      ;;
    --model-json)
      MODEL_JSON_PATH="$2"
      shift 2
      ;;
    --model-bin)
      MODEL_BINARY_PATH="$2"
      shift 2
      ;;
    *)
      echo "Unknown argument: $1"
      exit 1
      ;;
  esac
done

if [[ ! -f "$CATALOG_PATH" ]]; then
  echo "Catalog JSON not found: $CATALOG_PATH"
  exit 1
fi

CC_BIN="${CC:-}"
if [[ -z "$CC_BIN" ]]; then
  if command -v cc >/dev/null 2>&1; then
    CC_BIN="cc"
  elif command -v gcc >/dev/null 2>&1; then
    CC_BIN="gcc"
  elif command -v clang >/dev/null 2>&1; then
    CC_BIN="clang"
  else
    echo "No C compiler found for deterministic native model build."
    exit 1
  fi
fi

PYTHON_BIN="${BANANA_PYTHON_BIN:-}"
if [[ -z "$PYTHON_BIN" ]]; then
  if [[ -x "$ROOT_DIR/.venv/Scripts/python.exe" ]]; then
    PYTHON_BIN="$ROOT_DIR/.venv/Scripts/python.exe"
  elif [[ -x "$ROOT_DIR/.venv/bin/python" ]]; then
    PYTHON_BIN="$ROOT_DIR/.venv/bin/python"
  elif command -v python3 >/dev/null 2>&1; then
    PYTHON_BIN="python3"
  elif command -v python >/dev/null 2>&1; then
    PYTHON_BIN="python"
  else
    echo "No Python runtime found for deterministic plan rendering."
    exit 1
  fi
fi

mkdir -p "$(dirname "$MODEL_BINARY_PATH")"
mkdir -p "$(dirname "$MODEL_JSON_PATH")"
mkdir -p "$(dirname "$OUTPUT_PATH")"

"$CC_BIN" \
  -std=c11 \
  -O2 \
  -Wall \
  -Wextra \
  -Wpedantic \
  -I"$ROOT_DIR/src/native/core" \
  "$ROOT_DIR/src/native/core/domain/banana_agent_pulse_model.c" \
  "$ROOT_DIR/src/native/core/domain/banana_agent_pulse_model_cli.c" \
  -o "$MODEL_BINARY_PATH"

"$MODEL_BINARY_PATH" > "$MODEL_JSON_PATH"

"$PYTHON_BIN" "$ROOT_DIR/scripts/build-deterministic-agent-pulse-plan.py" \
  --catalog "$CATALOG_PATH" \
  --model-json "$MODEL_JSON_PATH" \
  --output "$OUTPUT_PATH"

printf '%s\n' "$OUTPUT_PATH"

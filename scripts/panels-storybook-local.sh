#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
OUTPUT_DIR="${ROOT_DIR}/artifacts/storybook/panels-static"
PORT="${PANELS_STORYBOOK_PORT:-6010}"

while [[ $# -gt 0 ]]; do
  case "$1" in
    --port)
      PORT="${2:-}"
      shift 2
      ;;
    --no-build)
      NO_BUILD=1
      shift
      ;;
    *)
      echo "[panels-storybook-local] ERROR: unknown argument: $1" >&2
      echo "Usage: bash scripts/panels-storybook-local.sh [--port <n>] [--no-build]" >&2
      exit 1
      ;;
  esac
done

if [[ "${NO_BUILD:-0}" != "1" ]]; then
  bash "${SCRIPT_DIR}/panels-storybook-build.sh" --output "${OUTPUT_DIR}"
fi

if command -v python3 >/dev/null 2>&1; then
  SERVER_CMD=(python3 -m http.server "${PORT}")
elif command -v python >/dev/null 2>&1; then
  SERVER_CMD=(python -m http.server "${PORT}")
else
  echo "[panels-storybook-local] ERROR: python3/python is required to serve static Storybook output." >&2
  exit 1
fi

echo "[panels-storybook-local] serving ${OUTPUT_DIR} at http://localhost:${PORT}"
cd "${OUTPUT_DIR}"
exec "${SERVER_CMD[@]}"

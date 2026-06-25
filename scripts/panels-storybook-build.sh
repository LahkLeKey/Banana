#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
PANELS_DIR="${ROOT_DIR}/src/typescript/packages/panels"
OUTPUT_DIR="${ROOT_DIR}/artifacts/storybook/panels-static"

if [[ "${1:-}" == "--output" ]]; then
  if [[ -z "${2:-}" ]]; then
    echo "[panels-storybook-build] ERROR: --output requires a path" >&2
    exit 1
  fi
  OUTPUT_DIR="$2"
fi

if ! command -v bun >/dev/null 2>&1; then
  echo "[panels-storybook-build] ERROR: bun is required but not installed or not on PATH." >&2
  exit 1
fi

mkdir -p "${OUTPUT_DIR}"

echo "[panels-storybook-build] building static Storybook into ${OUTPUT_DIR}"
cd "${PANELS_DIR}"
bun run build-storybook -- --output-dir "${OUTPUT_DIR}"

echo "[panels-storybook-build] complete"

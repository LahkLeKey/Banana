#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${ROOT_DIR}/artifacts/steam-build/dev-candidate/win-unpacked"
EXE_PATH="${OUT_DIR}/BananaEngineer.exe"

echo "[steam-build] root: ${ROOT_DIR}"

if [[ ! -f "${EXE_PATH}" ]]; then
  echo "[steam-build] ERROR: expected executable not found at ${EXE_PATH}" >&2
  echo "[steam-build] Legacy desktop packaging has been removed. Produce the DX12 Windows executable via the native build pipeline, then place it at the path above." >&2
  exit 1
fi

echo "[steam-build] candidate ready: ${EXE_PATH}"

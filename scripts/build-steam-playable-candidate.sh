#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ELECTRON_DIR="${ROOT_DIR}/src/typescript/electron"
OUT_DIR="${ROOT_DIR}/artifacts/steam-build/dev-candidate/win-unpacked"

echo "[steam-build] root: ${ROOT_DIR}"

echo "[steam-build] installing electron dependencies"
bun install --cwd "${ELECTRON_DIR}"

echo "[steam-build] packaging Windows candidate"
bash -lc "cd '${ELECTRON_DIR}' && BANANA_DESKTOP_START_URL='http://localhost:5173/session-room' bun run build:steam-candidate"

if [[ ! -f "${OUT_DIR}/BananaEngineer.exe" ]]; then
  echo "[steam-build] ERROR: expected executable not found at ${OUT_DIR}/BananaEngineer.exe" >&2
  exit 1
fi

echo "[steam-build] candidate ready: ${OUT_DIR}/BananaEngineer.exe"

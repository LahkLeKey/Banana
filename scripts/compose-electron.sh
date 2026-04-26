#!/usr/bin/env bash
# Spec 010 — Electron smoke (NOT the desktop UI runtime).
set -euo pipefail
cd "$(dirname "$0")/.."
exec docker compose --profile electron run --rm electron-example npm run smoke

#!/usr/bin/env bash
# Spec 010 — Ubuntu-side Electron desktop runtime (WSLg direct render).
# Exit 42 = preflight failure.
set -euo pipefail

require() {
  command -v "$1" >/dev/null 2>&1 || {
    echo "[banana] missing prerequisite: $1" >&2
    echo "[banana] install Docker + WSLg, then retry." >&2
    exit 42
  }
}

require docker
[[ -n "${DISPLAY:-}" ]] || {
  echo "[banana] DISPLAY unset — WSLg or X server required" >&2
  exit 42
}

cd "$(dirname "$0")/.."
exec docker compose --profile electron up electron-desktop

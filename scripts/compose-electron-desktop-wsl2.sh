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
if ! docker version >/dev/null 2>&1; then
  echo "[banana] docker server unavailable inside Ubuntu WSL2" >&2
  echo "[banana] enable Docker Desktop WSL integration for this distro and retry" >&2
  exit 42
fi

if [[ ! -S /var/run/docker.sock ]]; then
  echo "[banana] /var/run/docker.sock missing in Ubuntu WSL2 runtime" >&2
  echo "[banana] this is an environment contract failure; verify Docker Desktop WSL integration" >&2
  exit 42
fi

[[ -n "${DISPLAY:-}" ]] || {
  echo "[banana] DISPLAY unset — WSLg or X server required" >&2
  exit 42
}

cd "$(dirname "$0")/.."
bash scripts/compose-run-profile.sh --profile electron-desktop --action up --service electron-desktop
exec bash scripts/compose-profile-ready.sh --profile electron-desktop --service electron-desktop --timeout-sec "${BANANA_PROFILE_READY_TIMEOUT_SEC:-120}"

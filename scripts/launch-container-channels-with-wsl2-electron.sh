#!/usr/bin/env bash
# Spec 010 — Windows-shell Electron desktop launcher.
# Order of distro selection: BANANA_WSL_DISTRO -> Ubuntu-24.04 -> Ubuntu.
# Exit 42 = preflight failure.
set -euo pipefail

pick_distro() {
  if [[ -n "${BANANA_WSL_DISTRO:-}" ]]; then
    echo "$BANANA_WSL_DISTRO"; return
  fi
  if wsl.exe -l -q 2>/dev/null | tr -d '\r' | grep -Fxq 'Ubuntu-24.04'; then
    echo 'Ubuntu-24.04'; return
  fi
  if wsl.exe -l -q 2>/dev/null | tr -d '\r' | grep -Fxq 'Ubuntu'; then
    echo 'Ubuntu'; return
  fi
  echo "[banana] no Ubuntu WSL distro found (set BANANA_WSL_DISTRO)" >&2
  exit 42
}

DISTRO=$(pick_distro)
exec wsl.exe -d "$DISTRO" -- bash -lc \
  "cd /mnt/c/Github/Banana && bash scripts/compose-electron-desktop-wsl2.sh"

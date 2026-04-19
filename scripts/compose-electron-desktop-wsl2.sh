#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if ! grep -qiE "(microsoft|wsl)" /proc/version 2>/dev/null; then
    echo "This script must run inside WSL2."
    echo "Use scripts/launch-container-channels-with-wsl2-electron.sh from Windows shells."
    exit 1
fi

if [[ ! -d /mnt/wslg ]]; then
    echo "WSLg mount /mnt/wslg was not found. Start a WSLg session and retry."
    exit 1
fi

export DISPLAY="${DISPLAY:-:0}"
export WAYLAND_DISPLAY="${WAYLAND_DISPLAY:-wayland-0}"
export XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/mnt/wslg/runtime-dir}"
export PULSE_SERVER="${PULSE_SERVER:-/mnt/wslg/PulseServer}"
export LIBGL_ALWAYS_SOFTWARE="${LIBGL_ALWAYS_SOFTWARE:-1}"
export COMPOSE_PROJECT_NAME="${COMPOSE_PROJECT_NAME:-banana-container}"

cd "$ROOT_DIR"
docker compose --profile apps --profile electron-desktop up --build -d electron-desktop

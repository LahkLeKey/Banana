#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

resolve_docker_cli() {
    if [[ -x "/usr/bin/docker" ]]; then
        printf '%s\n' "/usr/bin/docker"
        return 0
    fi

    if command -v docker >/dev/null 2>&1; then
        command -v docker
        return 0
    fi

    return 1
}

if ! grep -qiE "(microsoft|wsl)" /proc/version 2>/dev/null; then
    echo "This script must run inside WSL2."
    echo "Use scripts/launch-container-channels-with-wsl2-electron.sh from Windows shells."
    exit 1
fi

if [[ ! -d /mnt/wslg ]]; then
    echo "WSLg mount /mnt/wslg was not found. Start a WSLg session and retry."
    exit 1
fi

if ! DOCKER_BIN="$(resolve_docker_cli)"; then
    echo "Docker CLI is not available in this WSL distro."
    echo "Enable Docker Desktop WSL integration for this distro and retry."
    exit 42
fi

if ! "$DOCKER_BIN" version >/dev/null 2>&1; then
    echo "Docker is not functional in this WSL distro."
    echo "Detected Docker CLI path: $DOCKER_BIN"
    echo "Enable Docker Desktop > Settings > Resources > WSL Integration for this distro and retry."
    exit 42
fi

if [[ "$DOCKER_BIN" == /mnt/c/* ]]; then
    echo "Using Docker CLI from Windows PATH: $DOCKER_BIN"
    echo "WSL integration normally provides /usr/bin/docker in Ubuntu."
fi

export DISPLAY="${DISPLAY:-:0}"
export WAYLAND_DISPLAY="${WAYLAND_DISPLAY:-wayland-0}"
export XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/mnt/wslg/runtime-dir}"
export PULSE_SERVER="${PULSE_SERVER:-/mnt/wslg/PulseServer}"
export LIBGL_ALWAYS_SOFTWARE="${LIBGL_ALWAYS_SOFTWARE:-1}"
export COMPOSE_PROJECT_NAME="${COMPOSE_PROJECT_NAME:-banana-container}"

cd "$ROOT_DIR"
"$DOCKER_BIN" compose --profile apps --profile electron-desktop up --build -d electron-desktop

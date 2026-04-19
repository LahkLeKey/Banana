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

compose_desktop() {
    "$DOCKER_BIN" compose --profile apps --profile electron-desktop up --build --no-recreate -d electron-desktop
}

FALLBACK_DOCKER_CONFIG=""
COMPOSE_LOG=""

cleanup() {
    if [[ -n "$COMPOSE_LOG" && -f "$COMPOSE_LOG" ]]; then
        rm -f "$COMPOSE_LOG"
    fi

    if [[ -n "$FALLBACK_DOCKER_CONFIG" && -d "$FALLBACK_DOCKER_CONFIG" ]]; then
        rm -rf "$FALLBACK_DOCKER_CONFIG"
    fi
}

trap cleanup EXIT

export DISPLAY="${DISPLAY:-:0}"
export WAYLAND_DISPLAY="${WAYLAND_DISPLAY:-wayland-0}"
export XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/mnt/wslg/runtime-dir}"
export PULSE_SERVER="${PULSE_SERVER:-/mnt/wslg/PulseServer}"
export LIBGL_ALWAYS_SOFTWARE="${LIBGL_ALWAYS_SOFTWARE:-1}"
export COMPOSE_PROJECT_NAME="${COMPOSE_PROJECT_NAME:-banana-container}"

cd "$ROOT_DIR"

COMPOSE_LOG="$(mktemp)"

if compose_desktop 2> >(tee "$COMPOSE_LOG" >&2); then
    exit 0
else
    COMPOSE_STATUS=$?
fi

if grep -qiE "docker-credential-desktop\.exe: exec format error|error getting credentials - err: fork/exec /usr/bin/docker-credential-desktop\.exe: exec format error" "$COMPOSE_LOG"; then
    echo "Detected Docker Desktop credential helper incompatibility in WSL."
    echo "Retrying with a temporary helper-free Docker config for public image pulls."

    FALLBACK_DOCKER_CONFIG="$(mktemp -d)"
    printf '{}\n' > "$FALLBACK_DOCKER_CONFIG/config.json"

    DOCKER_CONFIG="$FALLBACK_DOCKER_CONFIG" compose_desktop
    exit 0
fi

exit "$COMPOSE_STATUS"

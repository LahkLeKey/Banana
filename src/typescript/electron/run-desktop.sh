#!/usr/bin/env bash
set -euo pipefail

BASE_CMD=(npm run start -- --no-sandbox --disable-gpu-sandbox)

# Prefer Wayland when the socket is available (WSLg path).
if [[ -n "${WAYLAND_DISPLAY:-}" ]] && [[ -n "${XDG_RUNTIME_DIR:-}" ]] && [[ -S "${XDG_RUNTIME_DIR}/${WAYLAND_DISPLAY}" ]]; then
    echo "Starting Electron with Wayland backend."
    exec "${BASE_CMD[@]}" --enable-features=UseOzonePlatform --ozone-platform=wayland
fi

# Fall back to X11 only when a concrete display socket exists.
DISPLAY_NUM="${DISPLAY:-:0}"
DISPLAY_NUM="${DISPLAY_NUM#:}"
X11_SOCKET="/tmp/.X11-unix/X${DISPLAY_NUM}"

if [[ -S "$X11_SOCKET" ]]; then
    echo "Starting Electron with X11 backend."
    exec "${BASE_CMD[@]}"
fi

echo "No Wayland/X11 socket found; direct container-to-WSLg rendering is unavailable."
echo "Run this through a WSL distro with Docker Desktop WSL integration enabled."
exit 42

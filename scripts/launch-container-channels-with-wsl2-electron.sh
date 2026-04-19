#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cd "$ROOT_DIR"
DETACH=1 scripts/compose-apps.sh

if grep -qiE "(microsoft|wsl)" /proc/version 2>/dev/null; then
    scripts/compose-electron-desktop-wsl2.sh
elif command -v wsl.exe >/dev/null 2>&1; then
    if command -v cygpath >/dev/null 2>&1; then
        WINDOWS_ROOT="$(cygpath -w "$ROOT_DIR")"
    else
        WINDOWS_ROOT="$ROOT_DIR"
    fi

    WSL_ROOT="$(wsl.exe wslpath "$WINDOWS_ROOT" | tr -d '\r')"

    if [[ -z "$WSL_ROOT" ]]; then
        echo "Unable to resolve repository path into WSL."
        exit 1
    fi

    wsl.exe -e bash -lc "cd '$WSL_ROOT' && scripts/compose-electron-desktop-wsl2.sh"
else
    echo "Unable to locate WSL2 from this shell."
    exit 1
fi

echo
echo "Banana channels are running:"
echo "API health: http://localhost:8080/health"
echo "React app: http://localhost:5173"
echo "React Native web: http://localhost:19006"
echo "Electron desktop (dockerized via WSL2): started"

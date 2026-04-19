#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

resolve_wsl_distro() {
    local listed

    if [[ -n "${BANANA_WSL_DISTRO:-}" ]]; then
        printf '%s\n' "$BANANA_WSL_DISTRO"
        return 0
    fi

        listed="$(WSL_UTF8=1 wsl.exe -l -q 2>/dev/null | tr -d '\000\r' || true)"

    if grep -Fxq "Ubuntu-24.04" <<<"$listed"; then
        printf '%s\n' "Ubuntu-24.04"
        return 0
    fi

    if grep -Fxq "Ubuntu" <<<"$listed"; then
        printf '%s\n' "Ubuntu"
        return 0
    fi

    return 1
}

cd "$ROOT_DIR"
scripts/compose-apps-down.sh
DETACH=1 scripts/compose-apps.sh

if grep -qiE "(microsoft|wsl)" /proc/version 2>/dev/null; then
    scripts/compose-electron-desktop-wsl2.sh
elif command -v wsl.exe >/dev/null 2>&1; then
    if ! TARGET_DISTRO="$(resolve_wsl_distro)"; then
        echo "Unable to locate a Ubuntu WSL distro for desktop launch."
        echo "Install Ubuntu from Microsoft Store or set BANANA_WSL_DISTRO=<distro-name>."
        exit 1
    fi

    if command -v cygpath >/dev/null 2>&1; then
        WINDOWS_ROOT="$(cygpath -m "$ROOT_DIR")"
    else
        WINDOWS_ROOT="$ROOT_DIR"
    fi

        WSL_ROOT="$(WSL_UTF8=1 wsl.exe -d "$TARGET_DISTRO" wslpath "$WINDOWS_ROOT" | tr -d '\000\r')"

    if [[ -z "$WSL_ROOT" ]]; then
        echo "Unable to resolve repository path into WSL distro: $TARGET_DISTRO"
        exit 1
    fi

    echo "Launching desktop via WSL distro: $TARGET_DISTRO"

    if wsl.exe -d "$TARGET_DISTRO" -e bash -lc "cd '$WSL_ROOT' && scripts/compose-electron-desktop-wsl2.sh"; then
        :
    else
        WSL_STATUS=$?

        if [[ "$WSL_STATUS" -eq 42 ]]; then
            echo "Direct container-to-WSLg desktop requires Docker Desktop WSL integration."
            echo "Enable integration for distro '$TARGET_DISTRO' in Docker Desktop and retry."
            exit 42
        fi

        echo "Electron desktop launch in '$TARGET_DISTRO' failed with exit code $WSL_STATUS."
        exit "$WSL_STATUS"
    fi
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

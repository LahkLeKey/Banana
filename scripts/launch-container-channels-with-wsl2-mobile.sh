#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

cleanup_ephemeral_mobile_session() {
    if [[ "${BANANA_MOBILE_EPHEMERAL_SESSION:-0}" != "1" ]]; then
        return 0
    fi

    echo
    echo "Stopping ephemeral Banana mobile session..."
    scripts/compose-apps-down.sh || true
}

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

build_wsl_mobile_env_exports() {
    local vars
    local name
    local value
    local escaped
    local exports

    vars=(
        BANANA_SKIP_ANDROID_EMULATOR
        BANANA_SKIP_IOS_PREVIEW
        BANANA_ANDROID_RUNTIME_MODE
        BANANA_ANDROID_AVD
        BANANA_ANDROID_BOOT_TIMEOUT_SEC
        BANANA_ANDROID_PREVIEW_URL
        BANANA_ANDROID_CONTAINER_PREVIEW_URL
        BANANA_ANDROID_CONTAINER_SERVICE
        BANANA_IOS_PREVIEW_URL
        BANANA_IOS_PREVIEW_ENGINE
        BANANA_MOBILE_ENDPOINT_TIMEOUT_SEC
        ANDROID_SDK_ROOT
        ANDROID_HOME
    )

    exports=""

    for name in "${vars[@]}"; do
        value="${!name:-}"

        if [[ -n "$value" ]]; then
            escaped="$(printf '%q' "$value")"
            exports+="export ${name}=${escaped}; "
        fi
    done

    printf '%s' "$exports"
}

if [[ "${BANANA_MOBILE_EPHEMERAL_SESSION:-0}" == "1" ]]; then
    trap cleanup_ephemeral_mobile_session EXIT
fi

cd "$ROOT_DIR"
scripts/compose-apps-down.sh
DETACH=1 scripts/compose-apps.sh

if grep -qiE "(microsoft|wsl)" /proc/version 2>/dev/null; then
    scripts/compose-mobile-emulators-wsl2.sh
elif command -v wsl.exe >/dev/null 2>&1; then
    if ! TARGET_DISTRO="$(resolve_wsl_distro)"; then
        echo "Unable to locate a Ubuntu WSL distro for mobile emulator launch."
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

    echo "Launching mobile emulators via WSL distro: $TARGET_DISTRO"

    WSL_ENV_EXPORTS="$(build_wsl_mobile_env_exports)"

    if wsl.exe -d "$TARGET_DISTRO" -e bash -lc "${WSL_ENV_EXPORTS}cd '$WSL_ROOT' && scripts/compose-mobile-emulators-wsl2.sh"; then
        :
    else
        WSL_STATUS=$?

        if [[ "$WSL_STATUS" -eq 42 ]]; then
            echo "Mobile emulator launch requires WSLg and Ubuntu session integration."
            echo "Enable Docker Desktop WSL integration and start Ubuntu with WSLg, then retry."
            exit 42
        fi

        echo "Mobile emulator launch in '$TARGET_DISTRO' failed with exit code $WSL_STATUS."
        exit "$WSL_STATUS"
    fi
else
    echo "Unable to locate WSL2 from this shell."
    exit 1
fi

echo
ANDROID_CHANNEL_STATUS="started"
IOS_CHANNEL_STATUS="opened"

if [[ "${BANANA_SKIP_ANDROID_EMULATOR:-0}" == "1" ]]; then
    ANDROID_CHANNEL_STATUS="skipped"
fi

if [[ "${BANANA_SKIP_IOS_PREVIEW:-0}" == "1" ]]; then
    IOS_CHANNEL_STATUS="skipped"
fi

echo "Banana channels are running:"
echo "API health: http://localhost:8080/health"
echo "React app: http://localhost:5173"
echo "React Native web: http://localhost:19006"
echo "Android emulator: $ANDROID_CHANNEL_STATUS"
echo "Android runtime mode: ${BANANA_ANDROID_RUNTIME_MODE:-container}"
echo "iOS-style preview window: $IOS_CHANNEL_STATUS"

if [[ "${BANANA_MOBILE_EPHEMERAL_SESSION:-0}" == "1" ]]; then
    echo
    echo "Ephemeral session mode is enabled."
    echo "Press Enter to tear down Banana mobile channels."

    if [[ -t 0 ]]; then
        read -r
    else
        echo "Non-interactive shell detected; tearing down immediately."
    fi
fi
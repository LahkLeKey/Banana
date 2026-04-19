#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ANDROID_CONTAINER_SERVICE="${BANANA_ANDROID_CONTAINER_SERVICE:-android-emulator}"
ANDROID_RUNTIME_MODE="${BANANA_ANDROID_RUNTIME_MODE:-auto}"
ANDROID_WINDOW_URL="${BANANA_ANDROID_WSLG_WINDOW_URL:-http://localhost:6080/vnc.html?autoconnect=1&resize=remote&reconnect=1}"
RUNNING_INSIDE_WSL=0
TARGET_DISTRO=""
WSL_ROOT=""
ANDROID_LAUNCH_EXIT_CODE=0

if grep -qiE "(microsoft|wsl)" /proc/version 2>/dev/null; then
    RUNNING_INSIDE_WSL=1
fi

open_android_emulator_window_local() {
    local url="$ANDROID_WINDOW_URL"
    local default_http_handler=""
    local can_use_xdg_open="1"

    if [[ "${BANANA_ANDROID_OPEN_WINDOW:-1}" != "1" ]]; then
        return 0
    fi

    if command -v gio >/dev/null 2>&1; then
        default_http_handler="$(gio mime x-scheme-handler/http 2>/dev/null | awk -F': ' '/^Default application:/ {print $2}' || true)"

        if [[ -z "$default_http_handler" || "$default_http_handler" == "(null)" ]]; then
            can_use_xdg_open="0"
        fi
    fi

    if command -v xdg-open >/dev/null 2>&1 && [[ "$can_use_xdg_open" == "1" ]]; then
        if xdg-open "$url" >/dev/null 2>&1; then
            echo "Attempted Android emulator UI launch in WSLg window: $url"
            return 0
        fi
    fi

    if command -v gio >/dev/null 2>&1 && [[ -n "$default_http_handler" && "$default_http_handler" != "(null)" ]]; then
        if gio open "$url" >/dev/null 2>&1; then
            echo "Attempted Android emulator UI launch in WSLg window: $url"
            return 0
        fi
    fi

    if command -v chromium-browser >/dev/null 2>&1; then
        nohup chromium-browser --new-window --app="$url" >/dev/null 2>&1 &
        echo "Attempted Android emulator UI launch in WSLg window: $url"
        return 0
    fi

    if command -v chromium >/dev/null 2>&1; then
        nohup chromium --new-window --app="$url" >/dev/null 2>&1 &
        echo "Attempted Android emulator UI launch in WSLg window: $url"
        return 0
    fi

    if command -v google-chrome >/dev/null 2>&1; then
        nohup google-chrome --new-window --app="$url" >/dev/null 2>&1 &
        echo "Attempted Android emulator UI launch in WSLg window: $url"
        return 0
    fi

    if command -v firefox >/dev/null 2>&1; then
        nohup firefox --new-window "$url" >/dev/null 2>&1 &
        echo "Attempted Android emulator UI launch in WSLg window: $url"
        return 0
    fi

    return 1
}

open_android_emulator_window_remote() {
    local distro="$1"
    local url="$ANDROID_WINDOW_URL"
    local escaped_url
    local open_result

    if [[ "${BANANA_ANDROID_OPEN_WINDOW:-1}" != "1" ]]; then
        return 0
    fi

    escaped_url="$(printf '%q' "$url")"

    open_result="$(WSL_UTF8=1 wsl.exe -d "$distro" -e bash -lc "ANDROID_WINDOW_URL=$escaped_url; default_http_handler=''; can_use_xdg_open='1'; if command -v gio >/dev/null 2>&1; then default_http_handler=\$(gio mime x-scheme-handler/http 2>/dev/null | awk -F': ' '/^Default application:/ {print \$2}' || true); if [[ -z \"\$default_http_handler\" ]] || [[ \"\$default_http_handler\" == \"(null)\" ]]; then can_use_xdg_open='0'; fi; fi; if command -v xdg-open >/dev/null 2>&1 && [[ \"\$can_use_xdg_open\" == \"1\" ]] && xdg-open \"\$ANDROID_WINDOW_URL\" >/dev/null 2>&1; then echo OPENED; elif command -v gio >/dev/null 2>&1 && [[ -n \"\$default_http_handler\" ]] && [[ \"\$default_http_handler\" != \"(null)\" ]] && gio open \"\$ANDROID_WINDOW_URL\" >/dev/null 2>&1; then echo OPENED; elif command -v chromium-browser >/dev/null 2>&1 && (nohup chromium-browser --new-window --app=\"\$ANDROID_WINDOW_URL\" >/dev/null 2>&1 &); then echo OPENED; elif command -v chromium >/dev/null 2>&1 && (nohup chromium --new-window --app=\"\$ANDROID_WINDOW_URL\" >/dev/null 2>&1 &); then echo OPENED; elif command -v google-chrome >/dev/null 2>&1 && (nohup google-chrome --new-window --app=\"\$ANDROID_WINDOW_URL\" >/dev/null 2>&1 &); then echo OPENED; elif command -v firefox >/dev/null 2>&1 && (nohup firefox --new-window \"\$ANDROID_WINDOW_URL\" >/dev/null 2>&1 &); then echo OPENED; else echo NO_OPENER; fi" 2>/dev/null | tr -d '\000\r' | tail -n 1 || true)"

    if [[ "$open_result" == "OPENED" ]]; then
        echo "Attempted Android emulator UI launch in WSLg window: $url"
        return 0
    else
        return 1
    fi
}

open_android_emulator_window_windows_host() {
    local url="$ANDROID_WINDOW_URL"

    if [[ "${BANANA_ANDROID_OPEN_WINDOW:-1}" != "1" ]]; then
        return 0
    fi

    if open_url_windows_host "$url" "1"; then
        echo "Opened Android emulator UI in host browser window: $url"
        return 0
    fi

    if command -v powershell.exe >/dev/null 2>&1; then
        if powershell.exe -NoProfile -Command "Start-Process '$url'" >/dev/null 2>&1; then
            echo "Opened Android emulator UI in host browser window: $url"
            return 0
        fi
    fi

    echo "Android emulator started, but no browser launcher was found."
    echo "Install a browser/opener in Ubuntu, for example: sudo apt-get update && sudo apt-get install -y xdg-utils firefox"
    echo "Open this URL manually: $url"
    return 1
}

open_url_windows_host() {
    local url="$1"
    local prefer_app_mode="${2:-0}"
    local escaped_url

    escaped_url="${url//\'/\'\'}"

    if command -v explorer.exe >/dev/null 2>&1; then
        if explorer.exe "$url" >/dev/null 2>&1; then
            return 0
        fi
    fi

    if command -v powershell.exe >/dev/null 2>&1; then
        if [[ "$prefer_app_mode" == "1" ]]; then
            if powershell.exe -NoProfile -Command "\$ErrorActionPreference='Stop'; \$u='${escaped_url}'; \$edge=Get-Command msedge.exe -ErrorAction SilentlyContinue; if (\$edge) { Start-Process -FilePath \$edge.Path -ArgumentList @('--new-window', ('--app=' + \$u)); exit 0 }; \$chrome=Get-Command chrome.exe -ErrorAction SilentlyContinue; if (\$chrome) { Start-Process -FilePath \$chrome.Path -ArgumentList @('--new-window', ('--app=' + \$u)); exit 0 }; Start-Process \$u" >/dev/null 2>&1; then
                return 0
            fi
        else
            if powershell.exe -NoProfile -Command "Start-Process '${escaped_url}'" >/dev/null 2>&1; then
                return 0
            fi
        fi
    fi

    if command -v cmd.exe >/dev/null 2>&1; then
        if cmd.exe /C start "" "$url" >/dev/null 2>&1; then
            return 0
        fi
    fi

    return 1
}

open_runtime_windows_windows_host() {
    local url

    if [[ "$RUNNING_INSIDE_WSL" == "1" ]]; then
        return 0
    fi

    if [[ "${BANANA_OPEN_RUNTIME_WINDOWS:-1}" != "1" ]]; then
        return 0
    fi

    for url in \
        "http://localhost:8080/health" \
        "http://localhost:5173" \
        "http://localhost:19006"
    do
        open_url_windows_host "$url" || true
    done

    if [[ "$ANDROID_CHANNEL_STATUS" == "started" && "$ANDROID_CHANNEL_RUNTIME" == "container" && "${BANANA_ANDROID_OPEN_WINDOW:-1}" == "1" ]]; then
        open_url_windows_host "$ANDROID_WINDOW_URL" "1" || true
    fi

    echo "Opened runtime windows in host browser."
}

launch_android_emulator_container() {
    if ! command -v docker >/dev/null 2>&1; then
        echo "Docker CLI is not available for Android emulator container launch."
        return 1
    fi

    if ! docker compose --profile apps --profile android-emulator up -d "$ANDROID_CONTAINER_SERVICE"; then
        echo "Android emulator container launch hit a transient compose state; retrying once."
        docker compose --profile apps --profile android-emulator rm --force --stop "$ANDROID_CONTAINER_SERVICE" >/dev/null 2>&1 || true
        docker compose --profile apps --profile android-emulator up -d "$ANDROID_CONTAINER_SERVICE"
    fi

    if ! wait_for_http_endpoint "$ANDROID_WINDOW_URL" "${BANANA_ANDROID_NOVNC_TIMEOUT_SEC:-60}"; then
        echo "Android emulator container started, but noVNC endpoint is not ready yet: $ANDROID_WINDOW_URL"
        echo "The launcher will continue; refresh the page after startup completes."
    fi
}

launch_android_emulator_from_channels_profile() {
    local container_status

    case "$ANDROID_RUNTIME_MODE" in
        container)
            launch_android_emulator_container
            ANDROID_CHANNEL_RUNTIME="container"
            ;;
        local)
            launch_android_emulator_local
            ANDROID_CHANNEL_RUNTIME="local"
            ;;
        auto)
            if launch_android_emulator_local; then
                ANDROID_CHANNEL_RUNTIME="local"
            else
                echo "Local Android emulator launch failed; falling back to containerized Android emulator."

                if launch_android_emulator_container; then
                    ANDROID_CHANNEL_RUNTIME="container"
                else
                    container_status=$?
                    return "$container_status"
                fi
            fi
            ;;
        *)
            echo "Unsupported BANANA_ANDROID_RUNTIME_MODE value: $ANDROID_RUNTIME_MODE"
            echo "Use one of: container, auto, local"
            return 1
            ;;
    esac
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

wait_for_http_endpoint() {
    local url="$1"
    local timeout_seconds="$2"
    local start

    start="$(date +%s)"

    while true; do
        if curl -fsS "$url" >/dev/null 2>&1; then
            return 0
        fi

        if (( "$(date +%s)" - start >= timeout_seconds )); then
            return 1
        fi

        sleep 2
    done
}

prepare_remote_wsl_context() {
    local windows_root

    if [[ "$RUNNING_INSIDE_WSL" == "1" ]]; then
        return 0
    fi

    if [[ -n "$TARGET_DISTRO" && -n "$WSL_ROOT" ]]; then
        return 0
    fi

    if ! command -v wsl.exe >/dev/null 2>&1; then
        echo "Unable to locate WSL2 from this shell."
        return 1
    fi

    if ! TARGET_DISTRO="$(resolve_wsl_distro)"; then
        echo "Unable to locate a Ubuntu WSL distro for desktop launch."
        echo "Install Ubuntu from Microsoft Store or set BANANA_WSL_DISTRO=<distro-name>."
        return 1
    fi

    if command -v cygpath >/dev/null 2>&1; then
        windows_root="$(cygpath -m "$ROOT_DIR")"
    else
        windows_root="$ROOT_DIR"
    fi

    WSL_ROOT="$(WSL_UTF8=1 wsl.exe -d "$TARGET_DISTRO" wslpath "$windows_root" | tr -d '\000\r')"

    if [[ -z "$WSL_ROOT" ]]; then
        echo "Unable to resolve repository path into WSL distro: $TARGET_DISTRO"
        return 1
    fi

    return 0
}

build_wsl_android_env_exports() {
    local vars
    local name
    local value
    local escaped
    local exports

    vars=(
        BANANA_ANDROID_AVD
        BANANA_ANDROID_BOOT_TIMEOUT_SEC
        BANANA_ANDROID_PREVIEW_URL
        BANANA_MOBILE_ENDPOINT_TIMEOUT_SEC
        ANDROID_SDK_ROOT
        ANDROID_HOME
    )

    exports="export BANANA_ANDROID_RUNTIME_MODE=local; export BANANA_SKIP_IOS_PREVIEW=1; "

    for name in "${vars[@]}"; do
        value="${!name:-}"

        if [[ -n "$value" ]]; then
            escaped="$(printf '%q' "$value")"
            exports+="export ${name}=${escaped}; "
        fi
    done

    printf '%s' "$exports"
}

launch_android_emulator_local() {
    local wsl_exports

    if [[ "$RUNNING_INSIDE_WSL" == "1" ]]; then
        BANANA_ANDROID_RUNTIME_MODE=local BANANA_SKIP_IOS_PREVIEW=1 scripts/compose-mobile-emulators-wsl2.sh
        return $?
    fi

    if ! prepare_remote_wsl_context; then
        return 1
    fi

    wsl_exports="$(build_wsl_android_env_exports)"

    wsl.exe -d "$TARGET_DISTRO" -e bash -lc "${wsl_exports}cd '$WSL_ROOT' && scripts/compose-mobile-emulators-wsl2.sh"
}

cd "$ROOT_DIR"
scripts/compose-apps-down.sh
DETACH=1 scripts/compose-apps.sh

ANDROID_CHANNEL_STATUS="skipped"
ANDROID_CHANNEL_RUNTIME="skipped"

if [[ "${BANANA_SKIP_ANDROID_EMULATOR:-0}" == "1" ]]; then
    echo "Skipping Android emulator launch because BANANA_SKIP_ANDROID_EMULATOR=1."
elif launch_android_emulator_from_channels_profile; then
    ANDROID_CHANNEL_STATUS="started"
else
    ANDROID_LAUNCH_EXIT_CODE=$?
    ANDROID_CHANNEL_STATUS="failed"
    ANDROID_CHANNEL_RUNTIME="failed"
    echo "Android emulator launch was attempted but did not complete in this profile."
fi

if [[ "$RUNNING_INSIDE_WSL" == "1" ]]; then
    if [[ "$ANDROID_CHANNEL_STATUS" == "started" && "$ANDROID_CHANNEL_RUNTIME" == "container" ]]; then
        if ! open_android_emulator_window_local; then
            open_android_emulator_window_windows_host || true
        fi
    fi

    scripts/compose-electron-desktop-wsl2.sh
elif command -v wsl.exe >/dev/null 2>&1; then
    if ! prepare_remote_wsl_context; then
        exit 1
    fi

    echo "Launching desktop via WSL distro: $TARGET_DISTRO"
    open_runtime_windows_windows_host

    if [[ "$ANDROID_CHANNEL_STATUS" == "started" && "$ANDROID_CHANNEL_RUNTIME" == "container" ]]; then
        if ! open_android_emulator_window_remote "$TARGET_DISTRO"; then
            open_android_emulator_window_windows_host || true
        fi
    fi

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
echo "Android emulator: $ANDROID_CHANNEL_STATUS"
echo "Android runtime mode: $ANDROID_CHANNEL_RUNTIME"

if [[ "$ANDROID_CHANNEL_STATUS" == "started" && "$ANDROID_CHANNEL_RUNTIME" == "container" ]]; then
    echo "Android emulator noVNC: http://localhost:6080"
    echo "Android emulator noVNC auto-connect: $ANDROID_WINDOW_URL"
fi

if [[ "$ANDROID_CHANNEL_STATUS" == "failed" ]]; then
    echo
    echo "Android emulator launch failed in the default channels profile."
    echo "Set BANANA_SKIP_ANDROID_EMULATOR=1 to run desktop channels without Android."
    exit "${ANDROID_LAUNCH_EXIT_CODE:-1}"
fi

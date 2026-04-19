#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if ! grep -qiE "(microsoft|wsl)" /proc/version 2>/dev/null; then
    echo "This script must run inside WSL2."
    echo "Use scripts/launch-container-channels-with-wsl2-mobile.sh from Windows shells."
    exit 1
fi

if [[ ! -d /mnt/wslg ]]; then
    echo "WSLg mount /mnt/wslg was not found. Start a WSLg session and retry."
    exit 42
fi

export DISPLAY="${DISPLAY:-:0}"
export WAYLAND_DISPLAY="${WAYLAND_DISPLAY:-wayland-0}"
export XDG_RUNTIME_DIR="${XDG_RUNTIME_DIR:-/mnt/wslg/runtime-dir}"
export PULSE_SERVER="${PULSE_SERVER:-/mnt/wslg/PulseServer}"
export LIBGL_ALWAYS_SOFTWARE="${LIBGL_ALWAYS_SOFTWARE:-1}"

ANDROID_SDK_ROOT="${ANDROID_SDK_ROOT:-${ANDROID_HOME:-$HOME/Android/Sdk}}"
EMULATOR_BIN="$ANDROID_SDK_ROOT/emulator/emulator"
ADB_BIN="$ANDROID_SDK_ROOT/platform-tools/adb"
ANDROID_RUNTIME_MODE="${BANANA_ANDROID_RUNTIME_MODE:-container}"
ANDROID_CONTAINER_SERVICE="${BANANA_ANDROID_CONTAINER_SERVICE:-android-emulator}"
DOCKER_BIN=""

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

ensure_docker_integration() {
    if [[ -n "$DOCKER_BIN" ]]; then
        return 0
    fi

    if ! DOCKER_BIN="$(resolve_docker_cli)"; then
        echo "Docker CLI is not available in this Ubuntu distro."
        echo "Enable Docker Desktop > Settings > Resources > WSL Integration for this distro and retry."
        return 42
    fi

    if [[ "$DOCKER_BIN" == /mnt/c/* ]]; then
        echo "Docker CLI resolves to a Windows path in this Ubuntu distro: $DOCKER_BIN"
        echo "Enable Docker Desktop > Settings > Resources > WSL Integration for this distro so Ubuntu uses a Linux docker CLI path."
        return 42
    fi

    if ! "$DOCKER_BIN" version >/dev/null 2>&1; then
        echo "Docker is not functional in this Ubuntu distro."
        echo "Detected Docker CLI path: $DOCKER_BIN"
        echo "Enable Docker Desktop > Settings > Resources > WSL Integration for this distro and retry."
        return 42
    fi

    if [[ ! -S /var/run/docker.sock ]]; then
        echo "Docker socket /var/run/docker.sock was not found in this Ubuntu distro."
        echo "Enable Docker Desktop > Settings > Resources > WSL Integration for this distro and retry."
        return 42
    fi

    return 0
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

resolve_android_avd() {
    local avds
    local selected

    if [[ -n "${BANANA_ANDROID_AVD:-}" ]]; then
        printf '%s\n' "$BANANA_ANDROID_AVD"
        return 0
    fi

    avds="$("$EMULATOR_BIN" -list-avds 2>/dev/null || true)"
    selected="$(printf '%s\n' "$avds" | sed -n '1p')"

    if [[ -z "$selected" ]]; then
        return 1
    fi

    printf '%s\n' "$selected"
}

android_emulator_serial() {
    "$ADB_BIN" devices 2>/dev/null | awk '/^emulator-[0-9]+[[:space:]]+device$/ {print $1; exit}'
}

wait_for_android_boot() {
    local timeout_seconds="${BANANA_ANDROID_BOOT_TIMEOUT_SEC:-300}"
    local start
    local boot_completed

    start="$(date +%s)"
    "$ADB_BIN" wait-for-device >/dev/null 2>&1 || true

    while true; do
        boot_completed="$("$ADB_BIN" shell getprop sys.boot_completed 2>/dev/null | tr -d '\r\n' || true)"

        if [[ "$boot_completed" == "1" ]]; then
            return 0
        fi

        if (( "$(date +%s)" - start >= timeout_seconds )); then
            return 1
        fi

        sleep 2
    done
}

launch_android_emulator_local() {
    local avd_name
    local log_file
    local android_url
    local existing_serial
    local accel_args=()

    android_url="${BANANA_ANDROID_PREVIEW_URL:-http://10.0.2.2:19006}"

    if [[ ! -x "$EMULATOR_BIN" || ! -x "$ADB_BIN" ]]; then
        echo "Android SDK tools were not found in this Ubuntu distro."
        echo "Expected emulator: $EMULATOR_BIN"
        echo "Expected adb: $ADB_BIN"
        echo "Install Android SDK command-line tools and emulator in Ubuntu, then retry."
        return 1
    fi

    "$ADB_BIN" start-server >/dev/null 2>&1 || true
    existing_serial="$(android_emulator_serial || true)"

    if [[ -n "$existing_serial" ]]; then
        echo "Android emulator already running ($existing_serial)."
    else
        if ! avd_name="$(resolve_android_avd)"; then
            echo "No Android AVD was found in this Ubuntu distro."
            echo "Create one in Android Studio Device Manager or with avdmanager, then retry."
            echo "Optional override: set BANANA_ANDROID_AVD=<avd-name>."
            return 1
        fi

        log_file="${TMPDIR:-/tmp}/banana-android-emulator.log"

        if [[ ! -c /dev/kvm ]]; then
            accel_args=("-accel" "off")
        fi

        echo "Launching Android emulator via WSLg (AVD: $avd_name)."
        nohup "$EMULATOR_BIN" -avd "$avd_name" -gpu swiftshader_indirect "${accel_args[@]}" -no-boot-anim -no-snapshot-save >"$log_file" 2>&1 &

        if ! wait_for_android_boot; then
            echo "Android emulator did not finish booting within timeout."
            echo "Inspect emulator log: $log_file"
            return 1
        fi
    fi

    if "$ADB_BIN" shell am start -a android.intent.action.VIEW -d "$android_url" >/dev/null 2>&1; then
        echo "Opened React Native web channel in Android emulator: $android_url"
    else
        echo "Android emulator is up, but automatic URL open failed."
        echo "Open this URL manually in emulator browser: $android_url"
    fi
}

wait_for_android_container_boot() {
    local timeout_seconds="${BANANA_ANDROID_BOOT_TIMEOUT_SEC:-300}"
    local start
    local boot_completed

    start="$(date +%s)"

    while true; do
        boot_completed="$("$DOCKER_BIN" compose exec -T "$ANDROID_CONTAINER_SERVICE" adb shell getprop sys.boot_completed 2>/dev/null | tr -d '\r\n' || true)"

        if [[ "$boot_completed" == "1" ]]; then
            return 0
        fi

        if (( "$(date +%s)" - start >= timeout_seconds )); then
            return 1
        fi

        sleep 5
    done
}

launch_android_emulator_container() {
    local android_url

    android_url="${BANANA_ANDROID_CONTAINER_PREVIEW_URL:-http://react-native-web:8081}"

    if ! ensure_docker_integration; then
        return 42
    fi

    echo "Launching Android emulator container via Docker Compose profile android-emulator."

    if ! "$DOCKER_BIN" compose --profile android-emulator up -d "$ANDROID_CONTAINER_SERVICE"; then
        echo "Android emulator container launch hit a transient compose state; retrying once."
        "$DOCKER_BIN" compose --profile android-emulator rm -sf "$ANDROID_CONTAINER_SERVICE" >/dev/null 2>&1 || true
        "$DOCKER_BIN" compose --profile android-emulator up -d "$ANDROID_CONTAINER_SERVICE"
    fi

    if ! wait_for_android_container_boot; then
        echo "Android emulator container did not finish booting within timeout."
        echo "Inspect logs with: docker compose logs $ANDROID_CONTAINER_SERVICE"
        return 1
    fi

    if "$DOCKER_BIN" compose exec -T "$ANDROID_CONTAINER_SERVICE" adb shell am start -a android.intent.action.VIEW -d "$android_url" >/dev/null 2>&1; then
        echo "Opened React Native web channel in Android emulator container: $android_url"
    else
        echo "Android emulator container is up, but automatic URL open failed."
        echo "Open a browser in emulator container manually to: $android_url"
    fi

    echo "Android emulator container UI (noVNC): http://localhost:6080"
}

launch_android_emulator() {
    case "$ANDROID_RUNTIME_MODE" in
        container)
            launch_android_emulator_container
            ;;
        local)
            launch_android_emulator_local
            ;;
        auto)
            if ! launch_android_emulator_container; then
                echo "Containerized Android emulator launch failed; falling back to local SDK emulator mode."
                launch_android_emulator_local
            fi
            ;;
        *)
            echo "Unsupported BANANA_ANDROID_RUNTIME_MODE value: $ANDROID_RUNTIME_MODE"
            echo "Use one of: container, local, auto"
            return 1
            ;;
    esac
}

launch_ios_preview_webkit_local() {
    local ios_url="$1"
    local log_file

    if command -v epiphany-browser >/dev/null 2>&1; then
        nohup epiphany-browser --application-mode="$ios_url" >/dev/null 2>&1 &
        return 0
    fi

    if command -v npx >/dev/null 2>&1; then
        log_file="${TMPDIR:-/tmp}/banana-ios-webkit.log"
        nohup npx --yes playwright open --browser=webkit "$ios_url" >"$log_file" 2>&1 &
        return 0
    fi

    return 1
}

launch_ios_preview_window() {
    local ios_url
    local ios_user_agent
    local ios_engine

    ios_url="${BANANA_IOS_PREVIEW_URL:-http://localhost:19006}"
    ios_engine="${BANANA_IOS_PREVIEW_ENGINE:-chromium}"
    ios_user_agent="Mozilla/5.0 (iPhone; CPU iPhone OS 17_4 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/17.4 Mobile/15E148 Safari/604.1"

    if [[ "$ios_engine" == "webkit" ]]; then
        if launch_ios_preview_webkit_local "$ios_url"; then
            return 0
        fi

        echo "Unable to launch local WebKit iOS preview."
        echo "Install epiphany-browser or node+npx (Playwright), or set BANANA_IOS_PREVIEW_ENGINE=chromium."
        return 1
    fi

    if command -v chromium-browser >/dev/null 2>&1; then
        nohup chromium-browser --new-window --app="$ios_url" --user-agent="$ios_user_agent" --window-size=430,932 >/dev/null 2>&1 &
        return 0
    fi

    if command -v chromium >/dev/null 2>&1; then
        nohup chromium --new-window --app="$ios_url" --user-agent="$ios_user_agent" --window-size=430,932 >/dev/null 2>&1 &
        return 0
    fi

    if command -v google-chrome >/dev/null 2>&1; then
        nohup google-chrome --new-window --app="$ios_url" --user-agent="$ios_user_agent" --window-size=430,932 >/dev/null 2>&1 &
        return 0
    fi

    if command -v xdg-open >/dev/null 2>&1; then
        xdg-open "$ios_url" >/dev/null 2>&1 || true
        return 0
    fi

    return 1
}

cd "$ROOT_DIR"

if ! wait_for_http_endpoint "http://localhost:19006" "${BANANA_MOBILE_ENDPOINT_TIMEOUT_SEC:-180}"; then
    echo "React Native web endpoint was not reachable at http://localhost:19006."
    echo "Start Banana channels first (scripts/compose-apps.sh) and retry."
    exit 1
fi

if [[ "${BANANA_SKIP_ANDROID_EMULATOR:-0}" == "1" ]]; then
    echo "Skipping Android emulator launch because BANANA_SKIP_ANDROID_EMULATOR=1."
else
    launch_android_emulator
fi

if [[ "${BANANA_SKIP_IOS_PREVIEW:-0}" == "1" ]]; then
    echo "Skipping iOS preview because BANANA_SKIP_IOS_PREVIEW=1."
else
    if launch_ios_preview_window; then
        echo "Opened iOS-style preview window (local fallback engine: ${BANANA_IOS_PREVIEW_ENGINE:-chromium})."
    else
        echo "Unable to open iOS-style preview window automatically."
    fi

    echo "iOS Simulator is not available on Ubuntu."
    echo "Apple iOS Simulator requires macOS + Xcode."
    echo "This Ubuntu workflow provides an iOS-style web preview fallback."
fi

ANDROID_TARGET="${BANANA_ANDROID_PREVIEW_URL:-http://10.0.2.2:19006}"

if [[ "$ANDROID_RUNTIME_MODE" == "container" ]]; then
    ANDROID_TARGET="${BANANA_ANDROID_CONTAINER_PREVIEW_URL:-http://react-native-web:8081}"
elif [[ "$ANDROID_RUNTIME_MODE" == "auto" ]]; then
    ANDROID_TARGET="${BANANA_ANDROID_CONTAINER_PREVIEW_URL:-http://react-native-web:8081} (auto fallback to ${BANANA_ANDROID_PREVIEW_URL:-http://10.0.2.2:19006})"
fi

echo
echo "Mobile emulator channels are running:"
echo "Android emulator target: $ANDROID_TARGET"
echo "Android runtime mode: $ANDROID_RUNTIME_MODE"
if [[ "${BANANA_SKIP_ANDROID_EMULATOR:-0}" != "1" && "$ANDROID_RUNTIME_MODE" != "local" ]]; then
    echo "Android emulator noVNC: http://localhost:6080"
fi
echo "iOS preview target: ${BANANA_IOS_PREVIEW_URL:-http://localhost:19006}"
echo "iOS preview engine: ${BANANA_IOS_PREVIEW_ENGINE:-chromium}"
echo "React Native web endpoint: http://localhost:19006"
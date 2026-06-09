#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BANANA_NATIVE_BUILD_DIR:-out/v3-native}"
BUILD_CONFIG="${BANANA_NATIVE_BUILD_CONFIG:-Debug}"
config_set=0
if [[ -n "${BANANA_NATIVE_BUILD_CONFIG:-}" ]]; then
    config_set=1
fi

auto_target=""
target_hotkey=""
telemetry=""
autotest_seconds=""
playtest_defaults=0
defaults_locked=0

suite="playtest"
list_only=0
skip_build=0
feature_variant=6

declare -a explicit_variants=()
declare -a selected_variants=()
declare -a explicit_features=()
declare -a selected_features=()

die() {
    echo "[dx12-playtest] error: $*" >&2
    exit 1
}

usage() {
    cat <<'EOF_USAGE'
Usage:
  bash scripts/run-dx12-playtest-suites.sh [options]

Options:
    --suite <name>               Named suite: playtest, showcase, metro, corridor, continent, all, features, none
    --variant <id>               Add a specific demo scene variant (repeatable)
    --feature <name>             Add a war-sentience feature run (repeatable)
    --build-dir <path>           Build folder (default: out/v3-native)
    --config <name>              Build config (default: Debug; playtest suites default to RelWithDebInfo)
    --skip-build                 Skip build step
    --autotest <seconds>         Run DX12 POC in autotest mode for N seconds
    --telemetry                  Enable runtime telemetry
    --auto-target                Enable auto targeting
    --target-hotkey              Enable target hotkey
    --defaults                   Enable playtest-friendly defaults (telemetry + auto-target)
    --no-defaults                Disable playtest-friendly defaults
    --list                       Print suite and variant map
    --help                       Show this help

Suites:
  playtest  - war-sentience showcase + metro station + corridor
  showcase  - war-sentience showcase only
  metro     - banana line station (neo-musa)
  corridor  - banana line corridor (metro -> crescent)
  continent - stem territories + north crown
    features  - warfront/flank/pressure/negotiate/truce/comeback/rally (war-sentience showcase)
  all       - all enabled variants above
    none      - no preset variants; use --variant or --feature

Environment:
  BANANA_NATIVE_BUILD_DIR, BANANA_NATIVE_BUILD_CONFIG
EOF_USAGE
}

append_variant() {
    local variant_id="$1"

    for existing in "${selected_variants[@]}"; do
        if [[ "$existing" == "$variant_id" ]]; then
            return
        fi
    done

    selected_variants+=("$variant_id")
}

variant_label() {
    case "$1" in
        0) echo "continent-stem-territories" ;;
        1) echo "continent-north-crown" ;;
        2) echo "banana-line-neo-musa" ;;
        3) echo "banana-line-metro-crescent" ;;
        6) echo "war-sentience-showcase" ;;
        *) echo "variant-$1" ;;
    esac
}

normalize_feature() {
    case "$1" in
        warfront|war-front|front) echo "warfront" ;;
        flank) echo "flank" ;;
        pressure) echo "pressure" ;;
        negotiate|negotiation) echo "negotiate" ;;
        truce) echo "truce" ;;
        comeback) echo "comeback" ;;
        rally) echo "rally" ;;
        *) echo "$1" ;;
    esac
}

append_feature() {
    local feature
    feature="$(normalize_feature "$1")"

    for existing in "${selected_features[@]}"; do
        if [[ "$existing" == "$feature" ]]; then
            return
        fi
    done

    selected_features+=("$feature")
}

append_suite_variants() {
    local suite_name="$1"

    case "$suite_name" in
        playtest)
            append_variant 6
            append_variant 2
            append_variant 3
            ;;
        showcase)
            append_variant 6
            ;;
        metro)
            append_variant 2
            ;;
        corridor)
            append_variant 3
            ;;
        continent)
            append_variant 0
            append_variant 1
            ;;
        all)
            append_variant 0
            append_variant 1
            append_variant 2
            append_variant 3
            append_variant 6
            ;;
        features)
            ;;
        none)
            ;;
        *)
            die "unknown suite '$suite_name'"
            ;;
    esac
}

append_suite_features() {
    local suite_name="$1"

    case "$suite_name" in
        features)
            append_feature warfront
            append_feature flank
            append_feature pressure
            append_feature negotiate
            append_feature truce
            append_feature comeback
            append_feature rally
            ;;
        *)
            ;;
    esac
}

print_suite_list() {
        cat <<'EOF_SUITES'
Available suites:
    playtest  - war-sentience showcase + metro station + corridor
    showcase  - war-sentience showcase only
    metro     - banana line station (neo-musa)
    corridor  - banana line corridor (metro -> crescent)
    continent - stem territories + north crown
    features  - warfront/flank/pressure/negotiate/truce/comeback/rally (war-sentience showcase)
    all       - all enabled variants above
    none      - no preset variants; use --variant

Known variants:
    0 - Continent: Stem Territories (port-koba)
    1 - Continent: North Crown (frostmere)
    2 - Banana Line Station: Neo Musa
    3 - Banana Line Corridor: Metro -> Crescent
    6 - War Sentience Showcase: Metro Arena

Feature runs:
    warfront, flank, pressure, negotiate, truce, comeback, rally
EOF_SUITES
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --suite)
            [[ $# -ge 2 ]] || die "--suite requires a value"
            suite="$2"
            shift 2
            ;;
        --variant)
            [[ $# -ge 2 ]] || die "--variant requires a value"
            explicit_variants+=("$2")
            shift 2
            ;;
        --feature)
            [[ $# -ge 2 ]] || die "--feature requires a value"
            explicit_features+=("$2")
            shift 2
            ;;
        --build-dir)
            [[ $# -ge 2 ]] || die "--build-dir requires a value"
            BUILD_DIR="$2"
            shift 2
            ;;
        --config)
            [[ $# -ge 2 ]] || die "--config requires a value"
            BUILD_CONFIG="$2"
            config_set=1
            shift 2
            ;;
        --skip-build)
            skip_build=1
            shift
            ;;
        --autotest)
            [[ $# -ge 2 ]] || die "--autotest requires a value"
            autotest_seconds="$2"
            shift 2
            ;;
        --telemetry)
            telemetry=1
            shift
            ;;
        --auto-target)
            auto_target=1
            shift
            ;;
        --target-hotkey)
            target_hotkey=1
            shift
            ;;
        --defaults)
            playtest_defaults=1
            defaults_locked=1
            shift
            ;;
        --no-defaults)
            playtest_defaults=0
            defaults_locked=1
            shift
            ;;
        --list)
            list_only=1
            shift
            ;;
        --help|-h)
            usage
            exit 0
            ;;
        *)
            die "unknown argument '$1'"
            ;;
    esac
done

if [[ "$list_only" -eq 1 ]]; then
    print_suite_list
    exit 0
fi

append_suite_variants "$suite"
append_suite_features "$suite"

if [[ "$config_set" -eq 0 ]]; then
    case "$suite" in
        playtest|showcase|metro|corridor|continent|features|all)
            BUILD_CONFIG="RelWithDebInfo"
            ;;
    esac
fi

for variant in "${explicit_variants[@]}"; do
    append_variant "$variant"
done

for feature in "${explicit_features[@]}"; do
    append_feature "$feature"
done

if [[ ${#selected_variants[@]} -eq 0 && ${#selected_features[@]} -eq 0 ]]; then
    die "no scene variants or features resolved (pick --suite, --variant, or --feature)"
fi

if [[ "$defaults_locked" -eq 0 && ( "$suite" == "playtest" || "$suite" == "features" ) ]]; then
    playtest_defaults=1
fi

if [[ "$playtest_defaults" -eq 1 ]]; then
    telemetry=1
    auto_target=1
    export BANANA_DX12_POC_FRAME_CAP_INDEX="${BANANA_DX12_POC_FRAME_CAP_INDEX:-1}"
    export BANANA_DX12_POC_UI_THROTTLE_MS="${BANANA_DX12_POC_UI_THROTTLE_MS:-100}"
fi

binary_candidates=(
    "$ROOT_DIR/$BUILD_DIR/engine/$BUILD_CONFIG/banana_engine_win32_dx12_poc.exe"
    "$ROOT_DIR/$BUILD_DIR/engine/banana_engine_win32_dx12_poc.exe"
    "$ROOT_DIR/$BUILD_DIR/engine/$BUILD_CONFIG/banana_engine_win32_dx12_poc"
    "$ROOT_DIR/$BUILD_DIR/engine/banana_engine_win32_dx12_poc"
)

if [[ "$skip_build" -eq 0 ]]; then
    build_cmd=(cmake --build "$ROOT_DIR/$BUILD_DIR" --config "$BUILD_CONFIG" --target banana_engine_win32_dx12_poc)

    if [[ "${BANANA_NATIVE_BUILD_SERIAL:-1}" == "1" ]]; then
        build_cmd+=(-- -m:1)
    fi

    echo "[dx12-playtest] building target banana_engine_win32_dx12_poc"
    "${build_cmd[@]}"
fi

poc_binary=""
for candidate in "${binary_candidates[@]}"; do
    if [[ -f "$candidate" ]]; then
        poc_binary="$candidate"
        break
    fi
    if [[ -x "$candidate" ]]; then
        poc_binary="$candidate"
        break
    fi
done

[[ -n "$poc_binary" ]] || die "cannot find banana_engine_win32_dx12_poc under $BUILD_DIR"

variant_list="${selected_variants[*]}"
feature_list="${selected_features[*]}"
if [[ -z "$variant_list" ]]; then
    variant_list="none"
fi
if [[ -z "$feature_list" ]]; then
    feature_list="none"
fi

echo "[dx12-playtest] suite=$suite variants=$variant_list features=$feature_list"

reset_feature_overrides() {
    unset BANANA_WAR_SENTIENCE_MODE_OVERRIDE
    unset BANANA_WAR_SENTIENCE_MODE_OVERRIDE_BANANA
    unset BANANA_WAR_SENTIENCE_MODE_OVERRIDE_BEAN
    unset BANANA_WAR_SENTIENCE_HUMANOID_INDEX
    unset BANANA_WAR_SENTIENCE_COORDINATION_LEVEL
    unset BANANA_WAR_SENTIENCE_EMPATHY_LEVEL
    unset BANANA_WAR_SENTIENCE_NEGOTIATE_STREAK_SEED
    unset BANANA_CONTROLLER_WAR_RADIUS
    unset BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK
    unset BANANA_CONTROLLER_WAR_POPULATION_CAP
}

apply_feature_overrides() {
    local feature="$1"

    reset_feature_overrides

    export BANANA_CONTROLLER_WAR_RADIUS="5"
    export BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK="6"
    export BANANA_CONTROLLER_WAR_POPULATION_CAP="192"

    case "$feature" in
        warfront)
            export BANANA_WAR_SENTIENCE_MODE_OVERRIDE="hold"
            export BANANA_WAR_SENTIENCE_HUMANOID_INDEX="4"
            export BANANA_WAR_SENTIENCE_COORDINATION_LEVEL="2"
            export BANANA_WAR_SENTIENCE_EMPATHY_LEVEL="1"
            ;;
        flank)
            export BANANA_WAR_SENTIENCE_MODE_OVERRIDE="flank"
            export BANANA_WAR_SENTIENCE_COORDINATION_LEVEL="8"
            ;;
        pressure)
            export BANANA_WAR_SENTIENCE_MODE_OVERRIDE="hold"
            export BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK="12"
            export BANANA_CONTROLLER_WAR_POPULATION_CAP="240"
            ;;
        negotiate)
            export BANANA_WAR_SENTIENCE_MODE_OVERRIDE="negotiate"
            export BANANA_WAR_SENTIENCE_HUMANOID_INDEX="9"
            export BANANA_WAR_SENTIENCE_EMPATHY_LEVEL="7"
            ;;
        truce)
            export BANANA_WAR_SENTIENCE_MODE_OVERRIDE="negotiate"
            export BANANA_WAR_SENTIENCE_NEGOTIATE_STREAK_SEED="6"
            export BANANA_CONTROLLER_WAR_REINFORCEMENTS_PER_TICK="2"
            export BANANA_CONTROLLER_WAR_POPULATION_CAP="96"
            ;;
        comeback)
            export BANANA_WAR_SENTIENCE_MODE_OVERRIDE="regroup"
            export BANANA_WAR_SENTIENCE_COORDINATION_LEVEL="10"
            export BANANA_WAR_SENTIENCE_EMPATHY_LEVEL="6"
            ;;
        rally)
            export BANANA_WAR_SENTIENCE_MODE_OVERRIDE="hold"
            export BANANA_WAR_SENTIENCE_COORDINATION_LEVEL="4"
            export BANANA_WAR_SENTIENCE_EMPATHY_LEVEL="2"
            ;;
        *)
            die "unknown feature '$feature'"
            ;;
    esac
}

launch_poc() {
    local variant="$1"

    export BANANA_DX12_POC_SCENE_VARIANT="$variant"

    if [[ -n "$autotest_seconds" ]]; then
        export BANANA_DX12_POC_AUTOTEST=1
        export BANANA_DX12_POC_AUTOTEST_SECONDS="$autotest_seconds"
    fi

    if [[ -n "$telemetry" ]]; then
        export BANANA_DX12_POC_TELEMETRY=1
    fi

    if [[ -n "$auto_target" ]]; then
        export BANANA_DX12_POC_AUTO_TARGET=1
    fi

    if [[ -n "$autotest_seconds" && -n "$auto_target" && -z "${BANANA_DX12_POC_AUTOTEST_ALLOW_TARGET:-}" ]]; then
        export BANANA_DX12_POC_AUTOTEST_ALLOW_TARGET=1
    fi

    if [[ -n "$target_hotkey" ]]; then
        export BANANA_DX12_POC_ENABLE_TARGET_HOTKEY=1
    fi

    "$poc_binary"
}

for feature in "${selected_features[@]}"; do
    echo "[dx12-playtest] launching feature=$feature variant=$feature_variant"
    (
        apply_feature_overrides "$feature"
        launch_poc "$feature_variant"
    )
done

for variant in "${selected_variants[@]}"; do
    label="$(variant_label "$variant")"
    echo "[dx12-playtest] launching variant=$variant label=$label"
    (
        launch_poc "$variant"
    )
done

#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BANANA_NATIVE_BUILD_DIR:-out/v3-native}"
BUILD_CONFIG="${BANANA_NATIVE_BUILD_CONFIG:-Debug}"

scenario="warfront"
ticks=24
snapshot_interval=4
war_radius=6
reinforcements=2
population_cap=128
escalation_tier=1
intelligence_stage=4
biome_stage=0
script_path=""
output_path=""
feedback_note=""
skip_build=0
non_interactive=0
is_windows_exe=0

usage() {
    cat <<'EOF'
Usage:
  bash scripts/run-native-feedback-loop.sh [options]

Options:
    --scenario <warfront|negotiate|comeback|flank|pressure|truce|rally>
  --ticks <count>
  --snapshot-interval <count>
  --war-radius <float>
  --reinforcements <count>
  --population-cap <count>
  --escalation-tier <count>
  --intelligence-stage <count>
  --biome-stage <count>
    --script <path>
  --output <path>
  --feedback-note <text>
  --build-dir <path>
  --config <name>
  --skip-build
  --non-interactive
  --help

EOF
}

die() {
    echo "[feedback-loop] error: $*" >&2
    exit 1
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --scenario)
            [[ $# -ge 2 ]] || die "--scenario requires a value"
            scenario="$2"
            shift 2
            ;;
        --ticks)
            [[ $# -ge 2 ]] || die "--ticks requires a value"
            ticks="$2"
            shift 2
            ;;
        --snapshot-interval)
            [[ $# -ge 2 ]] || die "--snapshot-interval requires a value"
            snapshot_interval="$2"
            shift 2
            ;;
        --war-radius)
            [[ $# -ge 2 ]] || die "--war-radius requires a value"
            war_radius="$2"
            shift 2
            ;;
        --reinforcements)
            [[ $# -ge 2 ]] || die "--reinforcements requires a value"
            reinforcements="$2"
            shift 2
            ;;
        --population-cap)
            [[ $# -ge 2 ]] || die "--population-cap requires a value"
            population_cap="$2"
            shift 2
            ;;
        --escalation-tier)
            [[ $# -ge 2 ]] || die "--escalation-tier requires a value"
            escalation_tier="$2"
            shift 2
            ;;
        --intelligence-stage)
            [[ $# -ge 2 ]] || die "--intelligence-stage requires a value"
            intelligence_stage="$2"
            shift 2
            ;;
        --biome-stage)
            [[ $# -ge 2 ]] || die "--biome-stage requires a value"
            biome_stage="$2"
            shift 2
            ;;
        --script)
            [[ $# -ge 2 ]] || die "--script requires a value"
            script_path="$2"
            shift 2
            ;;
        --output)
            [[ $# -ge 2 ]] || die "--output requires a value"
            output_path="$2"
            shift 2
            ;;
        --feedback-note)
            [[ $# -ge 2 ]] || die "--feedback-note requires a value"
            feedback_note="$2"
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
            shift 2
            ;;
        --skip-build)
            skip_build=1
            shift
            ;;
        --non-interactive)
            non_interactive=1
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

binary_candidates=(
    "$ROOT_DIR/$BUILD_DIR/engine/$BUILD_CONFIG/banana_native_feedback_loop_factory.exe"
    "$ROOT_DIR/$BUILD_DIR/engine/$BUILD_CONFIG/banana_native_feedback_loop_factory"
    "$ROOT_DIR/$BUILD_DIR/engine/banana_native_feedback_loop_factory.exe"
    "$ROOT_DIR/$BUILD_DIR/engine/banana_native_feedback_loop_factory"
)

if [[ "$skip_build" -eq 0 ]]; then
    build_cmd=(cmake --build "$ROOT_DIR/$BUILD_DIR" --config "$BUILD_CONFIG" --target banana_native_feedback_loop_factory)

    if [[ "${BANANA_NATIVE_BUILD_SERIAL:-1}" == "1" ]]; then
        build_cmd+=(-- -m:1)
    fi

    echo "[feedback-loop] building target banana_native_feedback_loop_factory"
    "${build_cmd[@]}"
fi

feedback_binary=""
for candidate in "${binary_candidates[@]}"; do
    if [[ -f "$candidate" ]]; then
        feedback_binary="$candidate"
        break
    fi
done

[[ -n "$feedback_binary" ]] || die "cannot find banana_native_feedback_loop_factory binary under $BUILD_DIR"

if [[ "$feedback_binary" == *.exe ]]; then
    is_windows_exe=1
fi

to_windows_path() {
    local path="$1"

    if [[ -z "$path" ]]; then
        printf '%s' ""
        return
    fi

    if [[ "$is_windows_exe" -eq 1 ]] && command -v cygpath >/dev/null 2>&1; then
        cygpath -w "$path"
    else
        printf '%s' "$path"
    fi
}

script_path_arg="$script_path"
output_path_arg="$output_path"

if [[ "$is_windows_exe" -eq 1 ]]; then
    # Ensure Windows executables receive Windows-style paths for scripts/output bundles.
    script_path_arg="$(to_windows_path "$script_path")"
    output_path_arg="$(to_windows_path "$output_path")"
fi

cmd=()

cmd+=("$feedback_binary"
    --scenario "$scenario"
    --ticks "$ticks"
    --snapshot-interval "$snapshot_interval"
    --war-radius "$war_radius"
    --reinforcements "$reinforcements"
    --population-cap "$population_cap"
    --escalation-tier "$escalation_tier"
    --intelligence-stage "$intelligence_stage"
    --biome-stage "$biome_stage")

if [[ -n "$script_path" ]]; then
    cmd+=(--script "$script_path_arg")
fi

if [[ -n "$output_path" ]]; then
    output_dir="$(dirname "$output_path")"
    mkdir -p "$output_dir"
    cmd+=(--output "$output_path_arg")
fi

if [[ -n "$feedback_note" ]]; then
    cmd+=(--feedback-note "$feedback_note")
fi

if [[ "$non_interactive" -eq 1 ]]; then
    cmd+=(--non-interactive)
fi

if [[ -n "$script_path" ]]; then
    echo "[feedback-loop] running script=$script_path fallback_scenario=$scenario ticks=$ticks snapshot_interval=$snapshot_interval"
else
    echo "[feedback-loop] running scenario=$scenario ticks=$ticks snapshot_interval=$snapshot_interval"
fi
"${cmd[@]}"

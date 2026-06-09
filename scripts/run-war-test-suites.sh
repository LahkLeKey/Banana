#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BANANA_NATIVE_BUILD_DIR:-out/v3-native}"
BUILD_CONFIG="${BANANA_NATIVE_BUILD_CONFIG:-Debug}"
OUTPUT_DIR="${BANANA_NATIVE_FEEDBACK_OUTPUT_DIR:-$ROOT_DIR/artifacts/native/human-feedback}"
SCRIPT_DIR="${BANANA_NATIVE_FEEDBACK_SCRIPT_DIR:-}"

suite="focused"
ticks=24
snapshot_interval=4
war_radius=6
reinforcements=2
population_cap=128
escalation_tier=1
intelligence_stage=4
biome_stage=0
list_only=0
dry_run=0
non_interactive=0

declare -a explicit_scenarios=()
declare -a selected_scenarios=()

die() {
    echo "[war-suite] error: $*" >&2
    exit 1
}

usage() {
    cat <<'EOF_USAGE'
Usage:
  bash scripts/run-war-test-suites.sh [options]

Options:
    --suite <name>             Named suite: focused, evidence, soak, gameplay, legacy, mmo-only, none
    --scenario <name>          Add a specific scenario (repeatable): warfront, negotiate, comeback, flank, pressure, truce, rally
  --ticks <count>            Tick count per scenario (default: 24)
  --snapshot-interval <n>    Snapshot cadence (default: 4)
  --war-radius <float>       Gameplay war radius (default: 6)
  --reinforcements <count>   Reinforcements per tick (default: 2)
  --population-cap <count>   Population cap (default: 128)
  --escalation-tier <count>  Escalation tier (default: 1)
  --intelligence-stage <n>   Intelligence stage (default: 4)
  --biome-stage <n>          Biome stage index (default: 0)
  --build-dir <path>         Build folder (default: out/v3-native)
  --config <name>            Build config (default: Debug)
  --output-dir <path>        Per-scenario log output directory
    --script-dir <path>        Optional directory with <scenario>.dx12play scripts
  --list                     Print suite map
  --non-interactive          Disable operator prompt in loop binary
  --dry-run                  Print resolved commands without executing
  --help                     Show this help

Examples:
  bash scripts/run-war-test-suites.sh --suite focused --non-interactive
  bash scripts/run-war-test-suites.sh --suite evidence --ticks 32 --snapshot-interval 8
    bash scripts/run-war-test-suites.sh --suite none --scenario negotiate --scenario comeback
    bash scripts/run-war-test-suites.sh --suite gameplay --non-interactive
EOF_USAGE
}

append_unique_scenario() {
    local scenario_name="$1"

    for existing in "${selected_scenarios[@]}"; do
        if [[ "$existing" == "$scenario_name" ]]; then
            return
        fi
    done

    selected_scenarios+=("$scenario_name")
}

append_suite_scenarios() {
    local suite_name="$1"

    case "$suite_name" in
        focused)
            append_unique_scenario "warfront"
            append_unique_scenario "negotiate"
            ;;
        evidence)
            append_unique_scenario "warfront"
            append_unique_scenario "negotiate"
            append_unique_scenario "comeback"
            ;;
        soak)
            append_unique_scenario "warfront"
            append_unique_scenario "comeback"
            ;;
        gameplay)
            append_unique_scenario "warfront"
            append_unique_scenario "flank"
            append_unique_scenario "pressure"
            append_unique_scenario "negotiate"
            append_unique_scenario "truce"
            append_unique_scenario "comeback"
            append_unique_scenario "rally"
            ;;
        legacy)
            append_unique_scenario "warfront"
            ;;
        mmo-only)
            append_unique_scenario "warfront"
            ;;
        none)
            ;;
        *)
            die "unknown suite '$suite_name'"
            ;;
    esac
}

print_suite_list() {
    cat <<'EOF_SUITES'
Available suites:
  focused   - short operator loop: warfront + negotiate
  evidence  - canonical baseline loop: warfront + negotiate + comeback
  soak      - long pressure loop for warfront/comeback pair
    gameplay  - extended gameplay coverage using dx12 playloop scripts
  legacy    - single warfront loop for backward-compatible script calls
  mmo-only  - alias for warfront-only loop
  none      - no preset scenario; use --scenario
EOF_SUITES
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --suite)
            [[ $# -ge 2 ]] || die "--suite requires a value"
            suite="$2"
            shift 2
            ;;
        --scenario)
            [[ $# -ge 2 ]] || die "--scenario requires a value"
            explicit_scenarios+=("$2")
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
        --output-dir)
            [[ $# -ge 2 ]] || die "--output-dir requires a value"
            OUTPUT_DIR="$2"
            shift 2
            ;;
        --script-dir)
            [[ $# -ge 2 ]] || die "--script-dir requires a value"
            SCRIPT_DIR="$2"
            shift 2
            ;;
        --list)
            list_only=1
            shift
            ;;
        --dry-run)
            dry_run=1
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

if [[ "$list_only" -eq 1 ]]; then
    print_suite_list
    exit 0
fi

append_suite_scenarios "$suite"

if [[ "$suite" == "gameplay" && -z "$SCRIPT_DIR" ]]; then
    SCRIPT_DIR="$ROOT_DIR/tests/native/feedback/scripts"
fi

for scenario_name in "${explicit_scenarios[@]}"; do
    append_unique_scenario "$scenario_name"
done

if [[ ${#selected_scenarios[@]} -eq 0 ]]; then
    die "no scenarios resolved (pick --suite or --scenario)"
fi

mkdir -p "$OUTPUT_DIR"

if [[ "$suite" == "soak" && "$ticks" -lt 48 ]]; then
    ticks=48
fi

echo "[war-suite] root=$ROOT_DIR"
echo "[war-suite] suite=$suite scenarios=${selected_scenarios[*]}"
echo "[war-suite] build_dir=$BUILD_DIR config=$BUILD_CONFIG"
echo "[war-suite] output_dir=$OUTPUT_DIR"
if [[ -n "$SCRIPT_DIR" ]]; then
    echo "[war-suite] script_dir=$SCRIPT_DIR"
fi

i=0
for scenario_name in "${selected_scenarios[@]}"; do
    i=$((i + 1))
    scenario_log="$OUTPUT_DIR/${suite}-${i}-${scenario_name}.log"

    cmd=()

    cmd+=(bash "$ROOT_DIR/scripts/run-native-feedback-loop.sh"
        --scenario "$scenario_name"
        --ticks "$ticks"
        --snapshot-interval "$snapshot_interval"
        --war-radius "$war_radius"
        --reinforcements "$reinforcements"
        --population-cap "$population_cap"
        --escalation-tier "$escalation_tier"
        --intelligence-stage "$intelligence_stage"
        --biome-stage "$biome_stage"
        --build-dir "$BUILD_DIR"
        --config "$BUILD_CONFIG"
        --output "$scenario_log")

    if [[ -n "$SCRIPT_DIR" ]]; then
        scenario_script="$SCRIPT_DIR/${scenario_name}.dx12play"
        [[ -f "$scenario_script" ]] || die "missing script for scenario '$scenario_name': $scenario_script"
        cmd+=(--script "$scenario_script")
    fi

    if [[ "$i" -gt 1 ]]; then
        cmd+=(--skip-build)
    fi

    if [[ "$non_interactive" -eq 1 ]]; then
        cmd+=(--non-interactive)
    fi

    echo "[war-suite] scenario=$scenario_name log=$scenario_log"

    if [[ "$dry_run" -eq 1 ]]; then
        echo "[war-suite] dry-run: ${cmd[*]}"
        continue
    fi

    "${cmd[@]}"
done

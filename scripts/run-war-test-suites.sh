#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BANANA_NATIVE_BUILD_DIR:-out/v3-native}"
BUILD_CONFIG="${BANANA_NATIVE_BUILD_CONFIG:-Debug}"

suite="focused"
custom_regex=""
list_only=0
dry_run=0

declare -a explicit_tests=()
declare -a selected_tests=()

die() {
    echo "[war-suite] error: $*" >&2
    exit 1
}

print_usage() {
    cat <<'EOF'
Usage:
  bash scripts/run-war-test-suites.sh [options]

Options:
    --suite <name>       Named suite: focused, evidence, soak, legacy, mmo-only, none
  --test <ctest-name>  Add one exact test by ctest name (repeatable)
  --regex <pattern>    Run with a custom ctest regex pattern
  --build-dir <path>   Override ctest --test-dir (default: out/v3-native)
  --config <name>      Override ctest -C config (default: Debug)
  --list               Print suite names and included tests
  --dry-run            Print resolved ctest command without executing
  --help               Show this help

Examples:
  bash scripts/run-war-test-suites.sh --suite focused
  bash scripts/run-war-test-suites.sh --suite legacy
  bash scripts/run-war-test-suites.sh --test banana_runtime_gameplay_warfront_comeback_bonus_test
  bash scripts/run-war-test-suites.sh --suite none --test banana_runtime_full_mmo_demo_test --test banana_runtime_tick_orchestration_war_escalation_test
  bash scripts/run-war-test-suites.sh --regex "banana_(runtime_gameplay_warfront_expansion|runtime_full_mmo_demo)_test"
EOF
}

append_unique_test() {
    local test_name="$1"

    for existing in "${selected_tests[@]}"; do
        if [[ "$existing" == "$test_name" ]]; then
            return
        fi
    done

    selected_tests+=("$test_name")
}

append_suite_tests() {
    local suite_name="$1"

    case "$suite_name" in
        focused)
            append_unique_test "banana_runtime_tick_orchestration_war_terrain_expansion_test"
            append_unique_test "banana_runtime_tick_orchestration_war_escalation_test"
            append_unique_test "banana_runtime_tick_orchestration_war_intelligence_growth_test"
            append_unique_test "banana_runtime_tick_orchestration_war_game_of_life_sentience_test"
            append_unique_test "banana_runtime_tick_orchestration_war_overcrowd_expansion_test"
            append_unique_test "banana_runtime_tick_orchestration_war_long_run_soak_test"
            append_unique_test "banana_runtime_gameplay_warfront_expansion_test"
            append_unique_test "banana_runtime_gameplay_warfront_comeback_bonus_test"
            append_unique_test "banana_runtime_gameplay_warfront_negotiate_deescalation_test"
            append_unique_test "banana_runtime_gameplay_warfront_negotiate_factory_test"
            append_unique_test "banana_runtime_gameplay_warfront_intelligence_feature_unlock_test"
            append_unique_test "banana_runtime_full_mmo_demo_test"
            ;;
        evidence)
            append_unique_test "banana_combat_controller_signal_test"
            append_unique_test "banana_wildlife_controller_war_state_test"
            append_unique_test "banana_runtime_controller_team_war_test"
            append_unique_test "banana_runtime_tick_post_phase_test"
            append_unique_test "banana_runtime_tick_orchestration_war_terrain_expansion_test"
            append_unique_test "banana_runtime_tick_orchestration_war_escalation_test"
            append_unique_test "banana_runtime_tick_orchestration_war_intelligence_growth_test"
            append_unique_test "banana_runtime_tick_orchestration_war_game_of_life_sentience_test"
            append_unique_test "banana_runtime_tick_orchestration_war_overcrowd_expansion_test"
            append_unique_test "banana_runtime_tick_orchestration_war_long_run_soak_test"
            append_unique_test "banana_runtime_gameplay_warfront_expansion_test"
            append_unique_test "banana_runtime_gameplay_warfront_comeback_bonus_test"
            append_unique_test "banana_runtime_gameplay_warfront_negotiate_deescalation_test"
            append_unique_test "banana_runtime_gameplay_warfront_negotiate_factory_test"
            append_unique_test "banana_runtime_gameplay_warfront_intelligence_feature_unlock_test"
            append_unique_test "banana_runtime_demo_scene_catalog_vector_profile_test"
            append_unique_test "banana_runtime_demo_scene_catalog_war_policy_test"
            ;;
        soak)
            append_unique_test "banana_runtime_tick_orchestration_war_overcrowd_expansion_test"
            append_unique_test "banana_runtime_tick_orchestration_war_intelligence_growth_test"
            append_unique_test "banana_runtime_tick_orchestration_war_game_of_life_sentience_test"
            append_unique_test "banana_runtime_tick_orchestration_war_long_run_soak_test"
            append_unique_test "banana_runtime_gameplay_warfront_intelligence_feature_unlock_test"
            ;;
        legacy)
            append_unique_test "banana_combat_controller_signal_test"
            append_unique_test "banana_wildlife_controller_war_state_test"
            append_unique_test "banana_runtime_controller_team_war_test"
            append_unique_test "banana_runtime_tick_post_phase_test"
            append_unique_test "banana_runtime_tick_orchestration_war_terrain_expansion_test"
            append_unique_test "banana_runtime_tick_orchestration_war_escalation_test"
            append_unique_test "banana_runtime_tick_orchestration_war_intelligence_growth_test"
            append_unique_test "banana_runtime_gameplay_warfront_expansion_test"
            append_unique_test "banana_runtime_demo_scene_catalog_vector_profile_test"
            append_unique_test "banana_runtime_demo_scene_catalog_war_policy_test"
            ;;
        mmo-only)
            append_unique_test "banana_runtime_full_mmo_demo_test"
            ;;
        none)
            ;;
        *)
            die "unknown suite '$suite_name' (use --list)"
            ;;
    esac
}

print_suite_list() {
    cat <<'EOF'
Available suites:
    focused   - Active war growth + comeback + intelligence-stage feature smoke
  evidence  - Full combat-vs-wildlife war evidence suite
    soak      - Long-run war pressure soak and intelligence unlock coverage
  legacy    - Pre-comeback legacy war suite (kept for back-compat runs)
  mmo-only  - Full MMO demo only
  none      - No preset tests; use --test or --regex
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --suite)
            [[ $# -ge 2 ]] || die "--suite requires a value"
            suite="$2"
            shift 2
            ;;
        --test)
            [[ $# -ge 2 ]] || die "--test requires a ctest test name"
            explicit_tests+=("$2")
            shift 2
            ;;
        --regex)
            [[ $# -ge 2 ]] || die "--regex requires a pattern"
            custom_regex="$2"
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
        --list)
            list_only=1
            shift
            ;;
        --dry-run)
            dry_run=1
            shift
            ;;
        --help|-h)
            print_usage
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

append_suite_tests "$suite"

for test_name in "${explicit_tests[@]}"; do
    append_unique_test "$test_name"
done

if [[ -z "$custom_regex" ]]; then
    if [[ ${#selected_tests[@]} -eq 0 ]]; then
        die "no tests resolved (choose --suite, --test, or --regex)"
    fi

    regex_joined=""
    for test_name in "${selected_tests[@]}"; do
        if [[ -n "$regex_joined" ]]; then
            regex_joined+="|"
        fi
        regex_joined+="$test_name"
    done

    custom_regex="^(${regex_joined})$"
fi

cmd=(ctest --test-dir "$BUILD_DIR" -C "$BUILD_CONFIG" --output-on-failure -R "$custom_regex")

echo "[war-suite] root=$ROOT_DIR"
echo "[war-suite] build_dir=$BUILD_DIR config=$BUILD_CONFIG"
echo "[war-suite] suite=$suite"

if [[ ${#selected_tests[@]} -gt 0 ]]; then
    echo "[war-suite] resolved tests:"
    for test_name in "${selected_tests[@]}"; do
        echo "  - $test_name"
    done
fi

echo "[war-suite] regex=$custom_regex"

if [[ "$dry_run" -eq 1 ]]; then
    echo "[war-suite] dry-run command: ${cmd[*]}"
    exit 0
fi

cd "$ROOT_DIR"
"${cmd[@]}"

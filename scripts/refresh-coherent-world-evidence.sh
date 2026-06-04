#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${BANANA_NATIVE_BUILD_DIR:-out/v3-native}"
BUILD_CONFIG="${BANANA_NATIVE_BUILD_CONFIG:-Debug}"
ARTIFACT_DIR="$ROOT_DIR/artifacts/native/031-unified-coherent-world"
FEEDBACK_DIR="$ARTIFACT_DIR/human-feedback"
SCRIPT_DIR="${BANANA_NATIVE_FEEDBACK_SCRIPT_DIR:-}"

mkdir -p "$ARTIFACT_DIR" "$FEEDBACK_DIR"

cd "$ROOT_DIR"

run_and_capture() {
    local label="$1"
    local suite_name="$2"
    local ticks="$3"
    local snapshot_interval="$4"
    local output_file="$5"
    local -a cmd

    echo "[031-refresh] writing ${label} -> $output_file"

    cmd=(bash scripts/run-war-test-suites.sh
        --suite "$suite_name"
        --ticks "$ticks"
        --snapshot-interval "$snapshot_interval"
        --build-dir "$BUILD_DIR"
        --config "$BUILD_CONFIG"
        --output-dir "$FEEDBACK_DIR/$suite_name"
        --non-interactive)

    if [[ -n "$SCRIPT_DIR" ]]; then
        cmd+=(--script-dir "$SCRIPT_DIR")
    fi

    "${cmd[@]}" | tee "$output_file"
}

run_and_capture "umbrella baseline" "focused" 24 4 "$ARTIFACT_DIR/umbrella-baseline.txt"
run_and_capture "full engine baseline" "evidence" 32 6 "$ARTIFACT_DIR/full-engine-baseline.txt"
run_and_capture "full MMO demo baseline" "mmo-only" 24 4 "$ARTIFACT_DIR/full-mmo-demo.txt"
run_and_capture "combat-vs-wildlife war evidence" "evidence" 48 8 "$ARTIFACT_DIR/combat-wildlife-war-evidence.txt"

echo "[031-refresh] complete"

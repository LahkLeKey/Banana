#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
OUTPUT_DIR_DEFAULT="$ROOT_DIR/artifacts/native/032-demo-frame-qa/runs"

run_dir=""
output_dir="${BANANA_DEMO_FRAME_OUTPUT_DIR:-$OUTPUT_DIR_DEFAULT}"
run_label=""

usage() {
    cat <<'EOF'
Usage:
  bash tests/native/feedback/validate_demo_frame_bundle.sh [options]

Options:
  --run-dir <path>     Explicit run directory to validate
  --output-dir <path>  Root runs directory (default: artifacts/native/032-demo-frame-qa/runs)
  --run-label <label>  Filter latest run by label substring
  --help               Show help
EOF
}

die() {
    echo "[demo-frame-validate] error: $*" >&2
    exit 1
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --run-dir)
            [[ $# -ge 2 ]] || die "--run-dir requires a value"
            run_dir="$2"
            shift 2
            ;;
        --output-dir)
            [[ $# -ge 2 ]] || die "--output-dir requires a value"
            output_dir="$2"
            shift 2
            ;;
        --run-label)
            [[ $# -ge 2 ]] || die "--run-label requires a value"
            run_label="$2"
            shift 2
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

if [[ -z "$run_dir" ]]; then
    if [[ ! -d "$output_dir" ]]; then
        die "runs directory not found: $output_dir"
    fi

    if [[ -n "$run_label" ]]; then
        run_dir="$(ls -dt "$output_dir"/*"$run_label"* 2>/dev/null | head -n 1 || true)"
    else
        run_dir="$(ls -dt "$output_dir"/* 2>/dev/null | head -n 1 || true)"
    fi
fi

[[ -n "$run_dir" ]] || die "no run directory resolved"
[[ -d "$run_dir" ]] || die "run directory not found: $run_dir"

manifest="$run_dir/manifest.json"
frames_dir="$run_dir/frames"

[[ -f "$manifest" ]] || die "manifest.json missing in $run_dir"
[[ -d "$frames_dir" ]] || die "frames directory missing in $run_dir"

grep -q '"run_id"' "$manifest" || die "manifest missing run_id"

bmp_count=0
while IFS= read -r -d '' bmp; do
    bmp_count=$((bmp_count + 1))
    metadata="${bmp%.bmp}.json"
    [[ -f "$metadata" ]] || die "missing metadata for frame: $bmp"
done < <(find "$frames_dir" -type f -name "*.bmp" -print0)

[[ "$bmp_count" -gt 0 ]] || die "no bmp frames found in $frames_dir"

echo "[demo-frame-validate] ok run_dir=$run_dir frames=$bmp_count"

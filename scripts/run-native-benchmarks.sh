#!/usr/bin/env bash
# Runs the Banana native C benchmark suite using hyperfine.
# Requires: hyperfine (https://github.com/sharkdp/hyperfine)
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BIN="$ROOT/out/native/linux-x86_64/libbanana_native_bench"
hyperfine --warmup 5 --min-runs 20 "$BIN" --export-json artifacts/benchmarks/results.json

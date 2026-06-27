#!/usr/bin/env bash
set -euo pipefail

# Runs local dialogue taxonomy smoke checks against the native library.
# Exits non-zero if any expected pass/fail behavior regresses.

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
CLI_DIR="$REPO_ROOT/cli/banana"
NATIVE_LIB_DEFAULT="$REPO_ROOT/out/v3-native/Debug/banana_native.dll"
NATIVE_LIB_PATH="${1:-$NATIVE_LIB_DEFAULT}"

if [[ ! -f "$NATIVE_LIB_PATH" ]]; then
  echo "error: native library not found at $NATIVE_LIB_PATH" >&2
  exit 1
fi

if ! command -v python >/dev/null 2>&1; then
  echo "error: python is required but not found on PATH" >&2
  exit 1
fi

run_cmd() {
  (cd "$CLI_DIR" && "$@")
}

# 1) Standard dialogue path should pass.
run_cmd python -m banana_cli k3h4 dialogue-sample --seed 11 --preset pilot-edda \
  | run_cmd python -m banana_cli k3h4 dialogue-run --native-lib "$NATIVE_LIB_PATH" \
  | run_cmd python -m banana_cli k3h4 dialogue-export --format json \
  >/dev/null

# 2) Hard-block preset should pass with reserved deny range.
hard_block_json="$(run_cmd python -m banana_cli k3h4 dialogue-sample --seed 11 --preset hard-block-self-harm \
  | run_cmd python -m banana_cli k3h4 dialogue-run --native-lib "$NATIVE_LIB_PATH")"

printf '%s\n' "$hard_block_json" | run_cmd python -m banana_cli k3h4 dialogue-export --format json >/dev/null

HARD_BLOCK_JSON="$hard_block_json" python - <<'PY'
import json
import os
import sys

payload = json.loads(os.environ["HARD_BLOCK_JSON"])
metadata = payload.get("decision_metadata", {})
policy = metadata.get("response_policy")
code = metadata.get("deny_reason_code")

if policy != "hard_block":
    raise SystemExit(f"expected hard_block policy, got {policy!r}")
if not isinstance(code, int) or not (9100 <= code <= 9199):
    raise SystemExit(f"expected hard_block deny_reason_code in 9100..9199, got {code!r}")
PY

# 3) Invalid hard-block deny code must fail closed in dialogue-export.
set +e
printf '%s\n' "$hard_block_json" \
  | python -c "import json,sys; p=json.load(sys.stdin); p['decision_metadata']['deny_reason_code']=42; print(json.dumps(p,separators=(',',':')))" \
  | run_cmd python -m banana_cli k3h4 dialogue-export --format json >/dev/null 2>/dev/null
status=$?
set -e

if [[ $status -eq 0 ]]; then
  echo "error: expected dialogue-export to fail on invalid hard_block deny_reason_code" >&2
  exit 1
fi

echo "dialogue taxonomy smoke: ok"

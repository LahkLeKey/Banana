#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SPEC_DIR="$ROOT_DIR/.specify/specs/008-steam-launch-gating"

required_files=(
  "$SPEC_DIR/checklists/steam-launch-gating-readiness.md"
  "$ROOT_DIR/src/typescript/api/src/lib/contracts/launchGateReasonCodes.ts"
  "$ROOT_DIR/src/native/include/banana_launch_gate_policy.h"
  "$ROOT_DIR/src/typescript/react/src/lib/launchGateTypes.ts"
)

missing=0
for file in "${required_files[@]}"; do
  if [[ ! -f "$file" ]]; then
    echo "[launch-gate] missing required file: $file" >&2
    missing=1
  fi
done

if [[ "$missing" -ne 0 ]]; then
  exit 1
fi

if [[ ! -f "$SPEC_DIR/tasks.md" ]]; then
  echo "[launch-gate] missing tasks file: $SPEC_DIR/tasks.md" >&2
  exit 1
fi

remaining=$(grep -c '^- \[ \] T' "$SPEC_DIR/tasks.md" || true)
completed=$(grep -c '^- \[x\] T' "$SPEC_DIR/tasks.md" || true)

echo "[launch-gate] completed tasks: $completed"
echo "[launch-gate] remaining tasks: $remaining"

echo "[launch-gate] setup contracts validated"

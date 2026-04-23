#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

HISTORY_PATH="${BANANA_REGISTRY_HISTORY_PATH:-data/not-banana/model-release-history}"
SNAPSHOT_ID="${BANANA_REGISTRY_RESTORE_SNAPSHOT:-latest}"
OUTPUT_ROOT="${BANANA_REGISTRY_RESTORE_OUTPUT_ROOT:-artifacts}"
OVERWRITE="${BANANA_REGISTRY_RESTORE_OVERWRITE:-true}"
RELEASE_IDS="${BANANA_REGISTRY_RESTORE_RELEASE_IDS:-}"

RESTORE_ARGS=(
  restore-history
  --history-path "$HISTORY_PATH"
  --snapshot "$SNAPSHOT_ID"
  --output-root "$OUTPUT_ROOT"
)

if [[ "$OVERWRITE" == "false" ]]; then
  RESTORE_ARGS+=(--no-overwrite)
fi

if [[ -n "$RELEASE_IDS" ]]; then
  IFS=',' read -r -a release_items <<< "$RELEASE_IDS"
  for raw_release in "${release_items[@]}"; do
    release_id="$(echo "$raw_release" | xargs)"
    if [[ -z "$release_id" ]]; then
      continue
    fi
    RESTORE_ARGS+=(--release-id "$release_id")
  done
fi

python scripts/manage-not-banana-model-image.py "${RESTORE_ARGS[@]}"

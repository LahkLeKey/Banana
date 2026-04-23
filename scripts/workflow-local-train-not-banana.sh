#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

RELEASE_ID="${BANANA_RELEASE_ID:-local-validation}"
VOCAB_SIZE="${BANANA_TRAIN_VOCAB_SIZE:-24}"
TRAIN_PROFILE="${BANANA_TRAIN_PROFILE:-ci}"
SESSION_MODE="${BANANA_TRAIN_SESSION_MODE:-single}"
MAX_SESSIONS="${BANANA_TRAIN_MAX_SESSIONS:-1}"
MIN_SIGNAL="${BANANA_TRAIN_MIN_SIGNAL:-0.7}"
MIN_F1="${BANANA_TRAIN_MIN_F1:-0.7}"
PERSIST_REGISTRY_HISTORY="${BANANA_PERSIST_REGISTRY_HISTORY:-true}"

MODEL_OUTPUT_DIR="artifacts/not-banana-model/${RELEASE_ID}"
REGISTRY_DIR="artifacts/not-banana-model-registry/${RELEASE_ID}"
SNAPSHOT_DIR="artifacts/not-banana-model-registry-snapshots/${RELEASE_ID}"
RELEASE_BUNDLE_DIR="artifacts/not-banana-release-artifacts/not-banana-model-${RELEASE_ID}"

python scripts/train-not-banana-model.py \
  --corpus data/not-banana/corpus.json \
  --output "$MODEL_OUTPUT_DIR" \
  --vocab-size "$VOCAB_SIZE" \
  --training-profile "$TRAIN_PROFILE" \
  --session-mode "$SESSION_MODE" \
  --max-sessions "$MAX_SESSIONS" \
  --min-signal-score "$MIN_SIGNAL" \
  --min-f1 "$MIN_F1"

python scripts/manage-not-banana-model-image.py create \
  --model-dir "$MODEL_OUTPUT_DIR" \
  --registry-dir "$REGISTRY_DIR" \
  --channel candidate \
  --parent-from-channel stable

python scripts/manage-not-banana-model-image.py verify \
  --registry-dir "$REGISTRY_DIR" \
  --channel candidate

python scripts/manage-not-banana-model-image.py snapshot \
  --registry-dir "$REGISTRY_DIR" \
  --snapshot-dir "$SNAPSHOT_DIR" \
  --label "${RELEASE_ID}-local-${GITHUB_RUN_ID:-local-run}-${GITHUB_RUN_ATTEMPT:-1}"

rm -rf "$RELEASE_BUNDLE_DIR"
mkdir -p "$RELEASE_BUNDLE_DIR"
cp -R "$MODEL_OUTPUT_DIR" "$RELEASE_BUNDLE_DIR/not-banana-model"
cp -R "$REGISTRY_DIR" "$RELEASE_BUNDLE_DIR/not-banana-model-registry"
cp -R "$SNAPSHOT_DIR" "$RELEASE_BUNDLE_DIR/not-banana-model-registry-snapshots"

if [[ "$PERSIST_REGISTRY_HISTORY" == "true" ]]; then
  export BANANA_LOCAL_DRY_RUN="${BANANA_LOCAL_DRY_RUN:-true}"
  export BANANA_RELEASE_ARTIFACTS_SOURCE="artifacts/not-banana-release-artifacts"
  bash scripts/workflow-persist-registry-history-pr.sh
else
  echo "persist_registry_history=false, skipping PR path validation."
fi

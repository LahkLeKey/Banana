#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
OUTPUT_DIR="${ROOT_DIR}/artifacts/storybook/panels-static"

DEPLOY_HOST="${PANELS_STORYBOOK_DEPLOY_HOST:-panels.banana.engineer}"
DEPLOY_USER="${PANELS_STORYBOOK_DEPLOY_USER:-}"
DEPLOY_PORT="${PANELS_STORYBOOK_DEPLOY_PORT:-22}"
DEPLOY_PATH="${PANELS_STORYBOOK_DEPLOY_PATH:-/var/www/panels.banana.engineer}"

if [[ -z "${DEPLOY_USER}" ]]; then
  echo "[panels-storybook-deploy] ERROR: set PANELS_STORYBOOK_DEPLOY_USER for SSH deployment." >&2
  exit 1
fi

if ! command -v ssh >/dev/null 2>&1; then
  echo "[panels-storybook-deploy] ERROR: ssh client is required but not installed or not on PATH." >&2
  exit 1
fi

bash "${SCRIPT_DIR}/panels-storybook-build.sh" --output "${OUTPUT_DIR}"

echo "[panels-storybook-deploy] deploying to ${DEPLOY_USER}@${DEPLOY_HOST}:${DEPLOY_PATH}"

if command -v rsync >/dev/null 2>&1; then
  rsync -av --delete \
    -e "ssh -p ${DEPLOY_PORT}" \
    "${OUTPUT_DIR}/" "${DEPLOY_USER}@${DEPLOY_HOST}:${DEPLOY_PATH}/"
else
  echo "[panels-storybook-deploy] rsync not found, using tar+scp fallback"
  TMP_TAR="$(mktemp "${ROOT_DIR}/artifacts/storybook/panels-static.XXXXXX.tar.gz")"
  tar -C "${OUTPUT_DIR}" -czf "${TMP_TAR}" .
  scp -P "${DEPLOY_PORT}" "${TMP_TAR}" "${DEPLOY_USER}@${DEPLOY_HOST}:/tmp/panels-static.tar.gz"
  ssh -p "${DEPLOY_PORT}" "${DEPLOY_USER}@${DEPLOY_HOST}" \
    "mkdir -p '${DEPLOY_PATH}' && tar -xzf /tmp/panels-static.tar.gz -C '${DEPLOY_PATH}' && rm -f /tmp/panels-static.tar.gz"
  rm -f "${TMP_TAR}"
fi

echo "[panels-storybook-deploy] complete: https://${DEPLOY_HOST}"

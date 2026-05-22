#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

DEPOT_ID="${DEPOT_ID:-4767151}"
BUILD_ID="${BUILD_ID:-dev-candidate}"
CONTENT_ROOT="${CONTENT_ROOT:-artifacts/steam-build/${BUILD_ID}/win-unpacked}"
OUT_DIR="${OUT_DIR:-artifacts/steam-build/${BUILD_ID}/http-upload}"
BUILD_CANDIDATE="${BUILD_CANDIDATE:-true}"

cd "${ROOT_DIR}"

echo "[steam-http-zip] root: ${ROOT_DIR}"
echo "[steam-http-zip] depot: ${DEPOT_ID}"
echo "[steam-http-zip] content root: ${CONTENT_ROOT}"

if [[ "${BUILD_CANDIDATE}" == "true" ]]; then
  echo "[steam-http-zip] building candidate first"
  bash scripts/build-steam-playable-candidate.sh
fi

if [[ "${CONTENT_ROOT}" =~ ^[A-Za-z]:[\\/] || "${CONTENT_ROOT}" =~ ^/ ]]; then
  CONTENT_PATH_ABS="${CONTENT_ROOT}"
else
  CONTENT_PATH_ABS="${ROOT_DIR}/${CONTENT_ROOT}"
fi

mkdir -p "${OUT_DIR}"
STAMP="$(date +%Y%m%d-%H%M%S)"
ZIP_PATH="${OUT_DIR}/depot-${DEPOT_ID}-${STAMP}.zip"

if [[ "${ZIP_PATH}" =~ ^[A-Za-z]:[\\/] || "${ZIP_PATH}" =~ ^/ ]]; then
  ZIP_PATH_ABS="${ZIP_PATH}"
else
  ZIP_PATH_ABS="${ROOT_DIR}/${ZIP_PATH}"
fi

if command -v cygpath >/dev/null 2>&1; then
  CONTENT_PATH_WIN="$(cygpath -w "${CONTENT_PATH_ABS}")"
  ZIP_PATH_WIN="$(cygpath -w "${ZIP_PATH_ABS}")"
else
  CONTENT_PATH_WIN="${CONTENT_PATH_ABS}"
  ZIP_PATH_WIN="${ZIP_PATH_ABS}"
fi

# Compress in PowerShell for reliable Windows zip behavior.
powershell -NoProfile -Command "\
\$ErrorActionPreference='Stop'; \
\$src = Resolve-Path '${CONTENT_PATH_WIN}'; \
\$dst = '${ZIP_PATH_WIN}'; \
if (Test-Path \$dst) { Remove-Item -Force \$dst }; \
Compress-Archive -Path (Join-Path \$src '*') -DestinationPath \$dst -CompressionLevel Optimal; \
\$size = (Get-Item \$dst).Length; \
Write-Output ('[steam-http-zip] zip ready: ' + \$dst); \
Write-Output ('[steam-http-zip] bytes: ' + \$size)\
"

echo "[steam-http-zip] Upload this file in Steamworks -> App Admin -> Builds -> Upload Depot ${DEPOT_ID}"
echo "[steam-http-zip] Recommended mode: Standard (or Merge with Baseline if uploading partial content)"

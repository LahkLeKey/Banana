#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

APP_ID="${APP_ID:-4767150}"
DEPOT_ID="${DEPOT_ID:-}"
BUILD_ID="${BUILD_ID:-dev-candidate}"
CONTENT_ROOT="${CONTENT_ROOT:-artifacts/steam-build/dev-candidate/win-unpacked}"
BRANCH="${BRANCH:-default}"
STEAMCMD_PATH="${STEAMCMD_PATH:-}"
STEAM_USERNAME="${STEAM_USERNAME:-}"
STEAM_PASSWORD="${STEAM_PASSWORD:-}"
STEAM_GUARD_CODE="${STEAM_GUARD_CODE:-}"
BUILD_CANDIDATE="${BUILD_CANDIDATE:-true}"
DRY_RUN="${DRY_RUN:-false}"
STEAM_PIPE_DIR="artifacts/steam-build/${BUILD_ID}/steam-pipe"
APP_BUILD_VDF="${STEAM_PIPE_DIR}/app_build_${APP_ID}.vdf"
DEPOT_BUILD_VDF="${STEAM_PIPE_DIR}/depots/depot_build_${DEPOT_ID}.vdf"
LOG_DIR="${STEAM_PIPE_DIR}/logs"

if [[ -z "${DEPOT_ID}" ]]; then
  echo "[steam-publish] ERROR: DEPOT_ID is required." >&2
  echo "[steam-publish] Example: DEPOT_ID=4767151 STEAM_USERNAME=myuser STEAM_PASSWORD=... scripts/steam-publish-on-demand.sh" >&2
  exit 1
fi

if [[ -z "${STEAM_USERNAME}" ]]; then
  echo "[steam-publish] ERROR: STEAM_USERNAME is required." >&2
  exit 1
fi

if [[ -z "${STEAM_PASSWORD}" ]]; then
  echo "[steam-publish] ERROR: STEAM_PASSWORD is required." >&2
  exit 1
fi

cd "${ROOT_DIR}"

echo "[steam-publish] root: ${ROOT_DIR}"

echo "[steam-publish] app=${APP_ID} depot=${DEPOT_ID} build=${BUILD_ID} branch=${BRANCH}"

if [[ "${BUILD_CANDIDATE}" == "true" ]]; then
  echo "[steam-publish] building candidate binary"
  bash scripts/build-steam-playable-candidate.sh
fi

if [[ ! -f "${APP_BUILD_VDF}" ]]; then
  echo "[steam-publish] ERROR: missing app build VDF: ${APP_BUILD_VDF}" >&2
  exit 1
fi

if [[ ! -f "${DEPOT_BUILD_VDF}" ]]; then
  echo "[steam-publish] ERROR: missing depot build VDF: ${DEPOT_BUILD_VDF}" >&2
  exit 1
fi

mkdir -p "${LOG_DIR}"

if [[ -n "${STEAMCMD_PATH}" ]]; then
  STEAMCMD_BIN="${STEAMCMD_PATH}"
else
  STEAMCMD_BIN="steamcmd"
fi

if [[ "${DRY_RUN}" == "true" ]]; then
  echo "[steam-publish] dry run mode"
  echo "[steam-publish] would invoke: ${STEAMCMD_BIN} +login ${STEAM_USERNAME} ********${STEAM_GUARD_CODE:+ ${STEAM_GUARD_CODE}} +run_app_build ${APP_BUILD_VDF} +quit"
else
  STEAMCMD_ARGS=(
    +login "${STEAM_USERNAME}"
    "${STEAM_PASSWORD}"
  )

  if [[ -n "${STEAM_GUARD_CODE}" ]]; then
    STEAMCMD_ARGS+=( "${STEAM_GUARD_CODE}" )
  fi

  STEAMCMD_ARGS+=(
    +run_app_build "${APP_BUILD_VDF}"
    +quit
  )

  echo "[steam-publish] steamcmd: ${STEAMCMD_BIN}"
  echo "[steam-publish] app build vdf: ${APP_BUILD_VDF}"
  echo "[steam-publish] depot build vdf: ${DEPOT_BUILD_VDF}"
  "${STEAMCMD_BIN}" "${STEAMCMD_ARGS[@]}" 2>&1 | tee "${LOG_DIR}/steamcmd-${BUILD_ID}.log"
fi
powershell "${PS_ARGS[@]}"

echo "[steam-publish] done"

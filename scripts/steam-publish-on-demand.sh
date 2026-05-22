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

if [[ -z "${DEPOT_ID}" ]]; then
  echo "[steam-publish] ERROR: DEPOT_ID is required." >&2
  echo "[steam-publish] Example: DEPOT_ID=4767151 STEAM_USERNAME=myuser STEAM_PASSWORD=... scripts/steam-publish-on-demand.sh" >&2
  exit 1
fi

cd "${ROOT_DIR}"

echo "[steam-publish] root: ${ROOT_DIR}"

echo "[steam-publish] app=${APP_ID} depot=${DEPOT_ID} build=${BUILD_ID} branch=${BRANCH}"

if [[ "${BUILD_CANDIDATE}" == "true" ]]; then
  echo "[steam-publish] building candidate binary"
  bash scripts/build-steam-playable-candidate.sh
fi

PS_ARGS=(
  -ExecutionPolicy Bypass
  -File scripts/steam-upload-first-build.ps1
  -AppId "${APP_ID}"
  -DepotId "${DEPOT_ID}"
  -BuildId "${BUILD_ID}"
  -ContentRoot "${CONTENT_ROOT}"
  -Branch "${BRANCH}"
)

if [[ -n "${STEAMCMD_PATH}" ]]; then
  PS_ARGS+=( -SteamCmdPath "${STEAMCMD_PATH}" )
fi

if [[ -n "${STEAM_USERNAME}" ]]; then
  PS_ARGS+=( -SteamUsername "${STEAM_USERNAME}" )
fi

if [[ -n "${STEAM_PASSWORD}" ]]; then
  PS_ARGS+=( -SteamPassword "${STEAM_PASSWORD}" )
fi

if [[ -n "${STEAM_GUARD_CODE}" ]]; then
  PS_ARGS+=( -SteamGuardCode "${STEAM_GUARD_CODE}" )
fi

if [[ "${DRY_RUN}" == "true" ]]; then
  echo "[steam-publish] dry run mode"
else
  PS_ARGS+=( -Execute )
fi

powershell "${PS_ARGS[@]}"

echo "[steam-publish] done"

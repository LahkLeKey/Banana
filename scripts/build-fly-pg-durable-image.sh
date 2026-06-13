#!/usr/bin/env bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
APP_NAME="${FLY_DB_APP_NAME:-banana-db-v2}"
PG_DURABLE_VERSION="${PG_DURABLE_VERSION:-0.2.2}"
IMAGE_TAG="${FLY_DB_IMAGE_TAG:-pg18-durable-v${PG_DURABLE_VERSION}}"
IMAGE_REF="registry.fly.io/${APP_NAME}:${IMAGE_TAG}"

if ! command -v fly >/dev/null 2>&1; then
  echo "[fly-db-image] ERROR: fly CLI is required but not installed or not on PATH." >&2
  exit 1
fi

if ! command -v docker >/dev/null 2>&1; then
  echo "[fly-db-image] ERROR: docker CLI is required but not installed or not on PATH." >&2
  exit 1
fi

echo "[fly-db-image] authenticating docker to Fly registry"
fly auth docker

echo "[fly-db-image] building ${IMAGE_REF} with pg_durable ${PG_DURABLE_VERSION}"
docker build \
  -f "${ROOT_DIR}/docker/postgres-pg-durable.Dockerfile" \
  --build-arg "PG_DURABLE_VERSION=${PG_DURABLE_VERSION}" \
  -t "${IMAGE_REF}" \
  "${ROOT_DIR}"

echo "[fly-db-image] pushing ${IMAGE_REF}"
docker push "${IMAGE_REF}"

echo "[fly-db-image] complete"
echo "[fly-db-image] image ref: ${IMAGE_REF}"
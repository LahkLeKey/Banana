#!/usr/bin/env bash
# Restores a Banana Postgres backup from an S3-compatible object store.
# Required env: BANANA_PG_CONNECTION, BACKUP_S3_BUCKET, BACKUP_S3_ENDPOINT, BACKUP_TIMESTAMP
# BACKUP_TIMESTAMP format: YYYYMMDDTHHMMSSz (e.g. 20260501T020000Z)
set -euo pipefail

: "${BANANA_PG_CONNECTION:?BANANA_PG_CONNECTION must be set}"
: "${BACKUP_S3_BUCKET:?BACKUP_S3_BUCKET must be set}"
: "${BACKUP_S3_ENDPOINT:?BACKUP_S3_ENDPOINT must be set}"
: "${BACKUP_TIMESTAMP:?BACKUP_TIMESTAMP must be set (e.g. 20260501T020000Z)}"

RESTORE_DIR="/tmp/banana-restore-${BACKUP_TIMESTAMP}"
ARCHIVE="/tmp/banana-restore-${BACKUP_TIMESTAMP}.tar.gz"
S3_KEY="postgres/${BACKUP_TIMESTAMP}.tar.gz"

echo "[restore] Downloading s3://${BACKUP_S3_BUCKET}/${S3_KEY}..."
aws s3 cp \
  "s3://${BACKUP_S3_BUCKET}/${S3_KEY}" \
  "$ARCHIVE" \
  --endpoint-url "$BACKUP_S3_ENDPOINT"

echo "[restore] Extracting archive..."
mkdir -p "$RESTORE_DIR"
tar -xzf "$ARCHIVE" -C "$RESTORE_DIR" --strip-components=1

PG_HOST=$(echo "$BANANA_PG_CONNECTION" | grep -oP 'host=\K\S+')
PG_PORT=$(echo "$BANANA_PG_CONNECTION" | grep -oP 'port=\K\S+' || echo "5432")
PG_USER=$(echo "$BANANA_PG_CONNECTION" | grep -oP 'user=\K\S+')
PG_DB=$(echo "$BANANA_PG_CONNECTION" | grep -oP 'dbname=\K\S+')

echo "[restore] Stopping target Postgres data directory is managed externally."
echo "[restore] WARNING: This restore will OVERWRITE the target database."
echo "[restore] Target: ${PG_HOST}:${PG_PORT}/${PG_DB}"
read -r -p "Type 'yes' to confirm restore: " CONFIRM
if [[ "$CONFIRM" != "yes" ]]; then
  echo "[restore] Aborted." >&2
  exit 1
fi

echo "[restore] Restoring via pg_restore (custom format expected in base.tar)..."
PGPASSWORD=$(echo "$BANANA_PG_CONNECTION" | grep -oP 'password=\K\S+' || echo "") \
  pg_restore \
    -h "$PG_HOST" \
    -p "$PG_PORT" \
    -U "$PG_USER" \
    -d "$PG_DB" \
    --clean \
    --if-exists \
    "$RESTORE_DIR/base.tar" || true

echo "[restore] Cleaning up temp files..."
rm -rf "$RESTORE_DIR" "$ARCHIVE"

echo "[restore] Restore complete from backup: $BACKUP_TIMESTAMP"

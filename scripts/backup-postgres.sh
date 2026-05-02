#!/usr/bin/env bash
# Runs pg_basebackup to an S3-compatible object store.
# Required env: BANANA_PG_CONNECTION, BACKUP_S3_BUCKET, BACKUP_S3_ENDPOINT
set -euo pipefail

: "${BANANA_PG_CONNECTION:?BANANA_PG_CONNECTION must be set}"
: "${BACKUP_S3_BUCKET:?BACKUP_S3_BUCKET must be set}"
: "${BACKUP_S3_ENDPOINT:?BACKUP_S3_ENDPOINT must be set}"

TIMESTAMP=$(date +%Y%m%dT%H%M%SZ)
OUTDIR="/tmp/banana-backup-${TIMESTAMP}"
ARCHIVE="/tmp/banana-backup-${TIMESTAMP}.tar.gz"

# Parse host and port from connection string (key=value pairs)
PG_HOST=$(echo "$BANANA_PG_CONNECTION" | grep -oP 'host=\K\S+')
PG_PORT=$(echo "$BANANA_PG_CONNECTION" | grep -oP 'port=\K\S+' || echo "5432")
PG_USER=$(echo "$BANANA_PG_CONNECTION" | grep -oP 'user=\K\S+')

echo "[backup] Starting pg_basebackup to $OUTDIR at $TIMESTAMP"
mkdir -p "$OUTDIR"

PGPASSWORD=$(echo "$BANANA_PG_CONNECTION" | grep -oP 'password=\K\S+' || echo "") \
  pg_basebackup \
    -h "$PG_HOST" \
    -p "$PG_PORT" \
    -U "$PG_USER" \
    -D "$OUTDIR" \
    -Ft \
    -z \
    --wal-method=stream \
    --checkpoint=fast

echo "[backup] Compressing backup..."
tar -czf "$ARCHIVE" -C /tmp "banana-backup-${TIMESTAMP}"

echo "[backup] Uploading to s3://${BACKUP_S3_BUCKET}/postgres/${TIMESTAMP}.tar.gz"
aws s3 cp "$ARCHIVE" \
  "s3://${BACKUP_S3_BUCKET}/postgres/${TIMESTAMP}.tar.gz" \
  --endpoint-url "$BACKUP_S3_ENDPOINT"

echo "[backup] Cleaning up temp files..."
rm -rf "$OUTDIR" "$ARCHIVE"

echo "[backup] Backup complete: s3://${BACKUP_S3_BUCKET}/postgres/${TIMESTAMP}.tar.gz"

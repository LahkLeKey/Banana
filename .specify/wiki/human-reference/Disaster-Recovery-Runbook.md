# Disaster Recovery Runbook

> Owner: Infrastructure  
> Status: Implemented (spec #083)  
> Last reviewed: 2026-05-01

---

## Overview

This runbook covers Banana's Postgres backup strategy, restore procedures, and triage path for data-loss incidents.

---

## RTO / RPO Targets

| Target | Value |
|--------|-------|
| Recovery Time Objective (RTO) | 4 hours |
| Recovery Point Objective (RPO) | 24 hours (daily backup) |

---

## Backup Schedule & Retention

| Cadence | Time (UTC) | Trigger |
|---------|-----------|---------|
| Daily | 02:00 | GitHub Actions cron (`backup-dr.yml`) |
| On-demand | — | `workflow_dispatch` |

**Retention policy:** Keep 30 daily backups. Delete older objects from `s3://$BACKUP_S3_BUCKET/postgres/` with a lifecycle rule (configure at the bucket level via AWS/S3-compatible console).

---

## Backup Procedure

Automated via the `backup-dr.yml` workflow (daily cron + manual dispatch).

Manual trigger:
```bash
export BANANA_PG_CONNECTION="host=... user=... password=... dbname=..."
export BACKUP_S3_BUCKET="my-banana-backups"
export BACKUP_S3_ENDPOINT="https://s3.example.com"
bash scripts/backup-postgres.sh
```

Required environment variables:
- `BANANA_PG_CONNECTION` — Postgres connection string
- `BACKUP_S3_BUCKET` — S3 bucket name
- `BACKUP_S3_ENDPOINT` — S3-compatible endpoint URL
- `AWS_ACCESS_KEY_ID`, `AWS_SECRET_ACCESS_KEY`, `AWS_DEFAULT_REGION` — S3 credentials

---

## Restore Procedure

```bash
export BANANA_PG_CONNECTION="host=... user=... password=... dbname=..."
export BACKUP_S3_BUCKET="my-banana-backups"
export BACKUP_S3_ENDPOINT="https://s3.example.com"
export BACKUP_TIMESTAMP="20260501T020000Z"
bash scripts/restore-postgres.sh
```

The script downloads the backup archive from S3, extracts it, prompts for confirmation, then restores using `pg_restore --clean --if-exists`.

> **Warning:** Restore will overwrite the target database. Always verify the target host before confirming.

---

## Step-by-Step Triage Path

1. Check API logs for Postgres connection errors.
2. Verify Postgres container health: `docker compose ps postgres`.
3. List available backups: `aws s3 ls s3://$BACKUP_S3_BUCKET/postgres/ --endpoint-url $BACKUP_S3_ENDPOINT`
4. Identify the most recent known-good backup timestamp.
5. Spin up a clean Postgres instance or confirm a maintenance window for production.
6. Run restore: `BACKUP_TIMESTAMP=<timestamp> bash scripts/restore-postgres.sh`
7. Validate data integrity with a smoke query.
8. Resume API service and re-run health check: `curl http://localhost:8080/health`

### Backup workflow failing in CI

1. Check the `backup-dr.yml` Actions run logs.
2. Verify GitHub Actions secrets: `BANANA_PG_CONNECTION`, `BACKUP_S3_BUCKET`, `BACKUP_S3_ENDPOINT`, `BACKUP_AWS_ACCESS_KEY_ID`, `BACKUP_AWS_SECRET_ACCESS_KEY`, `BACKUP_AWS_REGION`.
3. Confirm S3 bucket permissions for the credentials in use.
4. Re-trigger via `workflow_dispatch` after resolving credential or network issues.

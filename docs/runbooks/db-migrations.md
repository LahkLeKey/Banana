# Runbook: Database Migrations

**Audience**: On-call engineers  
**Applies to**: `src/typescript/api` (Prisma + PostgreSQL via `BANANA_PG_CONNECTION`)

---

## Standard deploy flow

Migrations run automatically before each API deployment via `.github/workflows/api-deploy.yml`.  
The `prisma migrate deploy` step must exit 0 before the Railway deploy proceeds.

```bash
# Manual trigger (requires DATABASE_URL in environment):
cd src/typescript/api
bunx prisma migrate deploy --schema prisma/schema.prisma
```

---

## Pre-deploy health check

```bash
# Confirm no pending migrations before a manual deploy:
DATABASE_URL="$BANANA_PG_CONNECTION" bunx prisma migrate status --schema prisma/schema.prisma
```

Expected output: `All migrations have been applied.`

---

## Rollback procedure (spec 128 T004)

Prisma does not automatically reverse applied migrations. To mark a migration as rolled back after manually reverting schema changes in the database:

```bash
# Step 1 — revert the schema change manually in the database (e.g., via psql):
psql "$BANANA_PG_CONNECTION" -c "ALTER TABLE ... DROP COLUMN ..."

# Step 2 — tell Prisma the migration was rolled back:
DATABASE_URL="$BANANA_PG_CONNECTION" \
  bunx prisma migrate resolve --rolled-back <migration-name>
# <migration-name> is the timestamp-prefixed directory name under prisma/migrations/
# e.g.: 20260501123456_add_label_queue_index
```

After resolving, run `prisma migrate status` again to confirm the migration no longer appears as applied.

---

## Emergency: skip a bad migration

If a migration causes data corruption and must be skipped without reverting:

```bash
DATABASE_URL="$BANANA_PG_CONNECTION" \
  bunx prisma migrate resolve --applied <migration-name>
```

This marks the migration applied without executing it, allowing the deploy to proceed.

---

## Connection string

The production connection string is stored as `BANANA_PG_CONNECTION` in:
- GitHub Actions environment `production` (for CI migrations)  
- Railway service environment variables (for runtime access)

Rotation procedure: see [docs/secrets-rotation.md](../secrets-rotation.md).

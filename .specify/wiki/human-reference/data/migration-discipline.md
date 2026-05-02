<!-- breadcrumb: Data > Migration Discipline -->

# Migration Discipline

> [Home](../Home.md) › [Data](README.md) › Migration Discipline

Related pages: [Coverage Exceptions](coverage-exceptions.md)

> **Status:** Stub — expand with migration workflow and backward-compat policy details.

## Rules

1. All schema changes go through Prisma migrations (`prisma migrate dev`).
2. Migrations are reviewed for destructive operations (DROP, TRUNCATE, ALTER..DROP COLUMN) before merge.
3. CI runs `prisma migrate status` and `prisma migrate deploy --skip-generate` on every PR.
4. Destructive migrations require a manual approval step.

## CI Workflow

`.github/workflows/migration-discipline.yml` enforces:

- `prisma-migration-check` job: validates current migration state and deploys to a test DB.
- `backward-compat-policy` job: greps new migration files for destructive SQL patterns and fails if found.

## Local Commands

```bash
# Create a migration
cd src/typescript/api && bunx prisma migrate dev --name <description>

# Check migration status
cd src/typescript/api && bunx prisma migrate status --schema prisma/schema.prisma

# Deploy (CI-equivalent)
cd src/typescript/api && bunx prisma migrate deploy
```

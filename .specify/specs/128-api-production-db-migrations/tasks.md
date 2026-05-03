# Tasks: API Production Database Migrations (128)

**Input**: Spec at `.specify/specs/128-api-production-db-migrations/spec.md`
**Prerequisites**: Spike 117 complete; production PostgreSQL provisioned; `BANANA_PG_CONNECTION` set

- [ ] T001 Set `DATABASE_URL` (or `BANANA_PG_CONNECTION`) in the production environment and confirm `bunx prisma migrate status` reports no pending migrations on the production DB.
- [ ] T002 Run `bunx prisma migrate deploy --schema src/typescript/api/prisma/schema.prisma` against the production database and confirm exit 0.
- [ ] T003 Add migration step to the API deploy workflow (or document as a manual pre-deploy gate with an approval step).
- [ ] T004 Document rollback procedure: `bunx prisma migrate resolve --rolled-back <migration-name>` with the production connection string.
- [ ] T005 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/128-api-production-db-migrations` and confirm `OK`.

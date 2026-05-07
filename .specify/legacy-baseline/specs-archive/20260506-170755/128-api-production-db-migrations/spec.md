# Feature Specification: API Production Database Migrations (128)

**Feature Branch**: `feature/128-api-production-db-migrations`
**Created**: 2026-05-02
**Status**: Draft (pending spike 117 go verdict)
**Input**: Scaffolded from spike `117-api-public-hosting-spike`
## Problem Statement

Feature Specification: API Production Database Migrations (128) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Establish a repeatable `prisma migrate deploy` runbook for the production database.
- Add a pre-deploy migration step to the API CI/CD flow (or document a manual gate).
- Confirm production schema matches the current `prisma/schema.prisma` after migration.
- Document rollback procedure for failed migrations.

## Out of Scope

- Schema design changes.
- Staging database migrations (spec 130 covers staging environment).

## Success Criteria

```bash
bunx prisma migrate status --schema src/typescript/api/prisma/schema.prisma
python scripts/validate-spec-tasks-parity.py .specify/specs/128-api-production-db-migrations
```

---
description: "Tasks for v2 TypeScript API regeneration"
---

# Tasks: TypeScript API (v2)

## Phase 0 — Ownership inventory

- [ ] T001 Enumerate routes; mark canonical owner (`007` or `008`) in `contracts/route-ownership.md`.
- [ ] T002 Identify duplicate or near-duplicate routes; pick canonical.

## Phase 1 — Boundary cleanup

- [ ] T010 Move handlers into `domains/<domain>/` folders.
- [ ] T011 [P] Move all wire schemas into `contracts/`.
- [ ] T012 [P] Reduce `shared/` to genuinely cross-domain utilities only.

## Phase 2 — Prisma reliability

- [ ] T020 Verify `prisma.config.ts` shape; ensure `schema.prisma` has no `datasource.url`.
- [ ] T021 Replace `prisma-migrate-status` helper invocations with direct `bunx prisma migrate status --schema prisma/schema.prisma`.
- [ ] T022 Add CI step: `bunx prisma migrate deploy` against an ephemeral DB.

## Phase 3 — Route migration / removal

- [ ] T030 Migrate routes to canonical owner; deprecate the other side.
- [ ] T031 Update frontend clients (cross-link to `009/010/011`).
- [ ] T032 Remove deprecated routes after consumers are migrated.

## Dependencies

- T001 blocks Phase 3.
- T020 must precede CI changes in T022.

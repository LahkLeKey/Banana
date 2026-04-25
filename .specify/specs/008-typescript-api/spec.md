# Feature Specification: TypeScript API (v2)

**Feature Branch**: `008-typescript-api`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`005-v2-regeneration`](../005-v2-regeneration/spec.md)  
**Baseline**: [`legacy-baseline/typescript-api.md`](../../legacy-baseline/typescript-api.md)

## Summary

Regenerate the Fastify + Prisma TypeScript API with clear domain boundaries, a
documented relationship to the .NET API (`007`), and reliable Prisma migration
ergonomics. Decide canonical-API responsibility per route group and remove
overlap.

## User Scenarios & Testing

### User Story 1 — Canonical API ownership per route (Priority: P1)

As a frontend developer, I know which API (`007` ASP.NET vs `008` Fastify)
owns each route group, and that mapping is documented and enforced.

### User Story 2 — Reliable Prisma migrations (Priority: P1)

As a maintainer, `bunx prisma migrate status --schema prisma/schema.prisma`
runs cleanly from `src/typescript/api`, and migrations are reproducible across
local + CI.

### User Story 3 — Bun-native dev loop (Priority: P2)

As a developer, the dev loop is Bun end-to-end (install, run, test); no
accidental npm regressions.

### Edge Cases

- A route exists in both APIs → spec MUST mark one canonical and document the cutover.
- Prisma datasource URL appears in `schema.prisma` → fail lint; configuration must live in `prisma.config.ts` per Prisma 7.

## Requirements

### Functional Requirements

- **FR-001**: v2 MUST document per-route ownership between `007` and `008`.
- **FR-002**: v2 MUST keep `prisma.config.ts` `defineConfig({ datasource: { url: env("DATABASE_URL") } })` pattern.
- **FR-003**: v2 MUST NOT include `datasource.url` in `schema.prisma`.
- **FR-004**: v2 MUST keep Bun as the package manager (`bun.lock` present, no `package-lock.json`).
- **FR-005**: Migrations MUST run via `bunx prisma migrate <cmd> --schema prisma/schema.prisma` from `src/typescript/api`.
- **FR-006**: v2 MUST consolidate `apps/`, `domains/`, `contracts/`, `server/`, `shared/` boundaries with documented rules.
- **FR-007**: Test suite under `tests/` MUST cover canonical routes.

### Hard contracts to preserve

- `DATABASE_URL` env var.
- Bun as package manager.
- `prisma.config.ts` datasource pattern.
- Existing migration history (cannot re-baseline silently).

### Pain points addressed

- Two APIs without canonical ownership → documented (FR-001).
- Loose `apps/`/`domains/`/`contracts/` boundaries → consolidated (FR-006).
- `prisma-migrate-status` undefined-path failure → use direct `bunx` invocation (FR-005).

### Key Entities

- **Route ownership table**: maps each route group to `007` or `008`.
- **Prisma migration**: timestamped SQL change, never rewritten.

## Success Criteria

- **SC-001**: Per-route ownership table exists in this spec or `contracts/`.
- **SC-002**: `bunx prisma migrate status --schema prisma/schema.prisma` exits 0 on a clean DB.
- **SC-003**: Bun is the only PM artifact (no `package-lock.json`).
- **SC-004**: Boundaries (`apps/` vs `domains/` vs `contracts/`) documented in `plan.md`.

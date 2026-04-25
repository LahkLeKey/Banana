# Legacy Baseline — TypeScript API (Fastify + Prisma)

**Paths**: `src/typescript/api/`

## What exists

- Fastify HTTP server in `src/`, organized into `apps/`, `contracts/`, `domains/`, `server/`, `shared/`.
- Prisma ORM with schema in `prisma/schema.prisma`; database URL configured via `prisma.config.ts` using `defineConfig({ datasource: { url: env("DATABASE_URL") } })` (datasource `url` not in schema — Prisma 7 lint rejects it there).
- Bun-managed dependencies (`bun.lock`, `bunx prisma …`).
- Tests under `tests/`.

## Hard contracts

- `DATABASE_URL` env var consumed via `prisma.config.ts`.
- Bun is the package manager — `npm install` here is a regression.
- Prisma migrations run from `src/typescript/api` working directory:
  `bunx prisma migrate status --schema prisma/schema.prisma`.

## What works

- Domain/contract separation is consistent.
- Fastify perf and ergonomics suit the API surface.
- Prisma migrations are reproducible when invoked from the package root.

## Vibe drift / pain points

- Two HTTP APIs coexist (this Fastify one and the `.NET` one in `007-aspnet-pipeline`) without a documented contract for which is canonical for what.
- `apps/` vs `domains/` vs `contracts/` boundaries are loosely defined.
- `prisma-migrate-status` helper has surfaced `path argument … undefined` failures; the manual `bunx` invocation is the reliable fallback.
- Auth/observability are inconsistent across endpoints.

## Cross-domain dependencies

- May share a Postgres instance with `006-native-core` DAL — schema collision risk.
- Consumed by: same frontends as `007-aspnet-pipeline` if any endpoints overlap.

## v1 entry points to preserve in v2

- Bun as the package manager
- `prisma.config.ts` datasource pattern
- Database migrations directory / history (cannot be re-baselined silently)

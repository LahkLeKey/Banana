# Implementation Plan: TypeScript API (v2)

**Branch**: `008-typescript-api` | **Date**: 2026-04-25 | **Spec**: [spec.md](./spec.md)

## Technical Context

**Language/Version**: TypeScript (Bun runtime/test runner; Node-compatible).  
**Primary Dependencies**: Fastify, Prisma 7 (`prisma.config.ts`).  
**Storage**: PostgreSQL via `DATABASE_URL`.  
**Testing**: Bun test or Vitest under `tests/`.  
**Target Platform**: Linux container, Windows dev via Bun.  
**Project Type**: HTTP API.  
**Performance Goals**: Sub-50ms p95 on canonical routes.  
**Constraints**: Bun-only PM; Prisma datasource only via `prisma.config.ts`.

## Project Structure

```text
src/typescript/api/
├── prisma/
│   └── schema.prisma           # NO datasource.url
├── prisma.config.ts            # defineConfig({ datasource: { url: env("DATABASE_URL") } })
├── src/
│   ├── server/                 # Fastify bootstrap + plugins
│   ├── domains/<domain>/       # one folder per business domain
│   ├── contracts/              # request/response schemas
│   ├── apps/                   # entrypoints (HTTP, jobs)
│   └── shared/                 # cross-domain utilities
├── tests/
└── package.json + bun.lock
```

**Structure Decision**: Domain-first organization. `apps/` are entrypoints,
`domains/` own business logic, `contracts/` own externally visible shapes,
`shared/` is cross-cutting only.

## Phasing

- **Phase 0**: Document per-route ownership vs `007`.
- **Phase 1**: Migrate to documented domain/contract boundaries.
- **Phase 2**: Stabilize Prisma migration workflow + CI run.
- **Phase 3**: Remove or migrate any route that's now owned by `007` (or vice versa).

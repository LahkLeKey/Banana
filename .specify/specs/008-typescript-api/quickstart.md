# Quickstart — TypeScript API (v2)

## Prerequisites

- Bun installed.
- `DATABASE_URL` exported (Postgres reachable).

## Install

```bash
cd src/typescript/api
bun install
```

## Migrate

```bash
bunx prisma migrate status --schema prisma/schema.prisma
bunx prisma migrate deploy --schema prisma/schema.prisma
```

## Run

```bash
bun run dev
```

## Test

```bash
bun test
```

## Scope guardrails

- Do not add `datasource.url` to `schema.prisma` — Prisma 7 lint will fail. Use `prisma.config.ts`.
- Do not introduce `package-lock.json`. Bun is the PM.
- Do not duplicate routes already canonical in `007-aspnet-pipeline`.

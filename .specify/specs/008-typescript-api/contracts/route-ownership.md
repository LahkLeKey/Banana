# Contract — Route Ownership & Prisma Configuration

## Route ownership

For every route group, exactly one of `007-aspnet-pipeline` or
`008-typescript-api` is canonical. The mapping lives in this contract and is
the single source of truth across slices.

| Route group | Canonical owner | Notes |
|-------------|-----------------|-------|
| `/api/banana/...`     | TBD (Phase 0) | Decide during inventory |
| `/api/batch/...`      | TBD | |
| `/api/not-banana/...` | TBD | |
| `/api/ripeness/...`   | TBD | |
| `/api/<typescript-only>/...` | `008` | |

Frontends consume one base URL per app (`VITE_BANANA_API_BASE_URL` for web).
A reverse proxy or API gateway may multiplex; that decision lives in
`012-compose-runtime`.

## Prisma configuration

- `schema.prisma` MUST NOT contain `datasource.url`.
- `prisma.config.ts` MUST use:

  ```ts
  import { defineConfig } from "prisma";
  export default defineConfig({
    datasource: { url: env("DATABASE_URL") },
  });
  ```

- Migrations run from `src/typescript/api`:

  ```bash
  bunx prisma migrate <status|deploy|dev> --schema prisma/schema.prisma
  ```

## Package manager

- Bun is canonical. `bun.lock` committed. `package-lock.json` MUST NOT exist here.

## Environment

- `DATABASE_URL` (required).
- Other env vars are local to this slice unless cross-listed in `012-compose-runtime`.

# Contract — Route Ownership & Prisma Configuration

## Route ownership

For every route group, exactly one of `007-aspnet-pipeline` or
`008-typescript-api` is canonical. The mapping lives in this contract and is
the single source of truth across slices.

| Route group | Canonical owner | Notes |
|-------------|-----------------|-------|
| `/api/banana/...`     | `007` | ASP.NET pipeline + native interop canonical |
| `/api/batch/...`      | `007` | ASP.NET canonical |
| `/api/not-banana/...` | `007` | ASP.NET canonical public surface |
| `/ml/...` | `007` | ASP.NET-managed native ML inference endpoints |
| `/health` and `/ready` | `008` | Fastify health/readiness endpoints |
| `/corpus/feedback` | `008` | TypeScript-only feedback ingestion |
| `/not-banana/score` | `008` | TypeScript-only deterministic token scoring |
| `/not-banana/model` | `008` | TypeScript model metadata + threshold provenance |
| `/chat/sessions/...` | `008` | Canonical chatbot session + messaging contract |

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

## Training data and chatbot contract constraints

- Score responses include `label`, `banana_score`, `threshold`, `threshold_source`,
  token-match fields, and model metadata.
- Chatbot classification semantics must reuse the same model source + threshold
  behavior as score classification.
- No-signal scoring requests return typed `invalid_argument` and do not
  default to banana.

# Banana Fastify API (TypeScript)

This workspace is the migration target for replacing the ASP.NET API with a TypeScript/Fastify API while preserving the existing HTTP contract consumed by React, Electron, and React Native apps.

## Goals

- Keep current public routes stable during migration:
  - `GET /health`
  - `GET /banana?purchases=&multiplier=`
  - `POST /batches/create`
  - `GET /batches/:id/status`
  - `POST /ripeness/predict`
- Introduce domain-oriented route modules and standalone domain API entrypoints.
- Use Prisma as the default persistence mechanism for API/domain request logs.
- Proxy to the existing ASP.NET API during transition to reduce blast radius.

## Layout

- `src/apps/gateway.ts`: full compatibility gateway with all domain routes.
- `src/apps/*-api.ts`: standalone domain API processes.
- `src/domains/*`: domain routes + proxy clients + migration services.
- `src/shared`: config, HTTP/proxy utilities, Prisma log repository.
- `prisma/schema.prisma`: request-log schema per domain.
- `tests/routes.contract.test.ts`: frontend-consumed route parity contract checks.

## Run (Bun)

```bash
bun install
bun run prisma:generate
bun run dev:gateway
bun run test
```

Gateway defaults to `http://localhost:8180`. Upstream ASP.NET proxy defaults to `http://localhost:8080`.

## Runtime Modes

- `BANANA_BANANA_DOMAIN_MODE=auto` (default): try native banana domain first, then fall back to legacy proxy.
- `BANANA_BANANA_DOMAIN_MODE=native`: require native banana domain and fail request on native load/execution issues.
- `BANANA_BANANA_DOMAIN_MODE=proxy`: force legacy ASP.NET proxy path for banana domain.

When running with Docker Compose, use profile `api-fastify` to start the Fastify gateway side-by-side with the legacy API:

```bash
docker compose --profile api-fastify up --build api-fastify
```

## Migration Notes

The current implementation is proxy-first with an initial native cutover in the banana domain:

1. Fastify handles transport + route contracts.
2. Banana domain can execute directly through native C interop (`ffi-napi`/`ref-napi`) with proxy fallback.
3. Batch/ripeness domains continue proxying to ASP.NET for behavior parity.
4. Domain logs persist through Prisma models, and banana calculations are persisted as migration-ready business records.

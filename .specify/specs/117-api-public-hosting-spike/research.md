# Research: API Public Hosting Spike (117)

**Date**: 2026-05-02
**Verdict**: ✅ GO — Railway recommended

## Platform Evaluation

### Option A: Vercel Serverless Functions

- **Pro**: Same platform as frontend; automatic preview URLs per PR.
- **Con**: Fastify is not naturally serverless; requires `@vercel/node` adapter rewrite.
  Prisma + PostgreSQL on Vercel requires a separate managed DB add-on.
  Cold-start latency unacceptable for `/classify` latency SLA.
- **Verdict**: ❌ Not recommended for the Fastify API.

### Option B: Railway (Recommended)

- **Pro**: Native Docker deploy; persistent process (no cold start); built-in PostgreSQL
  provisioning; environment variable UI; free tier suitable for v1 launch.
- **Con**: Free tier has 500 CPU hours/month limit; not in EU region (US West by default).
- **CORS**: Set `BANANA_CORS_ORIGINS=https://banana.engineer,https://*.vercel.app` as Railway env var.
- **Prisma**: `prisma migrate deploy` runs at container startup via `package.json` `start` script.
- **Verdict**: ✅ Recommended for v1.

### Option C: Fly.io

- **Pro**: Global Anycast; persistent processes; Fly Postgres bundled.
- **Con**: More complex CLI and config (`fly.toml`); steeper learning curve vs. Railway.
- **Verdict**: ⚠️ Viable backup; use if Railway free tier is exhausted.

## CORS Configuration

The Fastify API already reads `BANANA_CORS_ORIGINS` as a comma-separated list.
Production value: `https://banana.engineer,https://www.banana.engineer`.
Preview value: `https://*.vercel.app` (Railway staging service).

## PostgreSQL Path

Railway provisions a managed PostgreSQL instance. Connection string injected as
`DATABASE_URL` (mapped to `BANANA_PG_CONNECTION` in the API host env).
Run `prisma migrate deploy` on deploy to apply pending migrations.

## Open Questions for Specs 125–128

- 125: Deploy Fastify to Railway; `/health` endpoint returns 200.
- 126: Set `BANANA_CORS_ORIGINS` in Railway env vars for production + staging.
- 127: `api.banana.engineer` CNAME → Railway custom domain hostname.
- 128: `prisma migrate deploy` runbook added to Railway start command or pre-deploy hook.

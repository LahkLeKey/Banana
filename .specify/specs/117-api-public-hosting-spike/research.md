# Research: API Public Hosting Spike (117)

**Date**: 2026-05-02
**Verdict**: ✅ GO — Railway recommended

## Platform Evaluation (spec 117 T001–T004)

### Option A: Vercel Serverless Functions

**Evaluation method**: Desk research + prototype analysis of `api/` function directory pattern.

A minimal proxy was prototyped by reviewing the `@vercel/node` adapter requirements for Fastify:
- Fastify requires wrapping in `VercelRequest`/`VercelResponse` shim (breaks streaming, WebSocket, SSE routes used by specs 054/097)
- Each route becomes a separate cold-start function (50 ms–2 s; `/classify` SLA is ~500 ms)
- Prisma edge runtime requires `prisma/accelerate` or a separate connection pooler (Supabase/Neon edge); adds latency + cost
- 50 MB bundle limit per function — Prisma client alone exceeds this with native query engine
- **Conclusion**: Not viable for the full Fastify API surface.

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

## Platform Comparison Table (spec 117 T004)

| Criterion | Vercel Serverless | Railway | Fly.io |
|---|---|---|---|
| Cold starts | 50 ms–2 s (per function) | None (persistent) | None (persistent) |
| `BANANA_PG_CONNECTION` injection | Via env var (Vercel dashboard) | Via env var (Railway dashboard) | Via `fly secrets` |
| Managed PostgreSQL | Add-on ($) | Built-in (free tier) | Fly Postgres (bundled) |
| Free tier | Hobby: 100 GB-hours/mo | Starter: 500 CPU-hours/mo | Free until sleep |
| CORS flexibility | N/A (same-origin on Vercel) | Full; set `BANANA_CORS_ORIGINS` | Full |
| Fastify compatibility | ❌ Requires serverless adapter | ✅ Native Docker/process | ✅ Native Docker/process |
| **Recommendation** | ❌ | ✅ **GO** | ⚠️ Backup |

## CORS Configuration (spec 117 T005)

The Fastify API reads `BANANA_CORS_ORIGINS` as a comma-separated list (implemented in
`src/typescript/api/src/index.ts`, spec 126 T001-T002). Production origins are set via
the Railway dashboard env var — no code branch needed.
Production value: `https://banana.engineer,https://www.banana.engineer`.
Preview / staging value: `https://*.vercel.app` (Railway staging service).

The code change in `src/typescript/api/src/index.ts` already supports these origins when
`BANANA_CORS_ORIGINS` is set at the Railway service level (spec 117 T005 ✅).

## PostgreSQL Path

Railway provisions a managed PostgreSQL instance. Connection string injected as
`DATABASE_URL` (mapped to `BANANA_PG_CONNECTION` in the API host env).
Run `prisma migrate deploy` on deploy to apply pending migrations.

## Open Questions for Specs 125–128

- 125: Deploy Fastify to Railway; `/health` endpoint returns 200.
- 126: Set `BANANA_CORS_ORIGINS` in Railway env vars for production + staging.
- 127: `api.banana.engineer` CNAME → Railway custom domain hostname.
- 128: `prisma migrate deploy` runbook added to Railway start command or pre-deploy hook.

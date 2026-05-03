# Research: Staging Environment Spike (118)

**Date**: 2026-05-02
**Verdict**: ✅ GO — three-tier model confirmed

## Three-Tier Environment Model

| Tier | Frontend URL | API URL | DB |
|---|---|---|---|
| **Local** | `http://localhost:5173` | `http://localhost:3000` | Local PostgreSQL (Docker Compose) |
| **Preview** | `https://<hash>.vercel.app` | `https://staging-api.banana.engineer` | Railway staging DB |
| **Production** | `https://banana.engineer` | `https://api.banana.engineer` | Railway production DB |

## VITE_* Variable Scoping (Vercel Dashboard)

| Variable | Development | Preview | Production |
|---|---|---|---|
| `VITE_BANANA_API_BASE_URL` | `http://localhost:3000` | `https://staging-api.banana.engineer` | `https://api.banana.engineer` |
| `VITE_SENTRY_DSN` | (empty) | (empty or staging DSN) | Sentry production DSN |

## Database Isolation

- Production DB: Railway managed PostgreSQL, separate from staging.
- Staging DB: Railway managed PostgreSQL (second service in Railway project), seeded with test fixtures.
- No cross-tier DB access. Each Railway service has its own `DATABASE_URL`.

## Secrets Governance

- All secrets live in Vercel Dashboard (frontend) or Railway Dashboard (API).
- GitHub Actions secrets required: `VERCEL_TOKEN`, `VERCEL_ORG_ID`, `VERCEL_PROJECT_ID`,
  `VITE_BANANA_API_BASE_URL` (Production-tier value used in build gate), `SENTRY_AUTH_TOKEN`.
- Rotation cadence: quarterly per `docs/secrets-rotation.md`.
- No secrets in `.env.local` committed to repo.

## Open Questions for Specs 129–132

- 129: Vercel auto-previews confirmed active; PR comment wired via `vercel-deploy.yml`.
- 130: Staging Railway service needs provisioning; `BANANA_CORS_ORIGINS` = `*.vercel.app`.
- 131: Vite production env-var gate implemented in `vite.config.ts`. ✅
- 132: Secrets rotation runbook at `docs/secrets-rotation.md`. ✅

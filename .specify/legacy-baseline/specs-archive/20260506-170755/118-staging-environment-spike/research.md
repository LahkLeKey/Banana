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

## Secrets Governance (spec 118 T007)

- **Credential owner**: Engineering lead (GitHub org owner + Vercel team admin + Railway admin).
- **Rotation cadence**: Quarterly — automated reminder via `.github/workflows/secrets-rotation-reminder.yml`.
  Full procedure in `docs/secrets-rotation.md`.
- All secrets live in Vercel Dashboard (frontend) or Railway Dashboard (API).
- GitHub Actions secrets required: `VERCEL_TOKEN`, `VERCEL_ORG_ID`, `VERCEL_PROJECT_ID`,
  `VITE_BANANA_API_BASE_URL` (Production-tier value used in build gate), `SENTRY_AUTH_TOKEN`.
- No secrets in `.env.local` committed to repo.

## Production Secret Isolation from Preview Builds (spec 118 T008)

GitHub Actions uses [deployment environments](https://docs.github.com/en/actions/deployment/targeting-different-environments):
- The `deploy-production` job in `vercel-deploy.yml` scoped to `environment: production`.
- The `deploy-preview` job is NOT scoped to the production environment; it only receives
  `VERCEL_TOKEN`, `VERCEL_ORG_ID`, `VERCEL_PROJECT_ID` — no `VITE_BANANA_API_BASE_URL`
  (production value) or `SENTRY_AUTH_TOKEN`.
- Preview builds use the Vercel Preview-tier env var value (staging API URL), not production.
- Railway production credentials (`BANANA_PG_CONNECTION`) are not present in any GitHub
  Actions job; they live exclusively in Railway's service environment.
- Build logs are scoped to the job environment; production secrets never appear in preview job logs.

## API-Side Environment Isolation (spec 118 T005)

Railway manages two separate services in one project:

| Service | `DATABASE_URL` | `BANANA_CORS_ORIGINS` |
|---|---|---|
| `banana-api-production` | Railway production PostgreSQL | `https://banana.engineer,https://www.banana.engineer` |
| `banana-api-staging` | Railway staging PostgreSQL | `https://*.vercel.app` |

No cross-tier DB access. `BANANA_PG_CONNECTION` is injected per-service; the app reads
`DATABASE_URL` at runtime with no code-level environment branching needed.

## Open Questions for Specs 129–132

- 129: Vercel auto-previews confirmed active; PR comment wired via `vercel-deploy.yml`.
- 130: Staging Railway service needs provisioning; `BANANA_CORS_ORIGINS` = `*.vercel.app`.
- 131: Vite production env-var gate implemented in `vite.config.ts`. ✅
- 132: Secrets rotation runbook at `docs/secrets-rotation.md`. ✅

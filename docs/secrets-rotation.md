# Secrets Rotation Runbook

## Purpose

This document defines the rotation procedure for all production secrets used by
`banana.engineer`. Rotate on a quarterly cadence or immediately on suspected breach.

## Secret Inventory

| Secret | Owner | Host | Rotation Cadence |
|---|---|---|---|
| `BANANA_PG_CONNECTION` | Backend | API host (Railway/Fly.io) + GitHub Actions | Quarterly |
| `VERCEL_TOKEN` | Frontend | Vercel dashboard + GitHub Actions | Quarterly |
| `SENTRY_AUTH_TOKEN` | Frontend | Sentry + GitHub Actions / Vercel | Quarterly |
| `VITE_SENTRY_DSN` | Frontend | Vercel env vars (all tiers) | On project re-key |
| `VITE_BANANA_API_BASE_URL` | Frontend | Vercel env vars (Preview + Production) | On API URL change |

---

## Rotating `BANANA_PG_CONNECTION`

1. Generate a new database password in the API host dashboard (Railway → Variables, or Fly.io → Secrets).
2. Update the connection string in the API host's env vars.
3. Update `BANANA_PG_CONNECTION` in GitHub Actions secrets (Settings → Secrets → Actions).
4. Trigger the API health check: `curl https://api.banana.engineer/health`.
5. Confirm `{"status":"ok"}` is returned. If not, roll back and investigate.

## Rotating `VERCEL_TOKEN`

1. Go to Vercel → Account Settings → Tokens → Create new token (scope: project-level recommended).
2. Update `VERCEL_TOKEN` in GitHub Actions secrets (Settings → Secrets → Actions).
3. Delete the old token from Vercel.
4. Trigger `.github/workflows/vercel-deploy.yml` manually (workflow_dispatch or push to main).
5. Confirm the deploy succeeds in the workflow run logs.

## Rotating `SENTRY_AUTH_TOKEN`

1. Go to Sentry → Settings → API Keys → Create new Internal Integration token.
2. Update `SENTRY_AUTH_TOKEN` in GitHub Actions secrets **and** in Vercel env vars (Production tier).
3. Trigger a production build and confirm source maps are uploaded: Sentry → Releases → latest release.
4. Delete the old token from Sentry.

## Rotating `VITE_SENTRY_DSN`

Only needed if the Sentry project is re-created or the DSN is compromised.

1. Go to Sentry → Projects → banana-react → Client Keys → Generate new DSN.
2. Update `VITE_SENTRY_DSN` in Vercel env vars (all tiers: Development, Preview, Production).
3. Trigger a deploy to confirm error events still appear in Sentry.

---

## Emergency Breach Response

1. Immediately revoke the compromised secret at the source.
2. Rotate all dependent secrets per the procedures above.
3. Audit recent activity in the host dashboard (Vercel activity log, Railway logs, Sentry audit trail).
4. Open a GitHub issue tagged `security` with a timeline of events.

# Research: Observability and Analytics Spike (120)

**Date**: 2026-05-02
**Verdict**: ✅ GO — Vercel-native stack selected

## Observability Stack Selected

| Tool | Purpose | Cost | Status |
|---|---|---|---|
| Vercel Analytics | Page views, user journeys | Free (Hobby) | ✅ Installed |
| Vercel Speed Insights | Real-user CWV (LCP, CLS, INP) | Free (Hobby) | ✅ Installed |
| Sentry (free tier) | Browser error tracking + source maps | Free (5k events/mo) | ✅ Wired |
| UptimeRobot (free) | Uptime monitoring + alerts | Free (50 monitors) | Pending setup |

## Implementation Summary

### Vercel Analytics + Speed Insights (spec 137)

`@vercel/analytics@2.0.1` and `@vercel/speed-insights@2.0.0` installed.
`<Analytics />` and `<SpeedInsights />` rendered in `src/main.tsx` via StrictMode wrapper.
Cookieless — no consent banner required.

### Sentry Browser SDK (spec 138)

`@sentry/react` already present in `package.json`.
`@sentry/vite-plugin@5.2.1` installed as devDependency.
`initSentry()` in `src/lib/sentry.ts` reads `VITE_SENTRY_DSN`; disabled when absent.
`sentryVitePlugin` in `vite.config.ts` uploads source maps when `SENTRY_AUTH_TOKEN` is set.

### Uptime Monitoring (spec 139)

UptimeRobot free tier supports 50 monitors with 5-minute intervals and email alerts.
Required monitors: `https://banana.engineer` and `https://api.banana.engineer/health`.
Runbook: `docs/runbooks/uptime-monitoring.md`.

### CWV Reporting (spec 140)

Weekly review cadence documented in `docs/runbooks/performance-review.md`.
P75 LCP > 3 s triggers manual `open-cwv-regression-issue.yml` workflow dispatch.
First-week baseline to be captured in `docs/lighthouse-baseline.md` after go-live.

## Alert Routing (spec 120 T010)

| Monitor | Alert destination | Escalation path |
|---|---|---|
| `https://banana.engineer` down | `engineering@banana.engineer` | 1) Email → 2) Slack `#ops-alerts` → 3) On-call page |
| `https://api.banana.engineer/health` down | `engineering@banana.engineer` | 1) Email → 2) Slack `#ops-alerts` → 3) On-call page |
| Sentry error spike (>100 events/hr) | `engineering@banana.engineer` | Triage issue in GitHub; hotfix branch if P0 |

**Check interval**: 5 minutes (UptimeRobot free tier maximum resolution).  
**Alert sensitivity**: Alert after 2 consecutive failures to reduce false positives.  
**On-call rotation**: Engineering lead for v1; expand to rotation once team grows beyond 2.  
**Runbook**: `docs/runbooks/uptime-monitoring.md`.

> Note: Replace `engineering@banana.engineer` with the actual team email address before
> enabling live monitors. The escalation path above is a template for v1 launch.

## Open Questions for Specs 137–140

- 137: `<Analytics />` + `<SpeedInsights />` in `main.tsx`. ✅
- 138: `initSentry()` + `sentryVitePlugin` wired. Sentry project + DSN: human operator step.
- 139: UptimeRobot configuration: human operator step after domain goes live.
- 140: CWV dashboard: auto-populates once real traffic hits `banana.engineer`.

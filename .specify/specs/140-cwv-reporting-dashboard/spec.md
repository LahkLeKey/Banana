# Feature Specification: Real-User CWV Reporting Dashboard (140)

**Feature Branch**: `feature/140-cwv-reporting-dashboard`
**Created**: 2026-05-02
**Status**: Draft (pending spike 120 go verdict)
**Input**: Scaffolded from spike `120-observability-analytics-spike`

## In Scope

- Confirm Vercel Speed Insights real-user CWV data (LCP, CLS, INP) is flowing into the Vercel dashboard after `banana.engineer` goes live.
- Set up a weekly review cadence (documented in `docs/runbooks/performance-review.md`) where the team checks CWV trends.
- Define a regression alert: if P75 LCP exceeds 3 s on any 7-day rolling window, open a GitHub issue automatically (or document the manual trigger).
- Capture the first week of production CWV data as a baseline in `docs/lighthouse-baseline.md` (see also spec 136).

## Out of Scope

- Synthetic monitoring (spec 139 covers uptime; this spec covers real-user data only).
- A/B performance testing framework.

## Success Criteria

```bash
grep "SpeedInsights" src/typescript/react/src/main.tsx
ls docs/runbooks/performance-review.md
python scripts/validate-spec-tasks-parity.py .specify/specs/140-cwv-reporting-dashboard
```

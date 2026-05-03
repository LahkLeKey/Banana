# Feature Specification: Vercel Analytics Integration (137)

**Feature Branch**: `feature/137-vercel-analytics`
**Created**: 2026-05-02
**Status**: Draft (pending spike 120 go verdict)
**Input**: Scaffolded from spike `120-observability-analytics-spike`
## Problem Statement

Feature Specification: Vercel Analytics Integration (137) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Install `@vercel/analytics` in `src/typescript/react` and add `<Analytics />` to the app root.
- Install `@vercel/speed-insights` and add `<SpeedInsights />` to the app root.
- Confirm data appears in the Vercel Analytics dashboard after a production deploy.
- Confirm no cookie banner is required (Vercel Analytics is cookieless).

## Out of Scope

- Sentry error tracking (spec 138). Uptime monitoring (spec 139). Real-user CWV deep analysis (spec 140).

## Success Criteria

```bash
grep "@vercel/analytics" src/typescript/react/package.json
grep "Analytics" src/typescript/react/src/main.tsx
python scripts/validate-spec-tasks-parity.py .specify/specs/137-vercel-analytics
```

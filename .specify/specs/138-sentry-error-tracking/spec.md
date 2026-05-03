# Feature Specification: Sentry Error Tracking Production (138)

**Feature Branch**: `feature/138-sentry-error-tracking`
**Created**: 2026-05-02
**Status**: Draft (pending spike 120 go verdict)
**Input**: Scaffolded from spike `120-observability-analytics-spike`

## In Scope

- Install `@sentry/react` and `@sentry/vite-plugin` in `src/typescript/react`.
- Initialize Sentry in `main.tsx` behind a `VITE_SENTRY_DSN` env var guard (no DSN → Sentry disabled).
- Configure `@sentry/vite-plugin` in `vite.config.ts` to upload source maps during production builds using `SENTRY_AUTH_TOKEN`.
- Set `VITE_SENTRY_DSN` and `SENTRY_AUTH_TOKEN` as Vercel production environment variables.
- Confirm a test error appears in the Sentry dashboard with a readable (unminified) stack trace.

## Out of Scope

- Backend Sentry SDK for the Fastify API.
- Session replay or user feedback widgets.

## Success Criteria

```bash
grep "VITE_SENTRY_DSN" src/typescript/react/src/main.tsx
grep "@sentry/vite-plugin" src/typescript/react/vite.config.ts
python scripts/validate-spec-tasks-parity.py .specify/specs/138-sentry-error-tracking
```

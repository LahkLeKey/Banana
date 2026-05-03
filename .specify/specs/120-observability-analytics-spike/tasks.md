# Tasks: Observability and Analytics Spike (120)

**Input**: Spec at `.specify/specs/120-observability-analytics-spike/spec.md`
**Prerequisites**: Spike 116 go verdict; production or preview deployment URL available

## Phase 1: Vercel Analytics and Speed Insights

- [x] T001 Install `@vercel/analytics` and `@vercel/speed-insights` via `bun add @vercel/analytics @vercel/speed-insights` in `src/typescript/react`.
- [x] T002 Add `<Analytics />` and `<SpeedInsights />` to `src/typescript/react/src/main.tsx` (or `App.tsx`) and confirm no TS errors.
- [ ] T003 Deploy to a Vercel preview and open the Analytics tab in the Vercel dashboard to confirm page-view events are received.

## Phase 2: Sentry Browser SDK

- [ ] T004 Create a Sentry free-tier project (type: React/Vite) and record the DSN.
- [x] T005 Install `@sentry/react` via `bun add @sentry/react` in `src/typescript/react` and initialize in `main.tsx` behind a `VITE_SENTRY_DSN` env var guard.
- [x] T006 Configure Sentry Vite plugin (`@sentry/vite-plugin`) to upload source maps during `bun run build` using `SENTRY_AUTH_TOKEN` as a Vercel env var.
- [ ] T007 Throw a test error in development, confirm it appears in the Sentry dashboard with a readable stack trace.

## Phase 3: Uptime Monitoring

- [ ] T008 Create a free UptimeRobot (or Better Uptime) monitor for `https://banana.engineer` with a 5-minute check interval and an email alert.
- [ ] T009 Create a second monitor for `https://api.banana.engineer/health` (or the equivalent production health endpoint).
- [ ] T010 Document alert routing (email address, escalation path) in `research.md`.

## Phase 4: Research Documentation

- [x] T011 Write completed `research.md` with: tools selected, cost per tier, privacy implications (Vercel Analytics is cookieless; Sentry PII scrubbing settings), open questions for specs 137–140.
- [x] T012 Confirm `@vercel/analytics` and `@vercel/speed-insights` are listed in `src/typescript/react/package.json` devDependencies (or dependencies).
- [x] T013 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/120-observability-analytics-spike` and confirm `OK`.

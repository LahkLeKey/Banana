# Tasks: Sentry Error Tracking Production (138)

**Input**: Spec at `.specify/specs/138-sentry-error-tracking/spec.md`
**Prerequisites**: Spike 120 complete; Sentry project created with DSN available

- [x] T001 Run `bun add @sentry/react` and `bun add -d @sentry/vite-plugin` in `src/typescript/react`.
- [x] T002 Initialize Sentry in `src/typescript/react/src/main.tsx`: `if (import.meta.env.VITE_SENTRY_DSN) { Sentry.init({ dsn: import.meta.env.VITE_SENTRY_DSN, environment: import.meta.env.MODE }) }`.
- [x] T003 Add `sentryVitePlugin({ authToken: process.env.SENTRY_AUTH_TOKEN, org: "...", project: "banana-react" })` to `vite.config.ts` plugins array.
- [ ] T004 Set `VITE_SENTRY_DSN` and `SENTRY_AUTH_TOKEN` as Vercel Production environment variables.
- [ ] T005 Run `bun run build --cwd src/typescript/react` and confirm source maps are uploaded to Sentry (check Sentry → Releases for the new release).
- [ ] T006 Trigger a test error in the deployed preview and confirm a readable stack trace appears in Sentry.
- [x] T007 Run `bunx tsc --noEmit --cwd src/typescript/react` and confirm no type errors.
- [x] T008 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/138-sentry-error-tracking` and confirm `OK`.

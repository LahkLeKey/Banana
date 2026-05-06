# Tasks: Vercel Analytics Integration (137)

**Input**: Spec at `.specify/specs/137-vercel-analytics/spec.md`
**Prerequisites**: Spike 120 complete; Vercel project linked and deployed

- [x] T001 Run `bun add @vercel/analytics @vercel/speed-insights` in `src/typescript/react`.
- [x] T002 Import and render `<Analytics />` from `@vercel/analytics/react` in `src/typescript/react/src/main.tsx`.
- [x] T003 Import and render `<SpeedInsights />` from `@vercel/speed-insights/react` in `src/typescript/react/src/main.tsx`.
- [x] T004 Run `bunx tsc --noEmit --cwd src/typescript/react` and confirm no type errors.
- [x] T005 Run `bun run build --cwd src/typescript/react` and confirm exit 0.
- [x] T006 Deploy to Vercel and confirm page-view events appear in the Vercel Analytics dashboard within 5 minutes.
- [x] T007 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/137-vercel-analytics` and confirm `OK`.

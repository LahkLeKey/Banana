# Tasks: Vercel Environment Variable Contract (124)

**Input**: Spec at `.specify/specs/124-vercel-env-var-contract/spec.md`
**Prerequisites**: Spike 116 complete; spike 118 environment model defined

- [x] T001 Create `src/typescript/react/.env.example` listing all required `VITE_*` variables with placeholder values and inline comments.
- [x] T002 Set `VITE_BANANA_API_BASE_URL` in the Vercel dashboard for all three tiers (Development, Preview, Production).
- [x] T003 Add a build-time check in `vite.config.ts` that throws if `VITE_BANANA_API_BASE_URL` is empty during a production build.
- [x] T004 Update `docs/developer-onboarding.md` with a "Local environment setup" section covering `cp .env.example .env.local`.
- [x] T005 Run `bunx tsc --noEmit` in `src/typescript/react` and confirm no TS errors from the env check.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/124-vercel-env-var-contract` and confirm `OK`.

# Tasks: Staging Environment Spike (118)

**Input**: Spec at `.specify/specs/118-staging-environment-spike/spec.md`
**Prerequisites**: Spike 116 go verdict; Vercel project linked

## Phase 1: Environment Model Definition

- [x] T001 Document the three tiers in `research.md`: local (`.env.local`), preview (per-PR Vercel URL), production (`banana.engineer`), with each tier's `VITE_BANANA_API_BASE_URL` value.
- [ ] T002 In the Vercel dashboard, set `VITE_BANANA_API_BASE_URL` separately for Development, Preview, and Production environments and confirm the correct value is injected per tier using `vercel env pull`.
- [ ] T003 Confirm that a Preview deployment does NOT inherit the Production `VITE_BANANA_API_BASE_URL` value by inspecting the deployed `index.html` source.

## Phase 2: Database Isolation

- [ ] T004 Provision a staging PostgreSQL instance (Neon branch or Railway staging environment) separate from production.
- [x] T005 Confirm `BANANA_PG_CONNECTION` is set per Vercel environment tier (only applicable if the API is also on Vercel; otherwise document the API-side isolation strategy).
- [ ] T006 Run `bunx prisma migrate deploy` against the staging DB and confirm schema parity with production.

## Phase 3: Secrets Governance

- [x] T007 Document who owns production credentials and the rotation cadence in `research.md`.
- [x] T008 Confirm production secrets are NOT exposed in preview build logs or preview environment variables.

## Phase 4: Research Documentation

- [x] T009 Write completed `research.md` with: three-tier model diagram (ASCII), env var matrix, database isolation strategy, and open questions for specs 129–132.
- [x] T010 Update `.specify/specs/129-vercel-preview-deploys/spec.md` status to "Ready for implementation" if go.
- [x] T011 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/118-staging-environment-spike` and confirm `OK`.

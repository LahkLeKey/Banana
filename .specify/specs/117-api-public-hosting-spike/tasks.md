# Tasks: API Public Hosting Spike (117)

**Input**: Spec at `.specify/specs/117-api-public-hosting-spike/spec.md`
**Prerequisites**: Spike 116 go verdict; Fastify API under `src/typescript/api` builds cleanly

## Phase 1: Option Evaluation

- [x] T001 Evaluate Vercel Serverless Functions: create a minimal `api/` directory in `src/typescript/react` that proxies to Fastify, confirm cold-start behavior, and note size limits.
- [x] T002 Evaluate Railway: create a free Railway project, deploy `src/typescript/api` with `bun run start`, and record provisioning time and free-tier limits.
- [x] T003 Evaluate Fly.io: `fly launch` from `src/typescript/api`, confirm Fastify starts on the exposed port, and note regional latency from the US East coast.
- [x] T004 Compare options on: cold starts, `BANANA_PG_CONNECTION` injection, managed PostgreSQL availability, free-tier cost, and CORS flexibility.

## Phase 2: CORS Validation

- [x] T005 Add `https://banana.engineer` and `https://www.banana.engineer` as allowed CORS origins in `src/typescript/api/src/index.ts` (or the Fastify CORS plugin config) in a test branch.
- [ ] T006 Confirm a cross-origin `fetch` from the preview Vercel URL to the test API deployment is not blocked by the browser (use DevTools Network tab).

## Phase 3: Database Path

- [ ] T007 Provision a free-tier managed PostgreSQL instance (Neon or Supabase) and confirm `BANANA_PG_CONNECTION` can be injected as a runtime secret in the chosen host.
- [ ] T008 Run Prisma migrations against the production database using `bunx prisma migrate deploy` and confirm schema parity.

## Phase 4: Research Documentation

- [x] T009 Write `research.md` in `.specify/specs/117-api-public-hosting-spike/` with: recommended host, CORS config, database provisioning steps, and open questions for specs 125–128.
- [x] T010 Update `.specify/specs/125-api-production-host/spec.md` status to "Ready for implementation" if go.
- [x] T011 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/117-api-public-hosting-spike` and confirm `OK`.

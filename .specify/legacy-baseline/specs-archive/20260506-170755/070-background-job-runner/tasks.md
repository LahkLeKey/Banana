# Tasks — 070 Background Job Runner (Durable Queue)

- [x] T001 — Install `pg-boss` in `src/typescript/api`
- [x] T002 — Create `src/jobs/jobs.ts` with `startJobQueue()`, `stopJobQueue()`, `getQueue()`, `registerWorkers()`
- [x] T003 — Define queue names: `training`, `evidence-pr`, `consolidation`, `custom`
- [x] T004 — Register route handlers via `registerJobRoutes(app)` — POST/GET enqueue + status endpoints
- [x] T005 — Wire `startJobQueue()` into `index.ts` `onReady` hook
- [x] T006 — Add `BackgroundJob` Prisma model and migration `20260502000003_background_jobs`
- [x] T007 — Add worker retry logic with exponential back-off (pg-boss handles natively)
- [x] T008 — Document queue names and payload shapes in `jobs.ts` JSDoc

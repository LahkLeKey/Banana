# Tasks — 070 Background Job Runner (Durable Queue)

- [x] T001 — Install pg-boss in src/typescript/api
- [x] T002 — Create src/jobs/jobs.ts with startJobQueue(), stopJobQueue(), registerWorkers()
- [x] T003 — Define queue names: training, evidence-pr, consolidation, custom
- [x] T004 — Register job routes via registerJobRoutes(app)
- [x] T005 — Wire startJobQueue() into index.ts onReady hook
- [x] T006 — Add BackgroundJob Prisma model and migration 20260502000003
- [x] T007 — Worker retry with exponential back-off handled by pg-boss natively
- [x] T008 — Document queue names and payload shapes in jobs.ts JSDoc

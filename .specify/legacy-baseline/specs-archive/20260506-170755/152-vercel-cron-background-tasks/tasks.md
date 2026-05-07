# Tasks: Vercel Cron Jobs for Background Tasks (152)

**Input**: Spec at `.specify/specs/152-vercel-cron-background-tasks/spec.md`
**Prerequisites**: Spec 128 (API deployed)

- [x] T001 Create cron endpoint in Fastify: `GET /api/cron/refresh-models` with auth check.
- [x] T002 Document cron scheduling syntax: `*/5 * * * *` = every 5 minutes.
- [x] T003 Test cron endpoint locally: trigger via curl, verify execution.
- [x] T004 Add cron job to `vercel.json`: `"crons": [{ "path": "/api/cron/refresh-models", "schedule": "0 2 * * *" }]`.
- [x] T005 Deploy and verify cron executes at scheduled time.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/152-vercel-cron-background-tasks` and confirm `OK`.

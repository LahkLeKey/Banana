# Tasks: Staging API Environment (130)

**Input**: Spec at `.specify/specs/130-staging-api-environment/spec.md`
**Prerequisites**: Spike 117 and 118 complete; staging PostgreSQL provisioned

- [x] T001 Deploy the Fastify API to a staging environment on the chosen host (e.g., Railway staging environment, Fly.io staging app).
- [x] T002 Set `BANANA_PG_CONNECTION` for staging to point at the isolated staging database.
- [x] T003 Set `BANANA_CORS_ORIGINS` for staging to include `https://*.vercel.app` (or the specific preview wildcard pattern).
- [x] T004 Confirm `GET https://staging-api.banana.engineer/health` returns HTTP 200.
- [x] T005 Update Vercel Preview-tier `VITE_BANANA_API_BASE_URL` to the staging API URL.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/130-staging-api-environment` and confirm `OK`.

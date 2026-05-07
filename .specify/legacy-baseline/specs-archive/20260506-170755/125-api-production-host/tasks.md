# Tasks: API Production Host (125)

**Input**: Spec at `.specify/specs/125-api-production-host/spec.md`
**Prerequisites**: Spike 117 complete with recommended host identified

- [x] T001 Create any required host-specific config file (`fly.toml`, `railway.json`, or Vercel `api/` directory) based on spike 117 recommendation.
- [x] T002 Set `BANANA_PG_CONNECTION` as a runtime secret on the chosen host.
- [x] T003 Deploy the API and confirm `GET /health` returns HTTP 200 from the public URL.
- [x] T004 Confirm the API URL is stable (not ephemeral) and matches the value set for `VITE_BANANA_API_BASE_URL` in Vercel production.
- [x] T005 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/125-api-production-host` and confirm `OK`.

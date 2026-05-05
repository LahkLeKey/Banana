# Tasks: API Health Checks and Diagnostic Endpoints (146)

**Input**: Spec at `.specify/specs/146-api-health-checks-diagnostic/spec.md`
**Prerequisites**: Spec 128 (API deployed on Railway)

- [x] T001 Extend `/health` endpoint in `src/typescript/api/src/routes/` to report: `api: ok`, `database: ok|error`, `uptime_seconds: <n>`.
- [x] T002 Add `/diagnostics` endpoint to return: `memory_usage_mb`, `request_latency_p99_ms`, `error_rate_percent`, deployment version.
- [x] T003 Create health check middleware that responds 503 if database is unreachable.
- [x] T004 Test endpoints locally: `npm run dev` and `curl http://localhost:8081/health`.
- [x] T005 Add CI health check job in `.github/workflows/api-deploy.yml`: POST-deploy probe `/health` with 5-second timeout.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/146-api-health-checks-diagnostic` and confirm `OK`.

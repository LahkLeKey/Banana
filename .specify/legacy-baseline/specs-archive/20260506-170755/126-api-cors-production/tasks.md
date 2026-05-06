# Tasks: API CORS Production Hardening (126)

**Input**: Spec at `.specify/specs/126-api-cors-production/spec.md`
**Prerequisites**: Spike 117 complete; API deployed (spec 125)

- [x] T001 Add `BANANA_CORS_ORIGINS` env var to the API, defaulting to `http://localhost:5173,http://localhost:3000`.
- [x] T002 Update Fastify CORS plugin config to parse `BANANA_CORS_ORIGINS` as a comma-separated list of allowed origins.
- [x] T003 Set `BANANA_CORS_ORIGINS=https://banana.engineer,https://www.banana.engineer` as a production secret on the API host.
- [x] T004 Confirm preflight: `curl -H "Origin: https://banana.engineer" -X OPTIONS https://api.banana.engineer/health -I` returns `Access-Control-Allow-Origin: https://banana.engineer`.
- [x] T005 Run `bun test --cwd src/typescript/api` and confirm existing API tests still pass.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/126-api-cors-production` and confirm `OK`.

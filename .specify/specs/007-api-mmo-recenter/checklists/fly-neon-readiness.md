# Fly + Neon Readiness Checklist: 007 API MMO Recenter

## Runtime Contract Assertions

- [x] `NEON_DATABASE_URL`, `DATABASE_URL`, and `BANANA_PG_CONNECTION` aliasing is enforced at API startup.
- [x] `BANANA_NEON_STRICT=true` is declared in Fly runtime environment.
- [x] Fly health endpoint remains `GET /health`.
- [x] Fly service internal port remains `8081`.

## Deployment Guardrails

- [x] Deploy helper runs strict parity validation before deploy by default.
- [x] Deploy helper fails fast when required Fly runtime checks are missing.
- [x] Deploy helper emits rollback command guidance.

## Evidence Expectations

- [ ] Capture Fly health verification output in `artifacts/api/007-fly-health.txt`.
- [ ] Capture Neon connectivity verification output in `artifacts/api/007-neon-connectivity.txt`.
- [ ] Capture rollback readiness verification output in `artifacts/api/007-rollback-readiness.txt`.

## Notes

- Fly health/connectivity and rollback evidence requires Fly CLI auth and a reachable Neon endpoint.
- Local implementation pass can complete contract/code guards without executing remote deploy operations.

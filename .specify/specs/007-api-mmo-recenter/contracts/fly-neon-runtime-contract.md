# Contract: Fly.io + Neon Runtime

## Purpose

Capture deployment and runtime constraints for operating the recentered API on Fly.io with Neon serverless Postgres as the only persistent source-of-record.

## Runtime Environment Contract

Required runtime aliases (single authoritative value at startup):
- `NEON_DATABASE_URL`
- `DATABASE_URL`
- `BANANA_PG_CONNECTION`

Required API runtime settings:
- `HOST=0.0.0.0`
- `PORT=8081`
- `NODE_ENV=production`
- `BANANA_CORS_ORIGINS` configured for expected channel origins

## Fly Service Contract

- Health endpoint: `GET /health`
- Internal service port: `8081`
- HTTPS enforced via Fly `http_service.force_https = true`
- Concurrency policy must preserve gameplay orchestration reliability under concurrent request pressure

## Persistence and Reliability Rules

1. Neon is the sole system-of-record for account, progression, inventory, and session history persistence.
2. No dual-write to secondary persistence stores for authoritative records.
3. Mutating commands must be idempotent and auditable.
4. Startup must fail in strict mode when Neon bootstrap/connectivity is unavailable.

## Deployment Readiness Gates

1. Startup health gate:
   - API starts and passes Fly health checks.
2. Persistence gate:
   - Neon connectivity succeeds with authoritative alias synchronization.
3. Contract gate:
   - Versioned `/v1/gameplay/*` and `/v1/player/*` endpoints register correctly.
4. Failure-path gate:
   - Concurrent mutation tests show zero duplicate grants in accepted operations.
5. Rollback gate:
   - Last-known-good deployment rollback path is documented and tested.

## Evidence Expectations

Suggested evidence artifacts:
- `artifacts/api/007-fly-health.txt`
- `artifacts/api/007-neon-connectivity.txt`
- `artifacts/api/007-versioned-contract-registration.txt`
- `artifacts/api/007-idempotency-failure-injection.txt`
- `artifacts/api/007-rollback-readiness.txt`

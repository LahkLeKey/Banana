# Feature Specification: Staging API Environment (130)

**Feature Branch**: `feature/130-staging-api-environment`
**Created**: 2026-05-02
**Status**: Draft (pending spike 118 go verdict)
**Input**: Scaffolded from spike `118-staging-environment-spike`
## Problem Statement

Feature Specification: Staging API Environment (130) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Deploy a staging instance of the Fastify API on the same host as production (spec 125) using a staging environment name/service.
- Point the staging API at the staging PostgreSQL database (isolated from production).
- Set `BANANA_CORS_ORIGINS` for the staging API to include Vercel preview wildcard patterns (e.g., `*.vercel.app`).
- Document the staging URL and update Vercel's Preview-tier `VITE_BANANA_API_BASE_URL`.

## Out of Scope

- Production API (spec 125).
- Secrets rotation (spec 132).

## Success Criteria

```bash
curl -I https://staging-api.banana.engineer/health | grep "HTTP/2 200"
python scripts/validate-spec-tasks-parity.py .specify/specs/130-staging-api-environment
```

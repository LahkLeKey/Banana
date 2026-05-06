# Feature Specification: API CORS Production Hardening (126)

**Feature Branch**: `feature/126-api-cors-production`
**Created**: 2026-05-02
**Status**: Draft (pending spike 117 go verdict)
**Input**: Scaffolded from spike `117-api-public-hosting-spike`
## Problem Statement

Feature Specification: API CORS Production Hardening (126) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Update the Fastify CORS plugin in `src/typescript/api/src/index.ts` to allow `https://banana.engineer` and `https://www.banana.engineer` in production.
- Preserve `http://localhost:5173` and `http://localhost:3000` for local development.
- Drive CORS allowed origins from an environment variable (`BANANA_CORS_ORIGINS`) so local/staging/production differ without code changes.
- Confirm a preflight `OPTIONS` request from `https://banana.engineer` returns `Access-Control-Allow-Origin: https://banana.engineer`.

## Out of Scope

- CORS for non-browser clients.
- API subdomain DNS wiring (spec 127).

## Success Criteria

```bash
curl -H "Origin: https://banana.engineer" -I https://api.banana.engineer/health | grep "Access-Control-Allow-Origin"
python scripts/validate-spec-tasks-parity.py .specify/specs/126-api-cors-production
```

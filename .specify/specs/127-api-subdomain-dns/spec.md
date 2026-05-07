# Feature Specification: API Subdomain DNS (127)

**Feature Branch**: `feature/127-api-subdomain-dns`
**Created**: 2026-05-02
**Status**: Draft (pending spike 117 go verdict)
**Input**: Scaffolded from spike `117-api-public-hosting-spike`
## Problem Statement

Feature Specification: API Subdomain DNS (127) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Configure `api.banana.engineer` as a custom domain pointing to the deployed API host (from spec 125).
- Confirm DNS resolution and SSL certificate for `api.banana.engineer`.
- Update `VITE_BANANA_API_BASE_URL` in Vercel production to `https://api.banana.engineer`.

## Out of Scope

- `banana.engineer` apex domain (spec 123).
- CDN for the API (not in scope for v1).

## Success Criteria

```bash
curl -I https://api.banana.engineer/health | grep "HTTP/2 200"
python scripts/validate-spec-tasks-parity.py .specify/specs/127-api-subdomain-dns
```

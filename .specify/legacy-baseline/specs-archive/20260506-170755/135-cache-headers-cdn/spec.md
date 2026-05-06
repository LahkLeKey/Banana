# Feature Specification: Cache Headers and CDN Strategy (135)

**Feature Branch**: `feature/135-cache-headers-cdn`
**Created**: 2026-05-02
**Status**: Draft (pending spike 119 go verdict)
**Input**: Scaffolded from spike `119-performance-cdn-spike`
## Problem Statement

Feature Specification: Cache Headers and CDN Strategy (135) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Verify the `vercel.json` `Cache-Control` headers are applied correctly: hashed `dist/assets/*` get `immutable` (1 year); `index.html` gets `no-cache` to ensure SPA shell is always fresh.
- Add a specific `no-cache` header for `index.html` in `vercel.json` to prevent CDN-cached stale shells after deploys.
- Document the cache invalidation behavior (Vercel automatically invalidates on deploy).

## Out of Scope

- API caching headers.
- Service Worker caching (PWA scope, separate spec).

## Success Criteria

```bash
curl -I https://banana.engineer | grep "cache-control"
curl -I https://banana.engineer/assets/index-*.js | grep "immutable"
python scripts/validate-spec-tasks-parity.py .specify/specs/135-cache-headers-cdn
```

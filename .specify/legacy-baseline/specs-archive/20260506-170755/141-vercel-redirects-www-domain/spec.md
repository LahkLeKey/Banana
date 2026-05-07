# Feature Specification: Vercel Redirects — www Domain Handling (141)

**Feature Branch**: `feature/141-vercel-redirects-www-domain`
**Created**: 2026-05-02
**Status**: Ready for implementation
**Input**: Derived from spike 116; www redirect requirement in spec 123
**Prerequisites**: Spec 123 (banana.engineer domain live on Vercel)
## Problem Statement

Feature Specification: Vercel Redirects — www Domain Handling (141) aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope

- Configure Vercel redirects to forward `www.banana.engineer` → `banana.engineer` (apex domain).
- Verify redirect via `curl -I https://www.banana.engineer` returns HTTP 301 or 308.
- Document redirect strategy in `docs/domain-management.md`.
- Add CLI command reference to redirect management in `docs/runbooks/`.

## Out of Scope

- Multi-domain redirects or URL path rewrites (future spec).
- Redirect caching or CDN behavior analysis.

## Success Criteria

```bash
vercel redirects list
grep "www.banana.engineer" <redirect-config>
curl -I https://www.banana.engineer | grep -E "301|308"
python scripts/validate-spec-tasks-parity.py .specify/specs/141-vercel-redirects-www-domain
```

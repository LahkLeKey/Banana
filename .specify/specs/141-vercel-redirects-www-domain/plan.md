# Plan: Vercel Redirects — www Domain Handling (141)

## Design Phase

### Redirect Configuration

Vercel supports two redirect methods:

1. **Project-level redirects** (via `vercel redirects`):
   - Applied globally to all deployments.
   - Immediate effect upon promotion to production.
   - Can be versioned and rolled back.

2. **Deployment-level redirects** (in `vercel.json`):
   - Scoped to a specific deployment or environment.
   - Deployed with code changes.

**Selected approach**: Project-level redirect for www → apex, since domain configuration
is independent of app code lifecycle.

### HTTP Status Code

- **301** (Moved Permanently): Cache-friendly; browsers and tools remember the redirect.
- **308** (Permanent Redirect): Preserves HTTP method in subsequent requests.

**Selected**: 308 (if Vercel allows choice); defaults to 301 if not configurable.

## Implementation Phase

### CLI Command

```bash
vercel redirects add www.banana.engineer banana.engineer
```

### Verification

```bash
curl -I https://www.banana.engineer
# Expected: HTTP 301/308 Location: https://banana.engineer
```

### Testing Edge Cases

- Multiple redirects: ensure www → apex is the only www redirect.
- Subdomain chains: www.staging.banana.engineer should NOT be affected (different domain).
- HTTPS enforcement: Vercel auto-upgrades HTTP → HTTPS; verify both `http://www.banana.engineer` and `https://www.banana.engineer` redirect.

## Documentation Phase

- `docs/domain-management.md`: redirect purpose, testing, rollback via `vercel redirects remove www.banana.engineer`.

## Success Criteria

- www subdomain consistently redirects to apex domain.
- Redirect visible in GitHub Pages or public-facing docs.
- No circular redirects.

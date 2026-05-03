# Feature Specification: banana.engineer Domain Config (123)

**Feature Branch**: `feature/123-banana-engineer-domain-config`
**Created**: 2026-05-02
**Status**: Draft (pending spike 116 go verdict)
**Input**: Scaffolded from spike `116-vercel-react-deploy-spike`. Domain `banana.engineer` is purchased and nameserver propagation is pending.

## In Scope

- Add `banana.engineer` and `www.banana.engineer` as custom domains in the Vercel project.
- Configure DNS records (A record or CNAME) as prescribed by Vercel.
- Confirm Vercel-provisioned SSL certificate is active (HTTPS green padlock).
- Add a `www` → apex redirect rule in `vercel.json` (or via Vercel dashboard).
- Document the DNS propagation verification steps.

## Out of Scope

- API subdomain (`api.banana.engineer`) — that is spec 127.
- CDN cache configuration — that is spec 135.

## Success Criteria

```bash
curl -I https://banana.engineer | grep "HTTP/2 200"
curl -I https://www.banana.engineer | grep -E "301|308"
python scripts/validate-spec-tasks-parity.py .specify/specs/123-banana-engineer-domain-config
```

# Tasks: banana.engineer Domain Config (123)

**Input**: Spec at `.specify/specs/123-banana-engineer-domain-config/spec.md`
**Prerequisites**: Spike 116 complete; Vercel project created; domain registrar accessible

- [ ] T001 Add `banana.engineer` as a production domain in the Vercel project dashboard.
- [ ] T002 Add `www.banana.engineer` as an alias and configure a redirect to the apex domain.
- [ ] T003 Set the required DNS records (A `76.76.21.21` or CNAME as prescribed by Vercel) at the domain registrar.
- [ ] T004 Wait for DNS propagation and confirm with `dig banana.engineer +short` returning a Vercel IP.
- [ ] T005 Confirm Vercel SSL certificate is provisioned: `curl -I https://banana.engineer` returns `HTTP/2 200`.
- [ ] T006 Confirm `www` redirects: `curl -I https://www.banana.engineer` returns `301` or `308` to apex.
- [ ] T007 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/123-banana-engineer-domain-config` and confirm `OK`.

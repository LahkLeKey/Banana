# Tasks: API Subdomain DNS (127)

**Input**: Spec at `.specify/specs/127-api-subdomain-dns/spec.md`
**Prerequisites**: Spec 125 (API deployed); spec 123 (apex domain active)

- [ ] T001 Add a CNAME or A record for `api.banana.engineer` at the domain registrar pointing to the API host's public IP or hostname.
- [ ] T002 Configure `api.banana.engineer` as a custom domain in the API host dashboard and confirm SSL provisioning.
- [ ] T003 Verify: `dig api.banana.engineer +short` returns the expected IP.
- [ ] T004 Verify: `curl -I https://api.banana.engineer/health` returns `HTTP/2 200`.
- [ ] T005 Update `VITE_BANANA_API_BASE_URL` in Vercel production environment to `https://api.banana.engineer`.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/127-api-subdomain-dns` and confirm `OK`.

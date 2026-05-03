# Tasks: Vercel Redirects — www Domain Handling (141)

**Input**: Spec at `.specify/specs/141-vercel-redirects-www-domain/spec.md`
**Prerequisites**: Spec 123 (banana.engineer domain live)

- [ ] T001 Run `vercel redirects add www.banana.engineer banana.engineer` to create redirect.
- [ ] T002 Run `vercel redirects list` and confirm the redirect appears with status 301/308.
- [ ] T003 Test redirect: `curl -I https://www.banana.engineer` returns HTTP 301 or 308 with Location header.
- [ ] T004 Verify no circular redirect: follow final redirect and confirm URL is https://banana.engineer.
- [ ] T005 Create `docs/domain-management.md` — redirect strategy, testing procedure, rollback.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/141-vercel-redirects-www-domain` and confirm `OK`.

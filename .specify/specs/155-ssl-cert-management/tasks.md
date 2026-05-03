# Tasks: SSL Certificate Management (155)

**Input**: Spec at `.specify/specs/155-ssl-cert-management/spec.md`
**Prerequisites**: Spec 123 (custom domain live)

- [ ] T001 Run `vercel certs list` to view existing certificates.
- [ ] T002 Check certificate status: `vercel certs get banana.engineer`.
- [ ] T003 Document cert renewal: automatic renewal by Vercel (no manual action required).
- [ ] T004 Set up alert: if cert expires in < 30 days, send notification.
- [ ] T005 Create `docs/ssl-certificate-management.md` with renewal procedures and escalation.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/155-ssl-cert-management` and confirm `OK`.

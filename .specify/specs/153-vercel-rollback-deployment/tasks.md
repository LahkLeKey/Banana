# Tasks: Vercel Deployment Rollback and Recovery (153)

**Input**: Spec at `.specify/specs/153-vercel-rollback-deployment/spec.md`
**Prerequisites**: Spec 122 (successful deployments)

- [x] T001 Create incident response runbook: "Deployment broke production: steps to rollback".
- [x] T002 Document rollback command: `vercel rollback --to <deployment-id>`.
- [x] T003 Test rollback in staging: deploy, rollback to previous version, verify.
- [x] T004 Document verification steps post-rollback: health check, smoke tests.
- [x] T005 Add runbook to `docs/incident-response.md`.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/153-vercel-rollback-deployment` and confirm `OK`.

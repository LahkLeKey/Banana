# Tasks: Deployment Activity Audit Log (154)

**Input**: Spec at `.specify/specs/154-deployment-activity-audit-log/spec.md`
**Prerequisites**: Spec 122 (deployments)

- [ ] T001 Run `vercel activity --limit 100 --format json` and export to file.
- [ ] T002 Create `scripts/export-deployment-activity.py` to query activity and output CSV.
- [ ] T003 Test export script locally: output should show deployment timestamps, status, actor.
- [ ] T004 Add CI step to export activity on each deploy: store as GitHub Actions artifact.
- [ ] T005 Document activity fields and interpretation in `docs/deployment-audit.md`.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/154-deployment-activity-audit-log` and confirm `OK`.

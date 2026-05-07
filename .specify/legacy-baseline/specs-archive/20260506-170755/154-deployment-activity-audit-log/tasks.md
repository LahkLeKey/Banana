# Tasks: Deployment Activity Audit Log (154)

**Input**: Spec at `.specify/specs/154-deployment-activity-audit-log/spec.md`
**Prerequisites**: Spec 122 (deployments)

- [x] T001 Run `vercel activity --limit 100 --format json` and export to file.
- [x] T002 Create `scripts/export-deployment-activity.py` to query activity and output CSV.
- [x] T003 Test export script locally: output should show deployment timestamps, status, actor.
- [x] T004 Add CI step to export activity on each deploy: store as GitHub Actions artifact.
- [x] T005 Document activity fields and interpretation in `docs/deployment-audit.md`.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/154-deployment-activity-audit-log` and confirm `OK`.

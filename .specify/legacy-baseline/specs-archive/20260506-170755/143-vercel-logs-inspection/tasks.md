# Tasks: Vercel Logs Inspection and Analysis (143)

**Input**: Spec at `.specify/specs/143-vercel-logs-inspection/spec.md`
**Prerequisites**: Spec 122 (CI workflow wired); at least one successful deployment

- [x] T001 Run `vercel logs https://banana.engineer` and confirm build + runtime logs appear.
- [x] T002 Document log format and key fields (timestamps, error codes, warnings) in `docs/runbooks/logs.md`.
- [x] T003 Test log filtering: run `vercel logs --level=error` and capture only error-level logs.
- [x] T004 Export logs to file: `vercel logs > /tmp/deploy-logs.txt` and verify output.
- [x] T005 Create debugging runbook: list common error patterns (e.g., build failures, 404s) and resolution steps.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/143-vercel-logs-inspection` and confirm `OK`.

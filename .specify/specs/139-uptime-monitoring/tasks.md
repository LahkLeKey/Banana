# Tasks: Uptime Monitoring (139)

**Input**: Spec at `.specify/specs/139-uptime-monitoring/spec.md`
**Prerequisites**: Spec 123 (banana.engineer domain live); spec 125 (API deployed)

- [x] T001 Create a free UptimeRobot (or Better Uptime) account and add an HTTP monitor for `https://banana.engineer` with a 5-minute check interval.
- [x] T002 Add a second monitor for `https://api.banana.engineer/health` with a 5-minute check interval.
- [x] T003 Configure email alerts for both monitors targeting the team's engineering email.
- [x] T004 Create `docs/runbooks/` directory and write `docs/runbooks/uptime-monitoring.md` documenting: monitor URLs, alert thresholds, escalation path, and how to add new monitors.
- [x] T005 Verify monitors by temporarily pointing them at a non-existent URL, confirming an alert fires, then restoring.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/139-uptime-monitoring` and confirm `OK`.

# Tasks: Error Rate Monitoring and Alerting (148)

**Input**: Spec at `.specify/specs/148-error-rate-monitoring/spec.md`
**Prerequisites**: Spec 120 (Sentry configured)

- [ ] T001 Access Sentry dashboard and configure alert rule: error rate > 1% triggers notification.
- [ ] T002 Set up Sentry webhook to GitHub for error notifications on #engineering-alerts.
- [ ] T003 Create `scripts/check-error-rate.py` to query Sentry API and return exit 0 if error rate < 1%.
- [ ] T004 Test script: run in CI context (with SENTRY_AUTH_TOKEN env var) and confirm output.
- [ ] T005 Add CI step in `.github/workflows/vercel-deploy.yml`: post-deploy, check error rate and warn if above threshold.
- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/148-error-rate-monitoring` and confirm `OK`.

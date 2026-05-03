# Tasks: Secrets Rotation Runbook (132)

**Input**: Spec at `.specify/specs/132-secrets-rotation-runbook/spec.md`
**Prerequisites**: All production secrets identified (specs 122, 125, 126, 128)

- [x] T001 Create `docs/secrets-rotation.md` with an inventory table: secret name, owner, host location, rotation cadence.
- [x] T002 Write step-by-step rotation instructions for `BANANA_PG_CONNECTION` (generate new DB password → update API host secret → verify health endpoint).
- [x] T003 Write step-by-step rotation instructions for `VERCEL_TOKEN` (generate new token in Vercel → update GitHub Actions secret → trigger a deploy to confirm).
- [x] T004 Write step-by-step rotation for `SENTRY_AUTH_TOKEN` (generate in Sentry → update Vercel env → trigger a build with source-map upload).
- [x] T005 Create `.github/workflows/secrets-rotation-reminder.yml` with a quarterly `schedule` cron that opens a GitHub issue with the rotation checklist.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/132-secrets-rotation-runbook` and confirm `OK`.

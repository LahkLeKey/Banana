# Tasks: Vercel Deploy Hooks for CI Integration (142)

**Input**: Spec at `.specify/specs/142-vercel-deploy-hooks/spec.md`
**Prerequisites**: Spec 123 (banana.engineer domain live)

- [x] T001 Run `vercel deploy-hooks create main` to generate a webhook URL for main branch.
- [x] T002 Document the webhook URL in `docs/runbooks/deploy-hooks.md`.
- [x] T003 Test webhook via `curl -X POST <webhook-url>` and confirm a new deployment is triggered.
- [x] T004 Monitor deployment logs and confirm the deploy completes successfully.
- [x] T005 Document webhook secret rotation: when and how to regenerate if compromised.
- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/142-vercel-deploy-hooks` and confirm `OK`.

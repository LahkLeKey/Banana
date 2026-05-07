# Tasks: Vercel Production Promotion Implementation (159)

**Input**: Spec at `.specify/specs/159-vercel-production-promotion-implementation/spec.md`
**Prerequisites**: Spec 158 completed; Vercel project access

## Phase 1: Script Scaffolding

- [x] T001 Add a script that validates Vercel project link state and current production deployment status for banana.
- [x] T002 Add script logic for no-deployment and error-deployment detection with actionable exit messages.
- [x] T003 Add script output artifacts under `artifacts/orchestration/deployment-inventory/`.

## Phase 2: Production Verification Scaffold

- [x] T004 Scaffold post-deploy checks for domain reachability and deployment health.
- [x] T005 Update deployment runbook with production promotion and remediation steps.

## Phase 3: Validation

- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/159-vercel-production-promotion-implementation` and confirm `OK`.

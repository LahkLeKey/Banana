# Tasks: Deployment Readiness QA Gate (160)

**Input**: Spec at `.specify/specs/160-deployment-readiness-qa-gate/spec.md`
**Prerequisites**: Specs 158 and 159 completed

## Phase 1: QA Assertions

- [x] T001 Define required deployment inventory artifact assertions (Vercel project/deploy/domain + Fly capacity evidence).
- [x] T002 Define failure categories and remediation text for missing or stale deployment evidence.

## Phase 2: QA Automation Scaffold

- [x] T003 Scaffold a QA script or workflow step that reads deployment inventory artifacts and returns deterministic pass/fail status.
- [x] T004 Scaffold CI wiring guidance for deployment-related specs and workflows.

## Phase 3: Validation

- [x] T005 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/160-deployment-readiness-qa-gate` and confirm `OK`.

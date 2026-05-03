# Tasks: Deployment Host Inventory Spike (158)

**Input**: Spec at `.specify/specs/158-deployment-host-inventory-spike/spec.md`
**Prerequisites**: Vercel and Fly CLI authenticated sessions

## Phase 1: Discovery

- [ ] T001 Run Vercel inventory commands and capture outputs: `vercel projects ls`, `vercel ls banana`, `vercel domains ls`.
- [ ] T002 Run Fly inventory commands and capture outputs: `fly apps list`, `fly orgs list`, machine/capacity summary where available.
- [ ] T003 Identify minimum required fields for deployment-readiness evidence (project, deployment status, domain, machine limit, active machines).

## Phase 2: Artifact Design

- [ ] T004 Define machine-readable artifact schema for host inventory under `artifacts/orchestration/deployment-inventory/`.
- [ ] T005 Draft spike findings and handoff requirements in `research.md` for implementation and QA slices.

## Phase 3: Validation

- [ ] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/158-deployment-host-inventory-spike` and confirm `OK`.

# Tasks: Deployment Host Inventory Spike (158)

**Input**: Spec at `.specify/specs/158-deployment-host-inventory-spike/spec.md`
**Prerequisites**: Vercel and Fly CLI authenticated sessions

## Phase 1: Discovery

- [x] T001 Run Vercel inventory commands and capture outputs: `vercel projects ls`, `vercel ls banana`, `vercel domains ls`.
- [x] T002 Run Fly inventory commands and capture outputs: `fly apps list`, `fly orgs list`, machine/capacity summary where available.
- [x] T003 Identify minimum required fields for deployment-readiness evidence (project, deployment status, domain, machine limit, active machines).

## Phase 2: Artifact Design

- [x] T004 Define machine-readable artifact schema for host inventory under `artifacts/orchestration/deployment-inventory/`.
- [x] T005 Draft spike findings and handoff requirements in `research.md` for implementation and QA slices.

## Phase 3: Validation

- [x] T006 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/158-deployment-host-inventory-spike` and confirm `OK`.

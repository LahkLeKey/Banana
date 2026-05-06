# Research: One-Window Interactive Baseline

## Decision 1: Treat integrated browser validation as closure evidence, not optional convenience

- Decision: Frontend and Electron behavior changes must be validated in VS Code integrated browser before task closure.
- Rationale: Recent drift showed env/container checks can pass while rendered behavior still fails (`API base: <unset>`), so rendered validation must be first-class evidence.
- Alternatives considered:
  - Keep integrated browser optional and rely on container env checks.
  - Require external browser only.
  - Rejected because both alternatives preserve blind spots or reintroduce context switching.

## Decision 2: Use existing compose profile scripts/tasks as startup contract

- Decision: Keep `scripts/compose-run-profile.sh` and `scripts/compose-profile-ready.sh` as the canonical startup/readiness path for one-window validation.
- Rationale: These entry points already encode profile boundaries and readiness checks and align with repository runtime contract lessons.
- Alternatives considered:
  - Add new one-off startup scripts only for this slice.
  - Launch services manually with ad-hoc compose commands.
  - Rejected because they increase drift and duplicate orchestration behavior.

## Decision 3: Keep enforcement lightweight and documentation-led for this slice

- Decision: Implement baseline through constitution + Copilot instructions + runbook + spec tasks/checklists, while leveraging existing diagnostic tooling for verification.
- Rationale: The workflow already has enforcement touchpoints (Spec Kit, PR review, existing drift script). A documentation-first hardening slice gives immediate alignment with low implementation risk.
- Alternatives considered:
  - Build new CI hard-fail workflow immediately.
  - Add custom VS Code extension enforcement.
  - Rejected because they add complexity and can be phased in after adoption baselines are stable.

## Decision 4: Scope baseline to frontend and Electron UX work

- Decision: Apply one-window interactive baseline to frontend and desktop UX slices where rendered behavior is a release signal.
- Rationale: This matches the observed failure mode and avoids forcing irrelevant constraints on backend-only changes.
- Alternatives considered:
  - Apply universally to all repository domains.
  - Keep scope to React only.
  - Rejected because universal scope is over-broad, while React-only is too narrow for shared drift patterns across channels.

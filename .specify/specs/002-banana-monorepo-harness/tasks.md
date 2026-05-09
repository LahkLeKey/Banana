# Tasks: Banana Monorepo Workflow Harness

## Phase 1 - Canonical Harness Path

- [x] Rename `.github/workflows/pre-commit.yml` to `.github/workflows/banana.yml`.
- [x] Update internal self-reference in runtime compatibility lane.
- [x] Validate YAML syntax after rename.

## Phase 2 - Governance and Documentation

- [x] Update `.github/workflows/README.md` with canonical harness file path.
- [x] Amend `.specify/memory/constitution.md` with canonical `banana.yml` path policy.
- [x] Bump constitution version for traceability.

## Phase 3 - Spec Kit Scaffolding

- [x] Create feature spec in `.specify/specs/002-banana-monorepo-harness/spec.md`.
- [x] Add implementation plan in `.specify/specs/002-banana-monorepo-harness/plan.md`.
- [x] Add task scaffold in `.specify/specs/002-banana-monorepo-harness/tasks.md`.
- [ ] Update `.specify/feature.json` to point to this feature when execution moves to implementation mode.

## Phase 4 - Verification

- [ ] Verify Actions UI reflects `Banana-Monorepo` naming after workflow indexing refresh.
- [ ] Confirm only the canonical harness remains for repository-managed CI/CD lanes.

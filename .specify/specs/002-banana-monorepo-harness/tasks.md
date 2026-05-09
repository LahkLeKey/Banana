# Tasks: Banana Monorepo Workflow Harness

## Phase 1 - Canonical Harness Path

- [x] T001 [US2] Rename `.github/workflows/pre-commit.yml` to `.github/workflows/banana.yml`. (FR-001)
- [x] T002 [US2] Update internal self-reference in runtime compatibility lane. (FR-001)
- [x] T003 [US2] Validate YAML syntax after rename. (FR-003)

## Phase 2 - Governance and Documentation

- [x] T004 [US3] Update `.github/workflows/README.md` with canonical harness file path. (FR-005)
- [x] T005 [US3] Amend `.specify/memory/constitution.md` with canonical `banana.yml` path policy. (FR-005)
- [x] T006 [US3] Bump constitution version for traceability. (FR-005)

## Phase 3 - Spec Kit Scaffolding

- [x] T007 [US3] Create feature spec in `.specify/specs/002-banana-monorepo-harness/spec.md`. (FR-005)
- [x] T008 [US3] Add implementation plan in `.specify/specs/002-banana-monorepo-harness/plan.md`. (FR-005)
- [x] T009 [US3] Add task scaffold in `.specify/specs/002-banana-monorepo-harness/tasks.md`. (FR-005)
- [x] T010 [US3] Update `.specify/feature.json` to point to this feature when execution moves to implementation mode. (FR-005)

## Phase 4 - Verification

- [x] T011 [US1] Verify Actions UI reflects `Banana-Monorepo` naming after workflow indexing refresh. (FR-002)
- [x] T012 [US1] Confirm only the canonical harness remains for repository-managed CI/CD lanes. (FR-001, FR-004)

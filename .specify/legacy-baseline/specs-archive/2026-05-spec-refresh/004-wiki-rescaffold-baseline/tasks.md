# Tasks: Wiki Rescaffold Baseline

**Input**: Design documents from `.specify/specs/004-wiki-rescaffold-baseline/`
**Prerequisites**: plan.md, spec.md

## Phase 1: Baseline Definition

- [x] T001 [US1] Define approved baseline information architecture (sections, indexes, and home page links). (FR-002)
- [x] T002 [US1] Capture existing wiki inventory and classify pages as keep, rewrite, or remove. (FR-001)

## Phase 2: Canonical Source Reset

- [x] T003 [US1] Remove out-of-scope research-era pages from `.specify/wiki/human-reference/`. (FR-001)
- [x] T004 [US1] Scaffold replacement baseline pages in `.specify/wiki/human-reference/`. (FR-002)
- [x] T005 [US3] Update `.specify/wiki/human-reference-allowlist.txt` to exactly match approved baseline paths. (FR-004)

## Phase 3: Publication Surface Reset

- [x] T006 [US1] Remove out-of-scope pages from `.wiki/`. (FR-001)
- [x] T007 [US2] Re-scaffold `.wiki/` to mirror canonical source baseline. (FR-003)
- [x] T008 [US2] Update wiki home/index pages to remove stale links. (FR-005)

## Phase 4: Validation and Governance

- [x] T009 [US2] Run wiki sync in dry-run mode and verify no unexpected page drift. (FR-003)
- [x] T010 [US3] Run link/path validation for home and section index pages. (FR-005)
- [ ] T011 [US3] Document ongoing governance and approval process for future wiki growth. (FR-004)

## Phase 5: DDD + SOLID Hardening

- [ ] T012 [US3] Separate wiki policy evaluation from sync orchestration and filesystem publish adapters in active tooling. (FR-006)
- [ ] T013 [US3] Add validation checks that enforce architecture boundaries for future wiki-sync changes. (FR-006)

# Tasks: Wiki Rescaffold Baseline

**Input**: Design documents from `.specify/specs/004-wiki-rescaffold-baseline/`
**Prerequisites**: plan.md, spec.md

## Phase 1: Baseline Definition

- [ ] T001 [US1] Define approved baseline information architecture (sections, indexes, and home page links). (FR-002)
- [ ] T002 [US1] Capture existing wiki inventory and classify pages as keep, rewrite, or remove. (FR-001)

## Phase 2: Canonical Source Reset

- [ ] T003 [US1] Remove out-of-scope research-era pages from `.specify/wiki/human-reference/`. (FR-001)
- [ ] T004 [US1] Scaffold replacement baseline pages in `.specify/wiki/human-reference/`. (FR-002)
- [ ] T005 [US3] Update `.specify/wiki/human-reference-allowlist.txt` to exactly match approved baseline paths. (FR-004)

## Phase 3: Publication Surface Reset

- [ ] T006 [US1] Remove out-of-scope pages from `.wiki/`. (FR-001)
- [ ] T007 [US2] Re-scaffold `.wiki/` to mirror canonical source baseline. (FR-003)
- [ ] T008 [US2] Update wiki home/index pages to remove stale links. (FR-005)

## Phase 4: Validation and Governance

- [ ] T009 [US2] Run wiki sync in dry-run mode and verify no unexpected page drift. (FR-003)
- [ ] T010 [US3] Run link/path validation for home and section index pages. (FR-005)
- [ ] T011 [US3] Document ongoing governance and approval process for future wiki growth. (FR-004)

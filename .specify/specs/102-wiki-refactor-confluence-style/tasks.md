# Tasks: Wiki Refactor — Confluence-Style Information Architecture

**Spec**: [spec.md](./spec.md) · **Plan**: [plan.md](./plan.md)
**Branch**: `102-wiki-refactor-confluence-style`

## Stage 1 — Information Architecture

- [x] **T001** Define six-section layout (`getting-started`, `architecture`, `operations`, `governance`, `data`, `security`) and document in plan.md. (agent: technical-writer-agent)
- [x] **T002** Create section `README.md` index pages for each section with child-page list and one-sentence summary. (agent: technical-writer-agent)
- [x] **T003** Move `Disaster-Recovery-Runbook.md` → `operations/disaster-recovery.md`; delete old path. (agent: technical-writer-agent)
- [x] **T004** Move `threat-model.md` → `security/threat-model.md`; delete old path and `.wiki/threat-model.md`. (agent: technical-writer-agent)

## Stage 2 — Content Migration

- [x] **T010** Update `.specify/wiki/human-reference/Home.md` with section cards and correct links to new paths. (agent: technical-writer-agent)
- [x] **T011** Migrate all existing pages into correct section subdirectories; update any internal cross-links. (agent: technical-writer-agent)
- [x] **T012** Update `.specify/wiki/human-reference-allowlist.txt` to reflect new section paths. (agent: technical-writer-agent)

## Stage 3 — Validation

- [x] **T020** Verify allowlist contains no orphaned or missing paths relative to current `human-reference/` tree. (agent: technical-writer-agent)
- [x] **T021** Verify `Home.md` navigation links all resolve to existing files. (agent: technical-writer-agent)

## Traceability

| Task  | FR                    |
|-------|-----------------------|
| T001  | FR-001, FR-006        |
| T002  | FR-001, FR-002        |
| T003  | FR-003                |
| T004  | FR-003                |
| T010  | FR-004, FR-006        |
| T011  | FR-002, FR-005        |
| T012  | FR-005, SC-001        |
| T020  | SC-001                |
| T021  | SC-002                |

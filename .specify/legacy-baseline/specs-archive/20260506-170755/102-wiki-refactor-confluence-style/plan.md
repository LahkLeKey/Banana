# Plan: Wiki Refactor — Confluence-Style Information Architecture

**Spec**: [spec.md](./spec.md)
**Branch**: `102-wiki-refactor-confluence-style`
**Created**: 2026-05-01

---

## Design Decisions

### DD-001: Section Layout
Use a six-section layout under `.specify/wiki/human-reference/`:
- `getting-started/` — onboarding, build/run, layout
- `architecture/` — system overview, domain model, native ABI, DB pipeline
- `operations/` — CI notes, compose stabilization, runtime compat, WSL2 channels, disaster recovery
- `governance/` — API parity, autonomous self-training
- `data/` — coverage exceptions, migration discipline
- `security/` — threat model, CSP headers

Each section has a `README.md` index page.

### DD-002: Allowlist Enforcement
`.specify/wiki/human-reference-allowlist.txt` is updated to reflect the new paths.
The allowlist prevents unreviewed page growth.

### DD-003: Deletions
- `.specify/wiki/human-reference/Disaster-Recovery-Runbook.md` → moved to `operations/disaster-recovery.md`
- `.specify/wiki/human-reference/threat-model.md` → moved to `security/threat-model.md`
- `.wiki/threat-model.md` → removed (content migrated to allowlist-controlled path)

### DD-004: Section README Pages
Each section has a README.md with a child-page index and a one-sentence summary.

---

## Implementation Approach

All changes are file reorganization + content updates to Home.md and section READMEs.
No new scripts are required for the initial refactor. The existing `scripts/workflow-sync-wiki.sh`
handles wiki publication.

The `.specify/wiki/human-reference-allowlist.txt` is the only automation contract that changes.

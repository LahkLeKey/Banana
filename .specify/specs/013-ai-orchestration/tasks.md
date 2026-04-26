---
description: "Tasks for v2 AI orchestration regeneration"
---

# Tasks: AI Orchestration (v2)

## Phase 0 — Overlap audit

- [ ] T001 Map every helper agent description to the file paths it owns; flag overlaps.
- [ ] T002 Map every skill to its applicable scope; flag overlaps with sibling skills.

## Phase 1 — Dedup

- [ ] T010 Tighten agent descriptions so exactly one matches a given path.
- [ ] T011 [P] Trim skill content boundaries (discovery vs build-and-run, etc.).

## Phase 2 — Workflow consolidation

- [ ] T020 Identify shared steps across orchestration workflows.
- [ ] T021 Extract into a composite action under `.github/actions/<name>/action.yml`.
- [ ] T022 Update workflows to use composite action.

## Phase 3 — Guard correctness

- [ ] T030 Audit every workflow actor guard; ensure scheduled jobs include `github.event_name == 'schedule'`.
- [ ] T031 Quote YAML `${{ ... }}` defaults and input descriptions per parsing rules.

## Phase 4 — Documentation gap

- [ ] T040 Document `.specify/integrations/` purpose.
- [ ] T041 Document `.specify/extensions/` and `.specify/extensions.yml`.

## Phase 5 — Validator

- [ ] T050 Run `python scripts/validate-ai-contracts.py`; expect `"issues": []`.

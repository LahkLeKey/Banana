# Feature Specification: AI Orchestration (v2)

**Feature Branch**: `013-ai-orchestration`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`005-v2-regeneration`](../005-v2-regeneration/spec.md)  
**Baseline**: [`legacy-baseline/ai-orchestration.md`](../../legacy-baseline/ai-orchestration.md)

## Summary

Regenerate the AI orchestration surface — workflows, prompts, agents, skills,
instructions, Spec Kit — as a smaller, less-overlapping set with the wiki
freeze contract preserved and AI contract guard kept green throughout v2.

## User Scenarios & Testing

### User Story 1 — Wiki freeze stays enforced (Priority: P1)

As an AI agent, I cannot expand `.wiki/` outside the 12-file allowlist
without an intentional allowlist change. The validator catches drift.

### User Story 2 — One canonical helper agent per scope (Priority: P1)

As a planner, the narrowest helper agent for a touched area is unambiguous.
Overlapping helper-agent descriptions are deduplicated.

### User Story 3 — Workflows respect bot/human distinction (Priority: P2)

As CI, scheduled workflows include `github.event_name == 'schedule'` in
actor-guard conditions so cron (which runs as the bot) is not skipped.

### User Story 4 — Skills are deduplicated (Priority: P2)

As an agent, skills like discovery vs build-and-run have clear non-overlapping
content boundaries.

### Edge Cases

- A new wiki page is genuinely human-needed → expand allowlist via the documented process; do not bypass.
- A workflow's actor guard accidentally blocks scheduled cron → fail-loud with a comment in the YAML.

## Requirements

### Functional Requirements

- **FR-001**: v2 MUST preserve the wiki allowlist freeze and parity validation in `scripts/validate-ai-contracts.py`.
- **FR-002**: v2 MUST keep `.specify/wiki/` as the canonical AI-consumable wiki and `.wiki/` as the human-reference surface.
- **FR-003**: v2 MUST keep `.specify/wiki/human-reference-allowlist.txt` as the freeze list.
- **FR-004**: v2 MUST run `python scripts/validate-ai-contracts.py` in CI via `.github/workflows/ai-contract-guard.yml`.
- **FR-005**: v2 MUST preserve helper-agent decomposition pattern; eliminate description overlap between helpers in the same domain.
- **FR-006**: v2 MUST include `github.event_name == 'schedule'` in scheduled workflow actor guards.
- **FR-007**: v2 MUST quote workflow YAML `${{ ... }}` env defaults containing `:` and quote input descriptions containing `{...: ...}` snippets.
- **FR-008**: v2 SHOULD consolidate orchestration workflows via composite actions where two or more workflows share steps.

### Hard contracts to preserve

- AI contract guard validator path + exit semantics.
- Wiki allowlist file location + 12 entries (until intentionally expanded).
- Spec Kit constitution at `.specify/memory/constitution.md`.
- Helper-agent decomposition pattern.
- Prompt-wiki linkage validated by the guard.

### Pain points addressed

- Workflow duplication → composite actions (FR-008).
- Helper-agent overlap → deduplicated descriptions (FR-005).
- YAML parsing fragility → quoting rules (FR-007).
- Cron skipped under actor guards → schedule clause (FR-006).
- Skills overlap → boundary clarification (User Story 4).

### Pain points deferred

- `.specify/integrations/` and `extensions/` documentation gap — separate doc PR.

## Success Criteria

- **SC-001**: Validator green at end of v2 work.
- **SC-002**: For every domain, exactly one helper agent matches a given file path.
- **SC-003**: All scheduled workflows include the schedule-event clause.
- **SC-004**: Wiki allowlist parity holds (`.wiki/` file count == allowlist == `.specify/wiki/human-reference/` file count).

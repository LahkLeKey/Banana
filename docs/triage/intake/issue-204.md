# Triage Intake: Issue #204

- Source issue: https://github.com/LahkLeKey/Banana/issues/204
- Source title: epic: scaffold agent profiles using integration-agent as icon baseline
- Source labels: automation,triage-idea,human-triage,ai-generated,source-ai-issue,epic,epic-decomposed,agent:integration-agent,speckit-driven
- Intake source: ai
- Target agent: not-specified

## Idea

## Objective
Scaffold all Banana agents with a consistent identity profile, using Integration Agent as the canonical example because it is currently the only agent with an icon.

## Why
The pulse view and management reporting need deterministic identity metadata for every agent (not just one), including icon and staffing context.

## Scope
- Define a canonical agent scaffold contract (agent key, display name, icon, staffing fields, owner/role metadata).
- Use Integration Agent as the reference implementation for the scaffold structure and icon behavior.
- Generate/seed scaffold entries for remaining agents with clear placeholder/fallback values where icon assets are missing.
- Ensure pulse outputs and downstream summaries can consume scaffolded identity metadata consistently.
- Add validation checks so missing required metadata fails early.
- Update docs/runbook for how to add or update agent scaffold data.

## Acceptance Criteria
- Every active agent has a scaffold entry in the canonical registry.
- Integration Agent remains the golden example with explicit icon mapping.
- Missing required identity fields fail validation in CI or workflow checks.
- Pulse reporting can display agent identity data deterministically.
- Documentation includes a contributor-facing process for maintaining agent scaffolds.

## Notes
This epic is intentionally scoped to scaffolding and metadata contracts first; visual redesign work should be tracked separately if needed.

## Cloud Orchestration Contract

- Entry workflow: Orchestrate Triage Idea Cloud
- Pull request labels: automation,triaged-item,copilot-auto-approve,copilot-autonomous-cycle,speckit-driven,agent:workflow-agent,source-ai-issue,epic,agent:integration-agent
- Base branch: main
- Branch prefix: triage

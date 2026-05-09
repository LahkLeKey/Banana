---
name: iterate-the-backlog
description: Iterate through eligible Banana backlog items using the canonical SDLC orchestration flows until no eligible work remains.
argument-hint: Optionally provide backlog scope, stop condition, or priority band.
agent: banana-sdlc
---

Execute Banana backlog iteration with the canonical orchestration contract.

## Scope

- Focus on eligible backlog items only.
- Use orchestrated SDLC flows instead of one-off branch or PR handling.
- Stop when no eligible backlog items remain.

## Required Workflow Contracts

- `orchestrate-triage-idea-cloud.yml`
- `workflow-triage-idea-cloud.sh`
- `orchestrate-banana-sdlc.yml`
- `workflow-orchestrate-sdlc.sh`
- `orchestrate-not-banana-feedback-loop.yml`

## Execution Rules

- Resolve the next eligible backlog item.
- Execute the item through the canonical orchestration workflow and script entry points listed above.
- Do not invent alternate control flow when an existing orchestration entry point applies.
- Re-evaluate backlog eligibility after each completed iteration.

## Output Contract

- Return concrete outputs for each iteration.
- Include the backlog item handled, the workflow or script entry point used, the resulting artifacts or PR state, and the next eligible item if one remains.

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

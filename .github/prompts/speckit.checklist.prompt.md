---
name: speckit.checklist
description: Generate a custom checklist for the current feature based on user requirements.
argument-hint: Specify the requirement domain you want checklist coverage for.
agent: speckit.checklist
---

Run the Speckit checklist workflow.

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

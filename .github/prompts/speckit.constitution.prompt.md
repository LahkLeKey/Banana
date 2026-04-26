---
name: speckit.constitution
description: Create or update the project constitution and align dependent templates.
argument-hint: Provide the principle set or constitution updates to apply.
agent: speckit.constitution
---

Run the Speckit constitution workflow.

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

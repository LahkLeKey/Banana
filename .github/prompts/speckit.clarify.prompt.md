---
name: speckit.clarify
description: Identify underspecified areas in the current feature spec by asking up to 5 targeted clarification questions.
argument-hint: Provide the current feature context and uncertainty areas to clarify.
agent: speckit.clarify
---

Run the Speckit clarify workflow.

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

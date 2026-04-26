---
name: speckit.taskstoissues
description: Convert existing tasks into actionable, dependency-ordered GitHub issues.
argument-hint: Specify the feature/task set to publish as issues.
agent: speckit.taskstoissues
---

Run the Speckit tasks-to-issues workflow.

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

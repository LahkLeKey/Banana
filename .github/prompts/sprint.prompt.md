---
name: sprint
description: Run a Banana automation sprint with incremental feature branches, GH CLI pull requests, feedback-loop enforcement, and wiki sync.
argument-hint: Describe the sprint goal, planned increments, branch/reviewer constraints, and what must be automated.
agent: banana-sdlc
---

Execute this as an automation sprint for Banana.

Requirements:

- Treat feedback-loop updates as first-class SDLC inputs.
- Use incremental feature branches and GH CLI PR orchestration:
  - `scripts/workflow-orchestrate-triaged-item-pr.sh` for single-slice changes
  - `scripts/workflow-orchestrate-sdlc.sh` and `.github/workflows/orchestrate-banana-sdlc.yml` for multi-slice runs
- Keep wiki updates in the same flow via `scripts/workflow-sync-wiki.sh`.
- Keep no-op-safe incremental behavior enabled when a slice has no changes.
- Preserve Copilot triage and provenance guardrails for learning updates and automation PR approval gates.
- Delegate to narrow helper agents by ownership; use `integration-agent` for cross-domain validation and `banana-reviewer` for release risk review.
- Return concrete outputs: changed files, executed validations, branch names, and PR links.

Relevant assets:

- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)
- [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

---
name: focus-on-open-pull-requests
description: Focus Banana automation on open pull requests by triaging blockers, dispatching required checks, and returning prioritized merge actions.
argument-hint: Optional scope filters, label constraints, merge policy, and max PR count for this focus cycle.
agent: banana-sdlc
---

Execute this as an open pull request focus cycle for Banana.

Requirements:

- Treat the prompt argument as focus constraints (scope filters, labels, ownership, base branch, max PR count).
- Start by enumerating open pull requests and classifying each as: `merge-ready`, `waiting-checks`, `waiting-human-review`, `blocked-conflict-or-rebase`, or `superseded-or-close-candidate`.
- Keep review and gate orchestration on existing entry points only:
  - `.github/workflows/copilot-review-triage.yml`
  - `.github/workflows/require-human-approval.yml`
- For automation PRs, ensure required checks are dispatched and report resulting workflow run URLs.
- Preserve provenance and routing labels (`automation`, `triaged-item`, `copilot-auto-approve`, `speckit-driven`, plus source and `agent:*` labels when present).
- Do not force-merge or bypass branch protections unless explicitly requested by a human.
- Do not open duplicate replacement PRs when an equivalent branch and scope are already active.
- Return concrete outputs for each PR handled: PR URL, current check state, chosen action, run URLs if dispatched, and blocker reason.

Relevant assets:

- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)
- [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

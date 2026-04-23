---
name: iterate-the-backlog
description: Iterate through Banana triage and automation backlog with incremental orchestration, required check gating, and deterministic stop conditions.
argument-hint: Describe backlog scope, iteration cap, and any label or workflow constraints.
agent: banana-sdlc
---

Execute this as a backlog-iteration cycle for Banana.

Requirements:

- Treat the prompt argument as backlog-iteration constraints (scope filters, max iterations, labels, workflows).
- Start by enumerating open triage issues and automation pull requests that match backlog labels.
- Use incremental slices and existing orchestration entry points only:
  - `.github/workflows/orchestrate-triage-idea-cloud.yml` with `scripts/workflow-triage-idea-cloud.sh` for triage issue intake and update cycles.
  - `.github/workflows/orchestrate-banana-sdlc.yml` with `scripts/workflow-orchestrate-sdlc.sh` for multi-slice backlog increments.
  - `.github/workflows/orchestrate-not-banana-feedback-loop.yml` for approved feedback backlog ingestion.
- Keep test gating enabled for every generated or updated pull request by dispatching required checks:
  - `.github/workflows/copilot-review-triage.yml`
  - `.github/workflows/require-human-approval.yml`
- Preserve provenance and routing labels across the cycle (`automation`, `triaged-item`, `copilot-auto-approve`, `copilot-bypass-vibe-coded`, plus source and `agent:*` labels when present).
- Apply smart iteration stop conditions:
  - Stop when no eligible backlog items remain.
  - Stop when iteration cap is reached.
  - Stop early on repeated no-change outcomes or unresolved blocking failures.
- Do not open duplicate pull requests for backlog items that already have an active PR unless explicitly requested.
- Return concrete outputs for each iteration: processed issue and PR URLs, workflow run URLs, check-dispatch results, merge or skip decisions, and blocking reasons.

Relevant assets:

- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)
- [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

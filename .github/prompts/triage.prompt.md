---
name: triage
description: Create a labeled triage issue from an idea and run cloud triage orchestration workflows.
argument-hint: Provide an idea in quotes, plus optional issue_labels and pr_labels overrides.
agent: banana-sdlc
---

Execute this as Banana cloud triage intake.

Requirements:

- Treat the prompt argument as the triage idea text.
- Create or update a GitHub issue for the idea with custom labels when provided.
- Ensure label `triage-idea` is applied so cloud orchestration can trigger from issue events.
- Include label `copilot-suggestion` when the idea originates from Copilot review findings.
- Mark AI-originated triage issues with `ai-generated` and avoid applying `source-human-issue`.
- Preserve human-template routing markers (`banana-intake-source` and `banana-target-agent`) when orchestrating from existing human-created issues.
- Support any static helper agent defined in `.github/agents/*.agent.md` when `banana-target-agent` is provided by a human template.
- Keep propagated `agent:*` labels on both triage issues and generated PRs for deterministic helper routing.
- Clear backlog triage issues and triage automation PRs before creating new ones unless backlog cleanup is explicitly disabled.
- When the idea is epic-like, decompose it into linked `epic`, `user-story`, and `task` issues instead of keeping a 1:1 issue-to-PR shape.
- Start CI from the first generated task by dispatching triage orchestration for that task with backlog cleanup disabled.
- Preserve epic/story/task labels on generated PRs so backlog iteration can progress task-by-task until epic completion.
- Trigger cloud orchestration through workflow `.github/workflows/orchestrate-triage-idea-cloud.yml`.
- Preserve provenance labels on generated pull requests (`copilot-auto-approve` and `copilot-bypass-vibe-coded` unless overridden).
- Return concrete outputs: issue URL, workflow run URL, generated PR URL (if created), and any skipped-no-change reason.

Relevant assets:

- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

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
- Clear backlog triage issues and triage automation PRs before creating new ones unless backlog cleanup is explicitly disabled.
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

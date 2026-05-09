---
name: focus-on-open-pull-requests
description: Work through open pull requests with explicit branch-protection and duplication safeguards.
argument-hint: Optionally provide PR filter, branch, or review focus area.
agent: banana-sdlc
---

Execute a focused pass over open pull requests for Banana.

## Scope

- Focus only on open pull requests.
- Evaluate each PR for the next actionable step without widening scope unnecessarily.

## Safety Rules

- Do not force-merge or bypass branch protections unless explicitly requested by a human.
- Do not open duplicate replacement PRs.
- Preserve existing review context, required checks, and branch protection expectations.

## Execution Rules

- Inspect the current state of each relevant open PR.
- Prefer progressing the existing PR rather than creating a new one.
- Record blockers when a PR cannot progress autonomously.

## Output Contract

- Return concrete outputs for each PR handled.
- Include PR identifier, status, blocking checks or review issues, actions taken, and the next recommended step.

## Wiki Updater Contract

- Treat this prompt as a wiki-synced contract surface: when prompt behavior, scope, assets, or acceptance criteria changes, keep the corresponding wiki snapshot current in the same SDLC run.
- Use `scripts/workflow-sync-wiki.sh` (or SDLC orchestration wrappers) so prompt updates and wiki docs are delivered together.
- If a prompt change introduces new automation flow, branch policy, validation step, or contract dependency, update prompt-linked wiki content before closing the change.

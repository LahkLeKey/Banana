---
name: review-banana-change
description: Review Banana changes for bugs, regressions, missing tests, and release blockers.
argument-hint: Describe the changes to review or ask for a review of the current worktree.
agent: banana-reviewer
---

Review this Banana change in code-review mode.

Requirements:

- Prioritize findings over summaries.
- Focus on correctness, runtime risk, contract breaks, missing tests, and delivery issues.
- Order findings by severity and include concrete file references.
- Note when the work should have been split into narrower helper-owned slices but was not.
- If no findings are present, say so and note any residual validation gaps.

Relevant assets:

- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
- Identify the primary owner, supporting helpers, and validation owner before implementation.
- When handing work to another agent, include objective, ownership reason, touched files, contract impacts, validation state, and open risks.
- Preserve context continuity by carrying forward confirmed assumptions and prior validation signals.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.

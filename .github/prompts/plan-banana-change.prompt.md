---
name: plan-banana-change
description: Plan a Banana feature, bug fix, refactor, or workflow change before implementation.
argument-hint: Describe the requested change, affected behavior, and any constraints.
agent: banana-planner
---

Plan this Banana change before any implementation.

Requirements:

- Use the repo architecture and runtime contracts already defined in the workspace.
- Identify the touched domains, exact files or folders likely involved, and the smallest safe implementation order.
- Recommend the narrowest helper agent for each slice before naming a broader coordinating agent.
- Make helper handoffs explicit when more than one slice is required.
- Reuse existing build, test, coverage, and compose entry points.
- Provide a validation matrix and call out required env vars, runtime assumptions, and risks.

Relevant assets:

- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-discovery](../skills/banana-discovery/SKILL.md)
- [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

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
- Reuse existing build, test, coverage, and compose entry points.
- Provide a validation matrix and call out required env vars, runtime assumptions, and risks.

Relevant assets:

- [banana-discovery](../skills/banana-discovery/SKILL.md)
- [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)

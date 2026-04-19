---
name: implement-banana-change
description: Implement a Banana change end to end with the right domain agent, validation, and summary.
argument-hint: Describe the change to implement and include acceptance criteria if you have them.
agent: banana-sdlc
---

Implement this Banana change end to end.

Requirements:

- Discover the impacted domains first and keep the change scoped.
- Split the work into helper-owned slices whenever a narrower helper clearly owns the files or risk.
- Use a parent domain agent only when multiple helpers in the same domain must move together.
- Reuse existing tasks, scripts, workflows, and compose profiles before adding new automation.
- Run the relevant validation for the touched layers.
- Summarize changed behavior, verification, assumptions, and residual risks.

Relevant assets:

- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

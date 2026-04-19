---
name: validate-banana-change
description: Validate Banana changes with the correct build, test, coverage, and compose scope.
argument-hint: Describe what changed and what level of validation you need.
agent: integration-agent
---

Validate this Banana change with the smallest sufficient test and runtime surface.

Requirements:

- Choose the narrowest existing validation path that still matches the change risk.
- Expand to integration, compose, or coverage validation if the change crosses layers.
- Make env var and runtime assumptions explicit.
- Report the exact stage that passed or failed, the likely helper owner of any failure, and any gaps that remain.

Relevant assets:

- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)
- [banana-ci-debugging](../skills/banana-ci-debugging/SKILL.md)

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

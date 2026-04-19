---
name: implement-banana-workflow-change
description: Implement a focused Banana GitHub Actions or coverage automation change.
argument-hint: Describe the workflow job, artifact path, or CI stage change to implement.
agent: workflow-agent
---

Implement this Banana GitHub Actions or coverage automation change with focused ownership.

Requirements:

- Keep work centered on `.github/workflows` and the matching local mirror behavior.
- Preserve artifact paths and runtime assumptions unless the request explicitly changes them.
- Coordinate with `compose-runtime-agent` if the local runtime scripts must stay in sync.
- Summarize CI behavior changes, local validation mirror, and rollout assumptions.

Relevant assets:

- [infra.instructions.md](../instructions/infra.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-ci-debugging](../skills/banana-ci-debugging/SKILL.md)
## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

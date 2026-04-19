---
name: implement-banana-react-ui-change
description: Implement a focused Banana React UI or client-state change under src/typescript/react.
argument-hint: Describe the screen, component, state flow, or frontend contract you want to change.
agent: react-ui-agent
---

Implement this Banana React UI change with focused ownership.

Requirements:

- Keep work centered on `src/typescript/react/src` unless a backend contract change is explicitly required.
- Preserve Bun workflows and `VITE_BANANA_API_BASE_URL`.
- Keep API access typed and explicit.
- Summarize UI behavior changes, validation run, and any backend assumptions.

Relevant assets:

- [frontend.instructions.md](../instructions/frontend.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

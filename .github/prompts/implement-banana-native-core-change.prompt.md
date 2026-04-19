---
name: implement-banana-native-core-change
description: Implement a focused Banana native core change without broadening into wrapper, DAL, or managed code unless required.
argument-hint: Describe the core rule, algorithm, model, or native test behavior to change.
agent: native-core-agent
---

Implement this Banana native core change with focused ownership.

Requirements:

- Keep work inside `src/native/core`, `tests/native`, and directly related CMake wiring unless ABI or DAL contracts force wider edits.
- Coordinate with `native-wrapper-agent` only if exported behavior or managed interop must change.
- Reuse the existing native build and CTest flow.
- Summarize algorithm impact, validation run, downstream helper coordination, and residual risks.

Relevant assets:

- [native.instructions.md](../instructions/native.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)
## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

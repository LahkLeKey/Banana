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
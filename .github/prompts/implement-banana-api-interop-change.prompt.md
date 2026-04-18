---
name: implement-banana-api-interop-change
description: Implement a focused Banana managed interop or data-access contract change.
argument-hint: Describe the interop signature, status mapping, data-access selection, or managed contract change to implement.
agent: api-interop-agent
---

Implement this Banana managed interop or data-access change with focused ownership.

Requirements:

- Keep work centered on `NativeInterop`, `DataAccess`, and matching tests.
- Coordinate with `native-wrapper-agent` if the native ABI or status surface moves.
- Preserve explicit runtime contracts such as `BANANA_NATIVE_PATH` and `BANANA_PG_CONNECTION`.
- Summarize contract changes, validation run, and remaining runtime assumptions.

Relevant assets:

- [api.instructions.md](../instructions/api.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
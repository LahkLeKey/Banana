---
name: implement-banana-native-wrapper-change
description: Implement a focused Banana wrapper or ABI change and coordinate managed interop only when required.
argument-hint: Describe the export, status code, struct, or buffer ownership change to implement.
agent: native-wrapper-agent
---

Implement this Banana wrapper or ABI change with focused ownership.

Requirements:

- Keep work centered on `src/native/wrapper` and explicit contract synchronization.
- Preserve the ABI unless the request explicitly requires a contract change.
- Coordinate with `api-interop-agent` when managed interop must stay in sync.
- Summarize contract deltas, validation run, and downstream integration assumptions.

Relevant assets:

- [native.instructions.md](../instructions/native.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)
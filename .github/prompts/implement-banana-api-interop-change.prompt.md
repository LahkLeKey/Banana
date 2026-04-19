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
## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
- Identify the primary owner, supporting helpers, and validation owner before implementation.
- When handing work to another agent, include objective, ownership reason, touched files, contract impacts, validation state, and open risks.
- Preserve context continuity by carrying forward confirmed assumptions and prior validation signals.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.

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

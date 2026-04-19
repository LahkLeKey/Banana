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

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](../agents/domain-teaming-playbook.md) for ownership boundaries, handoff packet expectations, and escalation rules.
- Identify the primary owner, supporting helpers, and validation owner before implementation.
- When handing work to another agent, include objective, ownership reason, touched files, contract impacts, validation state, and open risks.
- Preserve context continuity by carrying forward confirmed assumptions and prior validation signals.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.

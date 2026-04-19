---
name: debug-banana-test-failure
description: Debug a Banana unit, integration, native, or coverage failure and route the fix to the right helper agent.
argument-hint: Describe the failing test, build error, or validation stage and include the relevant output.
agent: test-triage-agent
---

Debug this Banana test or validation failure.

Requirements:

- Identify the exact failing stage before proposing a fix.
- Distinguish harness issues, environment drift, and product defects.
- Re-run the narrowest useful reproducer first.
- Route the actual fix to the narrowest helper agent that owns the failing surface.

Relevant assets:

- [testing.instructions.md](../instructions/testing.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-ci-debugging](../skills/banana-ci-debugging/SKILL.md)
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

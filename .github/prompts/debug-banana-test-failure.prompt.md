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
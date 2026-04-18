---
name: implement-banana-api-pipeline-change
description: Implement a focused Banana ASP.NET controller, service, middleware, or pipeline change.
argument-hint: Describe the HTTP behavior, service orchestration, middleware rule, or pipeline step to implement.
agent: api-pipeline-agent
---

Implement this Banana ASP.NET pipeline change with focused ownership.

Requirements:

- Preserve the controller -> service -> pipeline -> native interop flow.
- Keep transport concerns in controllers and orchestration in services or steps.
- Coordinate with `api-interop-agent` only if the native or data-access contract must move.
- Summarize behavior changes, validation run, and any helper handoffs required.

Relevant assets:

- [api.instructions.md](../instructions/api.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-test-and-coverage](../skills/banana-test-and-coverage/SKILL.md)
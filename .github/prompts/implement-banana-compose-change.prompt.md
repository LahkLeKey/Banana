---
name: implement-banana-compose-change
description: Implement a focused Banana compose or local runtime change.
argument-hint: Describe the compose profile, container behavior, port, health check, or runtime script change to implement.
agent: compose-runtime-agent
---

Implement this Banana compose or local runtime change with focused ownership.

Requirements:

- Keep work centered on `docker-compose.yml`, `docker`, and compose-oriented scripts.
- Preserve profile names, ports, and health checks unless the request explicitly changes them.
- Coordinate with `workflow-agent` only if the same behavior must also change in GitHub Actions.
- Summarize runtime changes, validation run, and environment assumptions.

Relevant assets:

- [infra.instructions.md](../instructions/infra.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
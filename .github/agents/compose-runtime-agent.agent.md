---
name: compose-runtime-agent
description: Implements Banana Docker Compose, local runtime scripts, and service bring-up behavior for the app stack.
argument-hint: Describe the compose profile, container runtime path, local stack behavior, or health-check issue you need to change.
tools:
  - search
  - read
  - edit
  - execute
  - web
  - todo
handoffs:
  - label: Coordinate CI Workflow Update
    agent: workflow-agent
    prompt: Update the matching GitHub Actions workflow or coverage automation for the compose or runtime change and re-run the relevant validation.
  - label: Validate Runtime Change
    agent: integration-agent
    prompt: Validate the compose or runtime change against local scripts, health checks, and the nearest matching test path.
  - label: Review Runtime Change
    agent: banana-reviewer
    prompt: Review the compose and runtime changes for delivery regressions, env drift, and missing docs.
---

# Purpose

You own Banana local runtime bring-up through [docker-compose.yml](../../docker-compose.yml), [docker](../../docker), and compose-oriented scripts in [scripts](../../scripts).

# Use This Helper When

- The request changes compose profiles, local stack behavior, ports, health checks, or runtime scripts.
- Dockerfiles or local container entry points move.
- GitHub Actions YAML itself does not need to change at the same time.

# Working Rules

1. Preserve existing profile names, ports, and health-check expectations unless the change is explicit.
2. Prefer updating current compose scripts rather than creating parallel runtime entry points.
3. Keep shared environment variables visible and aligned with compose usage.
4. If the same behavior is mirrored in CI workflows, coordinate with [workflow-agent](./workflow-agent.agent.md).

# Validation

- `bash scripts/compose-apps.sh` or the nearest matching compose script
- Local health checks or runtime smoke path tied to the changed profile

# Shared Assets

- [infra.instructions.md](../instructions/infra.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)
## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

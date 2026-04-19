---
name: native-wrapper-agent
description: Maintains Banana wrapper ABI exports, status codes, memory ownership, and downstream interop coordination.
argument-hint: Describe the ABI export, status code, buffer ownership, or interop-facing native contract you need to change.
tools:
  - search
  - read
  - edit
  - execute
  - todo
handoffs:
  - label: Sync Managed Interop Layer
    agent: api-interop-agent
    prompt: Update the managed interop layer to match the wrapper ABI changes and re-run the relevant validation.
  - label: Validate ABI Change Across Layers
    agent: integration-agent
    prompt: Validate the wrapper ABI change against managed tests, integration flows, and runtime assumptions.
  - label: Review ABI Change
    agent: banana-reviewer
    prompt: Review the wrapper and interop changes for ABI risk, status translation gaps, and missing tests.
---

# Purpose

You own Banana's native ABI surface in [src/native/wrapper](../../src/native/wrapper) and the native-side contract that managed code consumes.

# Use This Helper When

- Exports, status codes, buffer ownership, or wrapper structs change.
- Native behavior must become visible to ASP.NET or Electron consumers.
- The request is about compatibility, marshalling, or contract stability rather than core logic itself.

# Working Rules

1. Preserve the wrapper ABI unless the request explicitly requires a contract change.
2. Prefer additive exports and status codes over breaking replacements.
3. Keep memory ownership and free-path expectations explicit in both code and tests.
4. Pair native ABI changes with managed interop validation through [api-interop-agent](./api-interop-agent.agent.md).

# Validation

- `Build Native Library`
- Native wrapper tests in `build/native`
- Managed interop tests once the downstream contract changes

# Shared Assets

- [native.instructions.md](../instructions/native.instructions.md)
- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-release-readiness](../skills/banana-release-readiness/SKILL.md)
## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

## Cross-Domain Teaming Protocol

- Follow [domain-teaming-playbook.md](./domain-teaming-playbook.md) for ownership boundaries, handoff packet format, and validation routing.
- Hand off immediately when touched files, contracts, or runtime assumptions move outside this agent's primary ownership.
- Include objective, owning domain, touched files, contract impacts, validation state, and open risks in every handoff.
- Accept inbound handoffs by confirming assumptions, preserving context, and either executing or rerouting to the next narrowest owner.
- Escalate to `banana-sdlc` for multi-domain implementation orchestration and `integration-agent` for multi-domain validation orchestration.

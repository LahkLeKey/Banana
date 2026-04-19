---
name: debug-banana-ci
description: Debug a Banana workflow, compose, or runtime failure by tracing it to the broken stage or dependency.
argument-hint: Describe the failing job, task, compose profile, or error output.
agent: infrastructure-agent
---

Debug this Banana CI, compose, or runtime failure.

Requirements:

- Map the failure to the corresponding local script, compose profile, or workflow job.
- Check environment contracts before assuming code regressions.
- Keep the investigation grounded in the existing delivery surface.
- Identify the narrowest helper that should own the eventual fix.
- If you fix the issue, validate the nearest matching local path and summarize the root cause.

Relevant assets:

- [banana-agent-decomposition](../skills/banana-agent-decomposition/SKILL.md)
- [banana-ci-debugging](../skills/banana-ci-debugging/SKILL.md)
- [banana-build-and-run](../skills/banana-build-and-run/SKILL.md)

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

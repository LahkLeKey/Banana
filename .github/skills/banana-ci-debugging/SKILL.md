---
name: banana-ci-debugging
description: Triage Banana workflow, compose, and runtime failures by mapping them to the exact stage, dependency, or environment contract that broke.
argument-hint: Describe the failing task, compose profile, or CI job and include the relevant output.
---

# Banana CI Debugging

Use this skill when a build, compose stack, or GitHub Actions job fails and you need a structured triage path.

## Triage Procedure

1. Identify which stage failed: native coverage, .NET tests, compose tests, runtime health check, or Electron smoke.
2. Map that stage to the corresponding local script, task, or workflow step and name the most likely helper owner.
3. Check environment assumptions before changing code.
4. Distinguish infrastructure breakage from application logic failures and test harness defects.
5. Route the fix to the narrowest helper once the failing surface is clear.
6. If a fix changes the delivery surface, update the closest docs or prompt references.

## Resources

- Triage checklist: [triage-checklist.md](./triage-checklist.md)
- Helper routing: [../banana-agent-decomposition/helper-matrix.md](../banana-agent-decomposition/helper-matrix.md)

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

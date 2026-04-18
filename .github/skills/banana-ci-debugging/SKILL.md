---
name: banana-ci-debugging
description: Triage Banana workflow, compose, and runtime failures by mapping them to the exact stage, dependency, or environment contract that broke.
argument-hint: Describe the failing task, compose profile, or CI job and include the relevant output.
---

# Banana CI Debugging

Use this skill when a build, compose stack, or GitHub Actions job fails and you need a structured triage path.

## Triage Procedure

1. Identify which stage failed: native coverage, .NET tests, compose tests, runtime health check, or Electron smoke.
2. Map that stage to the corresponding local script, task, or workflow step.
3. Check environment assumptions before changing code.
4. Distinguish infrastructure breakage from application logic failures.
5. If a fix changes the delivery surface, update the closest docs or prompt references.

## Resources

- Triage checklist: [triage-checklist.md](./triage-checklist.md)

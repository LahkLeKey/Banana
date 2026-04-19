---
name: banana-agent-decomposition
description: Break Banana work into the narrowest helper agents and explicit handoffs instead of overloading broad domain agents.
argument-hint: Describe the request, touched files, and whether you need planning, implementation, validation, or review.
---

# Banana Agent Decomposition

Use this skill when a Banana request is broad, crosses subdomains, or would be safer if split into helper-sized slices.

## Routing Procedure

1. Start from the touched paths, contracts, and phase of work: plan, implement, validate, review, or debug.
2. Prefer the narrowest helper agent that clearly owns the files and risks.
3. Use a parent domain agent only when two or more helpers in the same domain must move together.
4. Use `banana-sdlc` only for multi-phase or multi-domain orchestration.
5. Make handoffs explicit: helper name, owned files, acceptance criteria, and validation surface.

## Output

- Primary helper or coordinating agent
- Secondary helpers or validation agents
- Files or folders owned by each helper
- Ordered handoffs
- Validation owner per slice

## Resources

- Helper matrix: [helper-matrix.md](./helper-matrix.md)
- Discovery skill: [../banana-discovery/SKILL.md](../banana-discovery/SKILL.md)
## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

---
name: banana-build-and-run
description: Choose the correct Banana build, local run, or compose entry point based on the domain being changed.
argument-hint: Describe the area you changed and whether you need a build, local run, or full stack bring-up.
---

# Banana Build And Run

Use this skill to pick the right existing build or runtime entry point instead of inventing new commands.

## Build Selection Rules

1. Pick the entry point that matches the narrowest helper agent first, then widen only if multiple helpers are involved.
2. Native-only work should start from the CMake task or existing native coverage script.
3. ASP.NET work should reuse the `Build Banana API` task and explicit native/runtime env vars when running locally.
4. React work should use Bun scripts under `src/typescript/react`.
5. Cross-layer runtime work should prefer the compose scripts and profiles already defined in the repo.

## Runtime Rules

- Keep `BANANA_NATIVE_PATH` explicit for API work.
- Keep `BANANA_PG_CONNECTION` explicit for native-backed or integration flows.
- Use compose profiles when multiple services must move together.

## Resources

- Command matrix: [entry-points.md](./entry-points.md)
- Helper routing: [../banana-agent-decomposition/helper-matrix.md](../banana-agent-decomposition/helper-matrix.md)

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

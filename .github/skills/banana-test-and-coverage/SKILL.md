---
name: banana-test-and-coverage
description: Run Banana unit, integration, native, compose, and coverage validation with the smallest sufficient scope.
argument-hint: Describe the change and the level of confidence you need, such as native-only, API-only, or cross-layer.
---

# Banana Test And Coverage

Use this skill to select the correct validation surface for Banana changes.

## Validation Procedure

1. Identify the narrowest existing tests that cover the change.
2. Escalate to integration or aggregate coverage when contracts or runtime behavior change.
3. Make environment requirements explicit before running the command.
4. Report exact failing stage and likely helper owner, not just the final error.
5. Route fixes to the narrowest helper once the failure surface is isolated.

## Required Contracts

- Native PostgreSQL and integration flows require `BANANA_PG_CONNECTION`.
- Managed integration flows require `BANANA_NATIVE_PATH`.
- Compose validation should align with profiles in `docker-compose.yml`.

## Resources

- Validation matrix: [validation-matrix.md](./validation-matrix.md)
- Helper routing: [../banana-agent-decomposition/helper-matrix.md](../banana-agent-decomposition/helper-matrix.md)

## Shared Frontend Contract

- If a task touches src/typescript/react, src/typescript/electron, or src/typescript/shared/ui, keep shared primitives in @banana/ui instead of app-local thin re-export stubs.
- Reuse @banana/ui/tailwind/preset and @banana/ui/styles/tokens.css from consuming apps.
- Install dependencies in src/typescript/shared/ui before running app-level bun check/build flows.
- Reference .github/shared-typescript-ui.md for the full contract.

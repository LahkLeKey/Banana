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
4. Report exact failing stage, not just the final error.

## Required Contracts

- Native PostgreSQL and integration flows require `BANANA_PG_CONNECTION`.
- Managed integration flows require `BANANA_NATIVE_PATH`.
- Compose validation should align with profiles in `docker-compose.yml`.

## Resources

- Validation matrix: [validation-matrix.md](./validation-matrix.md)

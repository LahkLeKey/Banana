# Feature Specification: Mutation Testing Stryker Package Manager Fix

**Feature Branch**: `109-mutation-testing-stryker-package-manager-fix`
**Created**: 2026-05-02
**Status**: Follow-up stabilization scaffold
**Wave**: stabilization
**Domain**: workflow / typescript-api
**Depends on**: #085, #106, #107

## Problem Statement

The Mutation Testing workflow fails on `main` because Stryker rejects `packageManager: bun` (`allowed: npm, yarn, pnpm`). This causes deterministic lane failure before mutation execution and blocks mutation signal quality.

## In Scope *(mandatory)*

- Align Stryker package manager configuration to a supported value.
- Preserve Bun-first developer workflow while satisfying Stryker runtime constraints.
- Ensure mutation lane can install dependencies and execute on CI reproducibly.
- Add validation to prevent unsupported package manager values from re-entering CI.

## Out of Scope *(mandatory)*

- Replacing Stryker as mutation framework.
- Rewriting all TypeScript API test infrastructure.

## Success Criteria

- Mutation Testing workflow no longer fails with package manager validation error.
- Stryker executes mutation analysis end-to-end in CI.
- Configuration guardrails catch unsupported package manager settings pre-merge.

## Notes for the planner

- Keep Bun as primary repo package manager where possible; scope Stryker-specific compatibility narrowly.
- Record exact configuration rationale in the feature artifacts.

# Feature Specification: Compose CI Runtime Compatibility and Heredoc Stability

**Feature Branch**: `108-compose-ci-runtime-compat-and-heredoc-stability`
**Created**: 2026-05-02
**Status**: Follow-up stabilization scaffold
**Wave**: stabilization
**Domain**: workflow / testing / infra
**Depends on**: #106, #107

## Problem Statement

Post-merge Compose CI runs on `main` fail in two deterministic places: runtime compatibility reports `exceptions_registry_present=false` for `compose-ci.yml`, and .NET coverage fallback steps fail with shell heredoc termination errors (`wanted EOF`, `unexpected end of file`). These failures keep Compose CI red despite healthy lanes in other jobs.

## In Scope *(mandatory)*

- Restore runtime compatibility exception registry handling for `compose-ci.yml`.
- Repair heredoc-based artifact fallback snippets in Compose CI so fallback writes are syntactically safe.
- Add guardrails/tests that catch malformed fallback shell blocks before merge.
- Preserve existing lane result schema and artifact contracts.

## Out of Scope *(mandatory)*

- Reworking all Compose CI lane logic.
- Lowering any coverage threshold or bypassing coverage policy gates.

## Success Criteria

- `workflow-runtime-compatibility` passes in Compose CI on `main`.
- `dotnet-unit-tests-coverage` and `dotnet-integration-tests-coverage` no longer fail from shell syntax errors.
- Compose CI failure aggregate reflects only true lane failures, not workflow script syntax defects.

## Notes for the planner

- Keep fallback artifact generation deterministic under `if: always()` paths.
- Prefer script extraction or safe multiline patterns over fragile inline heredocs in YAML run blocks.

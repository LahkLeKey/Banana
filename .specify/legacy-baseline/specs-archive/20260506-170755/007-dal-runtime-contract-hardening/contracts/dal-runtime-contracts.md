# DAL Runtime Contracts

**Feature**: 007-dal-runtime-contract-hardening
**Date**: 2026-04-26

## Scope

This contract covers DAL runtime outcome behavior for profile query paths and associated managed mapping expectations.

## Native Outcome Contract

For DAL profile query execution in this feature scope:

1. Missing/empty `BANANA_PG_CONNECTION` MUST return deterministic non-success.
2. Dependency unavailable states (for example DAL linkage/runtime dependency not viable) MUST return deterministic non-success.
3. Query execution failures MUST return deterministic non-success.
4. Valid configured query success MUST preserve existing successful behavior compatibility.
5. Synthetic success payloads for non-viable DAL paths are prohibited.

## Managed Mapping Contract

Managed mapping in ASP.NET pipeline must:

1. Map each in-scope native DAL outcome deterministically to HTTP status and error body.
2. Preserve existing success-path contract behavior for valid successful DAL outcomes.
3. Include remediation guidance for operationally actionable non-success outcomes.

## Validation Contract

Required minimum validation lanes:

- `tests/native` for DAL category behavior
- Targeted `tests/unit` for status mapping and interop handling
- `tests/e2e` contract lane (`--filter "FullyQualifiedName~Contracts"`)

## Completion Criteria

This feature contract is satisfied when:

- All in-scope DAL non-success categories return deterministic non-success outcomes.
- Managed mapping is deterministic for each in-scope category.
- Required validation lanes pass with no contract regressions in scope.

## Execution Reconciliation (2026-04-26)

- Dependency-unavailable DAL fallback now returns explicit non-success (`BANANA_DB_ERROR`) and no longer emits synthetic success JSON.
- Managed and contract-lane assertions for `DbNotConfigured`/`DbError` mapping are passing in targeted test runs.
- Native lane remains pending due CMake Tools configuration blocker in this workspace.

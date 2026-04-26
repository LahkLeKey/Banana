# CI Runtime Compatibility

## Purpose

This page defines runtime-compatibility guardrails for JavaScript-based GitHub Actions in merge-gated workflows.

## Node Runtime Contract

- compose-ci workflow sets FORCE_JAVASCRIPT_ACTIONS_TO_NODE24=true.
- Runtime compatibility checks run in workflow and emit machine-readable artifacts.
- Runtime exception registry is tracked in .github/workflows/runtime-compatibility-exceptions.yml.

## Exception Policy

- Exceptions are temporary and should remain empty by default.
- Any exception entry must include owner and target remediation date.
- Exceptions should be removed once affected actions are updated or no longer used.

## Validation Output

- Runtime compatibility script writes JSON and text reports under .artifacts/compose-ci/runtime-compatibility.
- Reports are published as workflow artifacts and summary content.

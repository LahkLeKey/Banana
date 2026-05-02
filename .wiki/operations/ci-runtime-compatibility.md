<!-- breadcrumb: Operations > CI Runtime Compatibility -->

# CI Runtime Compatibility

> [Home](../Home.md) › [Operations](README.md) › CI Runtime Compatibility

Related pages: [CI and Compose Notes](ci-compose-notes.md), [Compose CI Stabilization](ci-compose-stabilization.md)

## What This Page Covers

How Banana tracks and enforces runtime compatibility for GitHub Actions execution.

## Current Rule

Compose CI opts into Node 24 for JavaScript actions to avoid Node 20 deprecation drift.

## How To Check Status

1. Open workflow artifact `workflow-runtime-compatibility`.
2. Read `runtime-compatibility.json` for pass/fail status.
3. If failed, inspect missing guardrails in `compose-ci.yml` or exception registry.

## Exception Handling

- Exception file: `.github/runtime-compatibility-exceptions.yml`
- Keep exceptions empty unless strictly needed.
- When adding one, include ownership and cleanup date.

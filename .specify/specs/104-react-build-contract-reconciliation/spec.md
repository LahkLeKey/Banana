# Feature Specification: React Build Contract Reconciliation

**Feature Branch**: `104-react-build-contract-reconciliation`
**Created**: 2026-05-01
**Status**: Follow-up stabilization stub
**Wave**: stabilization
**Domain**: react / frontend
**Depends on**: #075, #090, #091, #092

## Problem Statement

Post-bulk merges introduced React compile breakages in import contracts and dependencies. Current build errors include missing local module imports, missing `@sentry/react`, and API mismatch between exported and imported vitals symbols.

## In Scope *(mandatory)*

- Reconcile import paths and module boundaries for frontend hooks and API helpers.
- Align vitals helper exports with consuming entrypoint usage.
- Add required runtime and type dependencies for Sentry integration.
- Enforce a deterministic frontend build gate that blocks unresolved symbol drift.

## Out of Scope *(mandatory)*

- UI redesign or major route restructuring.
- New feature work not required to restore build stability.

## Success Criteria

- `bun run build` in React succeeds without TypeScript errors.
- Sentry integration compiles when enabled and no-ops safely when disabled.
- Contract tests or lint rules detect missing import/export symbols pre-merge.

## Notes for the planner

- Focus on preserving current behavior while fixing compile integrity.
- Validate both local dev and CI build paths.
- Keep changes bounded to stability and dependency correctness.

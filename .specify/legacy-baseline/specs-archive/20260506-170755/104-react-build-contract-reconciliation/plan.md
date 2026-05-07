# Implementation Plan: React Build Contract Reconciliation

**Branch**: `104-react-build-contract-reconciliation` | **Date**: 2026-05-02 | **Spec**: `.specify/specs/104-react-build-contract-reconciliation/spec.md`
**Input**: Feature specification from `.specify/specs/104-react-build-contract-reconciliation/spec.md`

## Summary

Restore deterministic React build integrity after bulk delivery by reconciling import/export contracts, stabilizing Sentry integration dependencies, and adding explicit diagnostics for rate-limit-related setup failures that can mask healthy code.

## Technical Context

**Language/Version**: TypeScript 5.x, Bun runtime
**Primary Dependencies**: React, Vite, `@sentry/react`, local shared UI package
**Storage**: N/A
**Testing**: `bun run build`, `bunx tsc --noEmit`, existing CI checks
**Target Platform**: GitHub Actions + local Git Bash on Windows
**Project Type**: Frontend (React)
**Performance Goals**: Keep compile gate under practical PR latency
**Constraints**: Preserve existing behavior; avoid UI-scope feature churn
**Scale/Scope**: `src/typescript/react` and related workflow/docs guardrails

## Constitution Check

- [x] Existing safety contracts are preserved.
- [x] No production deployment bypasses are introduced.
- [x] All changes are auditable by persisted artifacts and PR evidence.
- [x] GitHub/API-dependent automation has an explicit rate-limit resilience path and rerun guidance.

## Project Structure

```text
.specify/specs/104-react-build-contract-reconciliation/
src/typescript/react/
.github/workflows/
scripts/
```

## Phases

### Phase 1: Build Contract Alignment

**Goal**: Eliminate import/export and dependency drift causing compile failures.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Align frontend module import/export contracts | `src/typescript/react/src/**` | `bunx tsc --noEmit` succeeds |
| Reconcile Sentry dependency and optional behavior contract | `src/typescript/react/package.json`, runtime wiring files | `bun run build` succeeds with Sentry toggle semantics preserved |

### Phase 2: Gate and Diagnostic Hardening

**Goal**: Keep build gate deterministic and classify non-code failures.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Preserve deterministic frontend build gate in CI | `.github/workflows/*.yml` | build regressions fail consistently |
| Add rate-limit failure triage guidance for bootstrap/setup steps | relevant workflow docs/spec artifacts | 403 installation token failures are documented with rerun guidance |

### Phase 3: Validation

**Goal**: Confirm criteria and evidence are complete.

| Task | File(s) | Acceptance |
|------|---------|------------|
| Run React compile/build validation | `src/typescript/react` | `bunx tsc --noEmit` and `bun run build` pass |
| Verify CI classification guidance for rate-limit incidents | `.specify/specs/104-react-build-contract-reconciliation/tasks.md` | tasks include explicit recovery evidence path |

# Tasks: 104-react-build-contract-reconciliation

## T001 - Reconcile React import/export compile contracts

**Status**: Planned
**File(s)**: `src/typescript/react/src/**`
**Acceptance**: `bunx tsc --noEmit` reports zero unresolved module/type export errors.

---

## T002 - Stabilize Sentry dependency contract

**Status**: Planned
**File(s)**: `src/typescript/react/package.json`, `src/typescript/react/src/**`
**Acceptance**: `@sentry/react` integration compiles when enabled and no-op path remains safe when disabled.

---

## T003 - Validate deterministic React build gate

**Status**: Planned
**File(s)**: `src/typescript/react`, `.github/workflows/`
**Acceptance**: `bun run build` succeeds locally and CI surfaces the same failure mode for real compile regressions.

---

## T004 - Add rate-limit triage guidance for setup failures

**Status**: Planned
**File(s)**: `.specify/specs/104-react-build-contract-reconciliation/spec.md`, `.specify/specs/104-react-build-contract-reconciliation/plan.md`
**Acceptance**: Spec artifacts distinguish code regressions from transient 403 installation-token failures and document rerun strategy.

---

## T005 - Capture closure evidence

**Status**: Planned
**File(s)**: `.specify/specs/104-react-build-contract-reconciliation/tasks.md`
**Acceptance**: Task closure includes command outputs or run IDs for `bunx tsc --noEmit`, `bun run build`, and any rate-limit rerun evidence.

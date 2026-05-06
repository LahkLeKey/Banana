# Tasks: Functions Automation Safety Layer (178)

**Input**: Design documents from `.specify/specs/178-functions-automation-safety-layer/`
**Prerequisites**: `plan.md`, `spec.md`

## Phase 1: Guardrails

- [x] T001 Add capability discovery check and action enablement map.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

- [x] T002 Add per-action preflight validators.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

- [x] T003 Add dry-run toggle and execution path branching.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

## Phase 2: Diagnostics and Memory

- [x] T004 Add structured error panel with remediation rendering.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

- [x] T005 Add execution history persistence and restore on load.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

## Validation

- [x] T006 Run frontend type-check.
  Command: `bun run --cwd src/typescript/react tsc --noEmit`

- [x] T007 Validate Functions page behavior in browser.
  Checks: capability gate, preflight, dry-run, error panel, persisted history.

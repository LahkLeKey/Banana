# Tasks: Operator Command Center UX (176)

**Input**: Design documents from `.specify/specs/176-operator-command-center-ux/`
**Prerequisites**: `plan.md`, `spec.md`

## Phase 1: Foundations

- [x] T001 Add pinned sessions state model and local storage persistence.
  Files: `src/typescript/react/src/pages/OperatorPage.tsx`

- [x] T002 Add message search and role filter controls.
  Files: `src/typescript/react/src/pages/OperatorPage.tsx`

- [x] T003 Add token usage meter widget with fallback rendering.
  Files: `src/typescript/react/src/pages/OperatorPage.tsx`

## Phase 2: Recovery Actions

- [x] T004 Add retry failed turn action and replay payload wiring.
  Files: `src/typescript/react/src/pages/OperatorPage.tsx`

- [x] T005 Add quick command chips for common operator actions.
  Files: `src/typescript/react/src/pages/OperatorPage.tsx`

## Validation

- [x] T006 Run frontend type-check.
  Command: `bun run --cwd src/typescript/react tsc --noEmit`

- [x] T007 Verify operator flow in browser.
  Checks: pin/reload, search/filter, token meter, retry, quick chips.

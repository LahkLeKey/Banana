# Tasks: Release and Rollback Control Plane (180)

**Input**: Design documents from `.specify/specs/180-release-rollback-control-plane/`
**Prerequisites**: `plan.md`, `spec.md`

## Phase 1: Promotion Gates

- [x] T001 Add release checklist card state and rendering.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

- [x] T002 Add canary health gate and promotion block logic.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

## Phase 2: Recovery and Verification

- [x] T003 Add rollback playbook action with guided step display.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

- [x] T004 Add environment drift summary panel.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

- [x] T005 Add post-release verification matrix.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

## Validation

- [x] T006 Run frontend type-check.
  Command: `bun run --cwd src/typescript/react tsc --noEmit`

- [x] T007 Validate release control flows in browser.
  Checks: checklist gate, canary gate, rollback action, drift panel, verification matrix.

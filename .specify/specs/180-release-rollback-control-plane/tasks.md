# Tasks: Release and Rollback Control Plane (180)

**Input**: Design documents from `.specify/specs/180-release-rollback-control-plane/`
**Prerequisites**: `plan.md`, `spec.md`

## Phase 1: Promotion Gates

- [ ] T001 Add release checklist card state and rendering.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

- [ ] T002 Add canary health gate and promotion block logic.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

## Phase 2: Recovery and Verification

- [ ] T003 Add rollback playbook action with guided step display.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

- [ ] T004 Add environment drift summary panel.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

- [ ] T005 Add post-release verification matrix.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`

## Validation

- [ ] T006 Run frontend type-check.
  Command: `bun run --cwd src/typescript/react tsc --noEmit`

- [ ] T007 Validate release control flows in browser.
  Checks: checklist gate, canary gate, rollback action, drift panel, verification matrix.

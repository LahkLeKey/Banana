# Tasks: BananaAI Explainability Pack (179)

**Input**: Design documents from `.specify/specs/179-bananaai-explainability-pack/`
**Prerequisites**: `plan.md`, `spec.md`

## Phase 1: Explainability Rendering

- [x] T001 Add top feature attribution list in BananaAI verdict panel.
  Files: `src/typescript/react/src/pages/BananaAIPage.tsx`

- [x] T002 Add confidence calibration hint mapping.
  Files: `src/typescript/react/src/pages/BananaAIPage.tsx`

- [x] T003 Add ensemble lane contribution chart.
  Files: `src/typescript/react/src/pages/BananaAIPage.tsx`

## Phase 2: Auditability

- [x] T004 Add out-of-domain trigger reason rendering.
  Files: `src/typescript/react/src/pages/BananaAIPage.tsx`

- [x] T005 Add one-click copy for structured verdict JSON.
  Files: `src/typescript/react/src/pages/BananaAIPage.tsx`

## Validation

- [x] T006 Run frontend type-check.
  Command: `bun run --cwd src/typescript/react tsc --noEmit`

- [x] T007 Validate BananaAI behavior in browser.
  Checks: attribution, calibration hint, lane chart, OOD reasons, copy JSON.

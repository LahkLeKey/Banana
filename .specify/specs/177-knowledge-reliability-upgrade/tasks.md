# Tasks: Knowledge Reliability Upgrade (177)

**Input**: Design documents from `.specify/specs/177-knowledge-reliability-upgrade/`
**Prerequisites**: `plan.md`, `spec.md`

## Phase 1: Lane Reliability Surfaces

- [x] T001 Add lane health badge computation and rendering.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

- [x] T002 Add per-lane history sparkline component wiring.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

## Phase 2: Drill-in and Safety

- [x] T003 Add run details drawer and selection state.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

- [x] T004 Add stale-data warning banner by timestamp threshold.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

- [x] T005 Add copy/export action for visible metrics.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

## Validation

- [x] T006 Run frontend type-check.
  Command: `bun run --cwd src/typescript/react tsc --noEmit`

- [x] T007 Validate Knowledge page behavior in browser.
  Checks: badges, sparkline, drawer, stale banner, export.

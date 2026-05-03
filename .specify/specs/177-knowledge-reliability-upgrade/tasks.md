# Tasks: Knowledge Reliability Upgrade (177)

**Input**: Design documents from `.specify/specs/177-knowledge-reliability-upgrade/`
**Prerequisites**: `plan.md`, `spec.md`

## Phase 1: Lane Reliability Surfaces

- [ ] T001 Add lane health badge computation and rendering.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

- [ ] T002 Add per-lane history sparkline component wiring.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

## Phase 2: Drill-in and Safety

- [ ] T003 Add run details drawer and selection state.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

- [ ] T004 Add stale-data warning banner by timestamp threshold.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

- [ ] T005 Add copy/export action for visible metrics.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

## Validation

- [ ] T006 Run frontend type-check.
  Command: `bun run --cwd src/typescript/react tsc --noEmit`

- [ ] T007 Validate Knowledge page behavior in browser.
  Checks: badges, sparkline, drawer, stale banner, export.

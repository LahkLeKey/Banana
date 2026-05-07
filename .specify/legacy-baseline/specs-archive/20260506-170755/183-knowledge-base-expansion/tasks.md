# Tasks: Knowledge Base Expansion (183)

**Input**: `.specify/specs/183-knowledge-base-expansion/`

## Phase 1: Article Corpus and Filtering

- [x] T001 Expand ARTICLES corpus to ~12 entries across all four Spaces with category tags.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

- [x] T002 Add keyword search input and real-time filter state.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

- [x] T003 Make Spaces sidebar items act as category filters (selected state + All reset).
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

## Phase 2: Health Strip and Inline Preview

- [x] T004 Add live health strip fetching /health with status indicator and graceful fallback.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

- [x] T005 Add inline expand/collapse per article card showing full summary in-place.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`

## Validation

- [x] T006 Run frontend type-check.
  Command: `bun run --cwd src/typescript/react tsc --noEmit`

- [x] T007 Validate Knowledge page behavior in browser.
  Checks: search narrows, space filter works, health strip live, article expands inline.

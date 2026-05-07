# Issue Breakdown: Knowledge Reliability Upgrade (177)

Repository issue order derived from [tasks.md](tasks.md).

## Issue 1

Title: [177] Add lane health badges

Body:
Implement lane health badge computation and rendering on Knowledge page.

- Source Task: T001
- Depends On: None
- Files: src/typescript/react/src/pages/KnowledgePage.tsx

CLI:
`gh issue create --title "[177] Add lane health badges" --body "Implement lane health badge computation and rendering on Knowledge page.\n\nSource Task: T001\nDepends On: None\nFiles: src/typescript/react/src/pages/KnowledgePage.tsx"`

## Issue 2

Title: [177] Add lane history sparklines

Body:
Implement per-lane history sparkline rendering.

- Source Task: T002
- Depends On: Issue 1
- Files: src/typescript/react/src/pages/KnowledgePage.tsx

CLI:
`gh issue create --title "[177] Add lane history sparklines" --body "Implement per-lane history sparkline rendering.\n\nSource Task: T002\nDepends On: [177] Add lane health badges\nFiles: src/typescript/react/src/pages/KnowledgePage.tsx"`

## Issue 3

Title: [177] Add run details drawer

Body:
Add run selection state and inline run details drawer.

- Source Task: T003
- Depends On: Issue 2
- Files: src/typescript/react/src/pages/KnowledgePage.tsx

CLI:
`gh issue create --title "[177] Add run details drawer" --body "Add run selection state and inline run details drawer.\n\nSource Task: T003\nDepends On: [177] Add lane history sparklines\nFiles: src/typescript/react/src/pages/KnowledgePage.tsx"`

## Issue 4

Title: [177] Add stale-data warning banner

Body:
Show stale-data warning when history timestamp exceeds threshold.

- Source Task: T004
- Depends On: Issue 2
- Files: src/typescript/react/src/pages/KnowledgePage.tsx

CLI:
`gh issue create --title "[177] Add stale-data warning banner" --body "Show stale-data warning when history timestamp exceeds threshold.\n\nSource Task: T004\nDepends On: [177] Add lane history sparklines\nFiles: src/typescript/react/src/pages/KnowledgePage.tsx"`

## Issue 5

Title: [177] Add metrics copy-export action

Body:
Implement copy/export for visible Knowledge metrics.

- Source Task: T005
- Depends On: Issues 1-4
- Files: src/typescript/react/src/pages/KnowledgePage.tsx

CLI:
`gh issue create --title "[177] Add metrics copy-export action" --body "Implement copy/export for visible Knowledge metrics.\n\nSource Task: T005\nDepends On: Issues 1-4\nFiles: src/typescript/react/src/pages/KnowledgePage.tsx"`

## Issue 6

Title: [177] Run frontend type-check gate

Body:
Execute TypeScript type-check after Knowledge enhancements.

- Source Task: T006
- Depends On: Issues 1-5
- Command: bun run --cwd src/typescript/react tsc --noEmit

CLI:
`gh issue create --title "[177] Run frontend type-check gate" --body "Execute TypeScript type-check after Knowledge enhancements.\n\nSource Task: T006\nDepends On: Issues 1-5\nCommand: bun run --cwd src/typescript/react tsc --noEmit"`

## Issue 7

Title: [177] Verify Knowledge UX in browser

Body:
Validate badges, sparklines, drawer, stale banner, and export behavior.

- Source Task: T007
- Depends On: Issue 6

CLI:
`gh issue create --title "[177] Verify Knowledge UX in browser" --body "Validate badges, sparklines, drawer, stale banner, and export behavior.\n\nSource Task: T007\nDepends On: [177] Run frontend type-check gate"`

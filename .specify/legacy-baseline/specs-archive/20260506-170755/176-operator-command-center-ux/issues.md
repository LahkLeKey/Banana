# Issue Breakdown: Operator Command Center UX (176)

Repository issue order derived from [tasks.md](tasks.md).

## Issue 1

Title: [176] Add pinned sessions persistence

Body:
Implement pinned recent sessions state and local storage persistence on Operator page.

- Source Task: T001
- Depends On: None
- Files: src/typescript/react/src/pages/OperatorPage.tsx
- Acceptance:
  - Pinning a session adds it to a pinned list.
  - Pinned list restores after reload.

CLI:
`gh issue create --title "[176] Add pinned sessions persistence" --body "Implement pinned recent sessions state and local storage persistence on Operator page.\n\nSource Task: T001\nDepends On: None\nFiles: src/typescript/react/src/pages/OperatorPage.tsx\nAcceptance:\n- Pinning a session adds it to a pinned list.\n- Pinned list restores after reload."`

## Issue 2

Title: [176] Add message search and role filters

Body:
Add search input and role filter controls for Operator conversation turns.

- Source Task: T002
- Depends On: Issue 1
- Files: src/typescript/react/src/pages/OperatorPage.tsx
- Acceptance:
  - Search narrows visible turns by keyword.
  - Role filter narrows turns by role.

CLI:
`gh issue create --title "[176] Add message search and role filters" --body "Add search input and role filter controls for Operator conversation turns.\n\nSource Task: T002\nDepends On: [176] Add pinned sessions persistence\nFiles: src/typescript/react/src/pages/OperatorPage.tsx\nAcceptance:\n- Search narrows visible turns by keyword.\n- Role filter narrows turns by role."`

## Issue 3

Title: [176] Add token usage meter widget

Body:
Render token usage meter on active Operator session with safe fallback when token data is unavailable.

- Source Task: T003
- Depends On: Issue 1
- Files: src/typescript/react/src/pages/OperatorPage.tsx
- Acceptance:
  - Meter displays value when available.
  - Fallback state renders without errors.

CLI:
`gh issue create --title "[176] Add token usage meter widget" --body "Render token usage meter on active Operator session with safe fallback when token data is unavailable.\n\nSource Task: T003\nDepends On: [176] Add pinned sessions persistence\nFiles: src/typescript/react/src/pages/OperatorPage.tsx\nAcceptance:\n- Meter displays value when available.\n- Fallback state renders without errors."`

## Issue 4

Title: [176] Add retry failed turn action

Body:
Add retry button for failed turns and replay original payload.

- Source Task: T004
- Depends On: Issue 2
- Files: src/typescript/react/src/pages/OperatorPage.tsx
- Acceptance:
  - Failed turns expose retry action.
  - Retry uses original input and appends rerun result.

CLI:
`gh issue create --title "[176] Add retry failed turn action" --body "Add retry button for failed turns and replay original payload.\n\nSource Task: T004\nDepends On: [176] Add message search and role filters\nFiles: src/typescript/react/src/pages/OperatorPage.tsx\nAcceptance:\n- Failed turns expose retry action.\n- Retry uses original input and appends rerun result."`

## Issue 5

Title: [176] Add quick command chips

Body:
Add quick command chips for common operator actions.

- Source Task: T005
- Depends On: Issue 2
- Files: src/typescript/react/src/pages/OperatorPage.tsx
- Acceptance:
  - Clicking a chip dispatches the mapped action.
  - Chip actions do not break existing input flow.

CLI:
`gh issue create --title "[176] Add quick command chips" --body "Add quick command chips for common operator actions.\n\nSource Task: T005\nDepends On: [176] Add message search and role filters\nFiles: src/typescript/react/src/pages/OperatorPage.tsx\nAcceptance:\n- Clicking a chip dispatches the mapped action.\n- Chip actions do not break existing input flow."`

## Issue 6

Title: [176] Run frontend type-check gate

Body:
Execute TypeScript type-check after Operator enhancements.

- Source Task: T006
- Depends On: Issues 1-5
- Command: bun run --cwd src/typescript/react tsc --noEmit

CLI:
`gh issue create --title "[176] Run frontend type-check gate" --body "Execute TypeScript type-check after Operator enhancements.\n\nSource Task: T006\nDepends On: Issues 1-5\nCommand: bun run --cwd src/typescript/react tsc --noEmit"`

## Issue 7

Title: [176] Verify Operator UX in browser

Body:
Manually validate pin/reload, search/filter, token meter, retry, and command chips.

- Source Task: T007
- Depends On: Issue 6
- Acceptance:
  - All five enhancements behave correctly in browser validation.

CLI:
`gh issue create --title "[176] Verify Operator UX in browser" --body "Manually validate pin/reload, search/filter, token meter, retry, and command chips.\n\nSource Task: T007\nDepends On: [176] Run frontend type-check gate\nAcceptance:\n- All five enhancements behave correctly in browser validation."`

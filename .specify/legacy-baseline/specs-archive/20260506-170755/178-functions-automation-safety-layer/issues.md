# Issue Breakdown: Functions Automation Safety Layer (178)

Repository issue order derived from [tasks.md](tasks.md).

## Issue 1

Title: [178] Add capability discovery gating

Body:
Implement capability discovery check and action enablement map on Functions page.

- Source Task: T001
- Depends On: None
- Files: src/typescript/react/src/pages/FunctionsPage.tsx

CLI:
`gh issue create --title "[178] Add capability discovery gating" --body "Implement capability discovery check and action enablement map on Functions page.\n\nSource Task: T001\nDepends On: None\nFiles: src/typescript/react/src/pages/FunctionsPage.tsx"`

## Issue 2

Title: [178] Add per-action preflight validators

Body:
Add preflight validation per action invocation in Functions page.

- Source Task: T002
- Depends On: Issue 1
- Files: src/typescript/react/src/pages/FunctionsPage.tsx

CLI:
`gh issue create --title "[178] Add per-action preflight validators" --body "Add preflight validation per action invocation in Functions page.\n\nSource Task: T002\nDepends On: [178] Add capability discovery gating\nFiles: src/typescript/react/src/pages/FunctionsPage.tsx"`

## Issue 3

Title: [178] Add dry-run mode toggle

Body:
Add dry-run toggle and branch execution path away from mutating calls.

- Source Task: T003
- Depends On: Issue 2
- Files: src/typescript/react/src/pages/FunctionsPage.tsx

CLI:
`gh issue create --title "[178] Add dry-run mode toggle" --body "Add dry-run toggle and branch execution path away from mutating calls.\n\nSource Task: T003\nDepends On: [178] Add per-action preflight validators\nFiles: src/typescript/react/src/pages/FunctionsPage.tsx"`

## Issue 4

Title: [178] Add structured remediation error panel

Body:
Render normalized error payloads with remediation guidance.

- Source Task: T004
- Depends On: Issue 3
- Files: src/typescript/react/src/pages/FunctionsPage.tsx

CLI:
`gh issue create --title "[178] Add structured remediation error panel" --body "Render normalized error payloads with remediation guidance.\n\nSource Task: T004\nDepends On: [178] Add dry-run mode toggle\nFiles: src/typescript/react/src/pages/FunctionsPage.tsx"`

## Issue 5

Title: [178] Persist execution history locally

Body:
Persist and restore execution history from local storage.

- Source Task: T005
- Depends On: Issue 4
- Files: src/typescript/react/src/pages/FunctionsPage.tsx

CLI:
`gh issue create --title "[178] Persist execution history locally" --body "Persist and restore execution history from local storage.\n\nSource Task: T005\nDepends On: [178] Add structured remediation error panel\nFiles: src/typescript/react/src/pages/FunctionsPage.tsx"`

## Issue 6

Title: [178] Run frontend type-check gate

Body:
Execute TypeScript type-check after Functions safety enhancements.

- Source Task: T006
- Depends On: Issues 1-5
- Command: bun run --cwd src/typescript/react tsc --noEmit

CLI:
`gh issue create --title "[178] Run frontend type-check gate" --body "Execute TypeScript type-check after Functions safety enhancements.\n\nSource Task: T006\nDepends On: Issues 1-5\nCommand: bun run --cwd src/typescript/react tsc --noEmit"`

## Issue 7

Title: [178] Verify Functions safety UX in browser

Body:
Validate capability gate, preflight, dry-run, remediation panel, and persisted history.

- Source Task: T007
- Depends On: Issue 6

CLI:
`gh issue create --title "[178] Verify Functions safety UX in browser" --body "Validate capability gate, preflight, dry-run, remediation panel, and persisted history.\n\nSource Task: T007\nDepends On: [178] Run frontend type-check gate"`

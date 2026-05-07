# Issue Breakdown: Release and Rollback Control Plane (180)

Repository issue order derived from [tasks.md](tasks.md).

## Issue 1

Title: [180] Add release checklist card

Body:
Implement release checklist state and rendering in release control surface.

- Source Task: T001
- Depends On: None
- Files: src/typescript/react/src/pages/FunctionsPage.tsx

CLI:
`gh issue create --title "[180] Add release checklist card" --body "Implement release checklist state and rendering in release control surface.\n\nSource Task: T001\nDepends On: None\nFiles: src/typescript/react/src/pages/FunctionsPage.tsx"`

## Issue 2

Title: [180] Add canary health gate

Body:
Implement canary health gate and promotion block logic.

- Source Task: T002
- Depends On: Issue 1
- Files: src/typescript/react/src/pages/FunctionsPage.tsx

CLI:
`gh issue create --title "[180] Add canary health gate" --body "Implement canary health gate and promotion block logic.\n\nSource Task: T002\nDepends On: [180] Add release checklist card\nFiles: src/typescript/react/src/pages/FunctionsPage.tsx"`

## Issue 3

Title: [180] Add rollback playbook action

Body:
Add rollback action with ordered guided steps and status presentation.

- Source Task: T003
- Depends On: Issue 2
- Files: src/typescript/react/src/pages/FunctionsPage.tsx

CLI:
`gh issue create --title "[180] Add rollback playbook action" --body "Add rollback action with ordered guided steps and status presentation.\n\nSource Task: T003\nDepends On: [180] Add canary health gate\nFiles: src/typescript/react/src/pages/FunctionsPage.tsx"`

## Issue 4

Title: [180] Add environment drift summary panel

Body:
Render environment drift summary across release inputs.

- Source Task: T004
- Depends On: Issue 2
- Files: src/typescript/react/src/pages/FunctionsPage.tsx

CLI:
`gh issue create --title "[180] Add environment drift summary panel" --body "Render environment drift summary across release inputs.\n\nSource Task: T004\nDepends On: [180] Add canary health gate\nFiles: src/typescript/react/src/pages/FunctionsPage.tsx"`

## Issue 5

Title: [180] Add post-release verification matrix

Body:
Add matrix view for post-release verification checks and outcomes.

- Source Task: T005
- Depends On: Issues 1-4
- Files: src/typescript/react/src/pages/FunctionsPage.tsx

CLI:
`gh issue create --title "[180] Add post-release verification matrix" --body "Add matrix view for post-release verification checks and outcomes.\n\nSource Task: T005\nDepends On: Issues 1-4\nFiles: src/typescript/react/src/pages/FunctionsPage.tsx"`

## Issue 6

Title: [180] Run frontend type-check gate

Body:
Execute TypeScript type-check after release control enhancements.

- Source Task: T006
- Depends On: Issues 1-5
- Command: bun run --cwd src/typescript/react tsc --noEmit

CLI:
`gh issue create --title "[180] Run frontend type-check gate" --body "Execute TypeScript type-check after release control enhancements.\n\nSource Task: T006\nDepends On: Issues 1-5\nCommand: bun run --cwd src/typescript/react tsc --noEmit"`

## Issue 7

Title: [180] Verify release control UX in browser

Body:
Validate checklist gate, canary block, rollback action, drift summary, and verification matrix.

- Source Task: T007
- Depends On: Issue 6

CLI:
`gh issue create --title "[180] Verify release control UX in browser" --body "Validate checklist gate, canary block, rollback action, drift summary, and verification matrix.\n\nSource Task: T007\nDepends On: [180] Run frontend type-check gate"`

# Issue Breakdown: BananaAI Explainability Pack (179)

Repository issue order derived from [tasks.md](tasks.md).

## Issue 1

Title: [179] Add top feature attribution list

Body:
Add ranked top feature attribution rendering in BananaAI verdict panel.

- Source Task: T001
- Depends On: None
- Files: src/typescript/react/src/pages/BananaAIPage.tsx

CLI:
`gh issue create --title "[179] Add top feature attribution list" --body "Add ranked top feature attribution rendering in BananaAI verdict panel.\n\nSource Task: T001\nDepends On: None\nFiles: src/typescript/react/src/pages/BananaAIPage.tsx"`

## Issue 2

Title: [179] Add confidence calibration hint

Body:
Map confidence ranges to qualitative calibration hints.

- Source Task: T002
- Depends On: Issue 1
- Files: src/typescript/react/src/pages/BananaAIPage.tsx

CLI:
`gh issue create --title "[179] Add confidence calibration hint" --body "Map confidence ranges to qualitative calibration hints.\n\nSource Task: T002\nDepends On: [179] Add top feature attribution list\nFiles: src/typescript/react/src/pages/BananaAIPage.tsx"`

## Issue 3

Title: [179] Add ensemble lane contribution chart

Body:
Render per-lane contribution chart in BananaAI verdict details.

- Source Task: T003
- Depends On: Issue 1
- Files: src/typescript/react/src/pages/BananaAIPage.tsx

CLI:
`gh issue create --title "[179] Add ensemble lane contribution chart" --body "Render per-lane contribution chart in BananaAI verdict details.\n\nSource Task: T003\nDepends On: [179] Add top feature attribution list\nFiles: src/typescript/react/src/pages/BananaAIPage.tsx"`

## Issue 4

Title: [179] Add out-of-domain trigger reasons

Body:
Render normalized out-of-domain trigger reasons when present.

- Source Task: T004
- Depends On: Issue 3
- Files: src/typescript/react/src/pages/BananaAIPage.tsx

CLI:
`gh issue create --title "[179] Add out-of-domain trigger reasons" --body "Render normalized out-of-domain trigger reasons when present.\n\nSource Task: T004\nDepends On: [179] Add ensemble lane contribution chart\nFiles: src/typescript/react/src/pages/BananaAIPage.tsx"`

## Issue 5

Title: [179] Add one-click verdict JSON copy

Body:
Add one-click copy action for structured verdict JSON payload.

- Source Task: T005
- Depends On: Issue 4
- Files: src/typescript/react/src/pages/BananaAIPage.tsx

CLI:
`gh issue create --title "[179] Add one-click verdict JSON copy" --body "Add one-click copy action for structured verdict JSON payload.\n\nSource Task: T005\nDepends On: [179] Add out-of-domain trigger reasons\nFiles: src/typescript/react/src/pages/BananaAIPage.tsx"`

## Issue 6

Title: [179] Run frontend type-check gate

Body:
Execute TypeScript type-check after BananaAI explainability enhancements.

- Source Task: T006
- Depends On: Issues 1-5
- Command: bun run --cwd src/typescript/react tsc --noEmit

CLI:
`gh issue create --title "[179] Run frontend type-check gate" --body "Execute TypeScript type-check after BananaAI explainability enhancements.\n\nSource Task: T006\nDepends On: Issues 1-5\nCommand: bun run --cwd src/typescript/react tsc --noEmit"`

## Issue 7

Title: [179] Verify BananaAI explainability UX in browser

Body:
Validate attribution, calibration hint, lane chart, OOD reasons, and JSON copy.

- Source Task: T007
- Depends On: Issue 6

CLI:
`gh issue create --title "[179] Verify BananaAI explainability UX in browser" --body "Validate attribution, calibration hint, lane chart, OOD reasons, and JSON copy.\n\nSource Task: T007\nDepends On: [179] Run frontend type-check gate"`

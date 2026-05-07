# Issue Breakdown: JupyterLite Runtime Reliability (182)

Repository issue order derived from [tasks.md](tasks.md).

## Issue 1

Title: [182] Audit JupyterLite load path and static asset assumptions

Body:
Audit and document the current JupyterLite load path, iframe URL, and public asset references in DataSciencePage.tsx and the starter notebook. Record any broken or stale path assumptions.

- Source Task: T001
- Depends On: None
- Files: src/typescript/react/src/pages/DataSciencePage.tsx, src/typescript/react/public/notebooks/banana-ds-starter.ipynb

CLI:
`gh issue create --title "[182] Audit JupyterLite load path and static asset assumptions" --body "Audit and document the current JupyterLite load path, iframe URL, and public asset references in DataSciencePage.tsx and the starter notebook. Record any broken or stale path assumptions.\n\nSource Task: T001\nDepends On: None\nFiles: src/typescript/react/src/pages/DataSciencePage.tsx, src/typescript/react/public/notebooks/banana-ds-starter.ipynb"`

## Issue 2

Title: [182] Fix Data Science page embed config and error state handling

Body:
Patch DataSciencePage.tsx to correctly configure the JupyterLite embed and add robust load/error state management so failures are caught and surfaced to the user rather than rendering a blank or broken iframe.

- Source Task: T002
- Depends On: Issue 1
- Files: src/typescript/react/src/pages/DataSciencePage.tsx

CLI:
`gh issue create --title "[182] Fix Data Science page embed config and error state handling" --body "Patch DataSciencePage.tsx to correctly configure the JupyterLite embed and add robust load/error state management so failures are caught and surfaced to the user rather than rendering a blank or broken iframe.\n\nSource Task: T002\nDepends On: [182] Audit JupyterLite load path and static asset assumptions\nFiles: src/typescript/react/src/pages/DataSciencePage.tsx"`

## Issue 3

Title: [182] Add actionable fallback controls for JupyterLite load failure

Body:
Add retry button, direct open-in-JupyterLite link, and troubleshooting hints to the Data Science page for when the embedded runtime fails to load.

- Source Task: T003
- Depends On: Issue 2
- Files: src/typescript/react/src/pages/DataSciencePage.tsx

CLI:
`gh issue create --title "[182] Add actionable fallback controls for JupyterLite load failure" --body "Add retry button, direct open-in-JupyterLite link, and troubleshooting hints to the Data Science page for when the embedded runtime fails to load.\n\nSource Task: T003\nDepends On: [182] Fix Data Science page embed config and error state handling\nFiles: src/typescript/react/src/pages/DataSciencePage.tsx"`

## Issue 4

Title: [182] Validate starter notebook path contract and first-open discoverability

Body:
Ensure the starter notebook asset path is correct and consistently referenced from the Data Science UI. Fix any path discrepancies causing the notebook to fail to open on first load.

- Source Task: T004
- Depends On: Issues 1, 2
- Files: src/typescript/react/src/pages/DataSciencePage.tsx, src/typescript/react/public/notebooks/banana-ds-starter.ipynb

CLI:
`gh issue create --title "[182] Validate starter notebook path contract and first-open discoverability" --body "Ensure the starter notebook asset path is correct and consistently referenced from the Data Science UI. Fix any path discrepancies causing the notebook to fail to open on first load.\n\nSource Task: T004\nDepends On: [182] Audit JupyterLite load path and static asset assumptions, [182] Fix Data Science page embed config and error state handling\nFiles: src/typescript/react/src/pages/DataSciencePage.tsx, src/typescript/react/public/notebooks/banana-ds-starter.ipynb"`

## Issue 5

Title: [182] Add browser smoke test for JupyterLite readiness and notebook open flow

Body:
Add a browser smoke test that loads /data-science, waits for JupyterLite ready signal, and opens the starter notebook to confirm content renders in the editor.

- Source Task: T005
- Depends On: Issues 2, 4
- Files: tests/e2e/**/* or frontend runtime validation helper scripts

CLI:
`gh issue create --title "[182] Add browser smoke test for JupyterLite readiness and notebook open flow" --body "Add a browser smoke test that loads /data-science, waits for JupyterLite ready signal, and opens the starter notebook to confirm content renders in the editor.\n\nSource Task: T005\nDepends On: [182] Fix Data Science page embed config and error state handling, [182] Validate starter notebook path contract and first-open discoverability\nFiles: tests/e2e/**/* or frontend runtime validation helper scripts"`

## Issue 6

Title: [182] Add smoke check for baseline code-cell execution in JupyterLite

Body:
Add a focused runtime check that confirms a baseline Python code cell executes successfully in JupyterLite without kernel crash or timeout. Used to gate notebook runtime regressions.

- Source Task: T006
- Depends On: Issue 5
- Files: tests/e2e/**/* or Data Science validation helper scripts

CLI:
`gh issue create --title "[182] Add smoke check for baseline code-cell execution in JupyterLite" --body "Add a focused runtime check that confirms a baseline Python code cell executes successfully in JupyterLite without kernel crash or timeout. Used to gate notebook runtime regressions.\n\nSource Task: T006\nDepends On: [182] Add browser smoke test for JupyterLite readiness and notebook open flow\nFiles: tests/e2e/**/* or Data Science validation helper scripts"`

## Issue 7

Title: [182] Run frontend type-check and build gate for Data Science page

Body:
Run bun tsc --noEmit and bun build for the React app after all Data Science page patches. Confirm zero type errors and clean production output.

- Source Task: T007
- Depends On: Issues 2, 3, 4
- Files: src/typescript/react/src/pages/DataSciencePage.tsx

CLI:
`gh issue create --title "[182] Run frontend type-check and build gate for Data Science page" --body "Run bun tsc --noEmit and bun build for the React app after all Data Science page patches. Confirm zero type errors and clean production output.\n\nSource Task: T007\nDepends On: Issues 2-4\nFiles: src/typescript/react/src/pages/DataSciencePage.tsx"`

## Closure Status (2026-05-03)

- #845 closed — implemented in `tests/e2e/Contracts/JupyterLiteRuntimeContractTests.cs` (`StarterNotebookPathExistsAndIsDiscoverable`)
- #846 closed — implemented in `tests/e2e/Contracts/JupyterLiteRuntimeContractTests.cs` (`DataSciencePageContainsJupyterLiteReadinessAndRetryFlowContracts`)
- #847 closed — implemented in `tests/e2e/Contracts/JupyterLiteRuntimeContractTests.cs` (`DataSciencePageContainsBaselineCodeCellExecutionContracts`)

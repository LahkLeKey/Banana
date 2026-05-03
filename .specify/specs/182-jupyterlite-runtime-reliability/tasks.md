# Tasks: JupyterLite Runtime Reliability (182)

**Input**: Design documents from `.specify/specs/182-jupyterlite-runtime-reliability/`
**Prerequisites**: `plan.md`, `spec.md`

## Phase 1: Runtime Path and UX Hardening

- [ ] T001 Audit and document current JupyterLite load path, iframe URL, and public asset assumptions.
  Files: `src/typescript/react/src/pages/DataSciencePage.tsx`, `src/typescript/react/public/notebooks/banana-ds-starter.ipynb`

- [ ] T002 Fix Data Science page embed config and add robust load/error state handling.
  Files: `src/typescript/react/src/pages/DataSciencePage.tsx`

- [ ] T003 Add actionable fallback controls (retry, open direct JupyterLite workspace, troubleshooting hints).
  Files: `src/typescript/react/src/pages/DataSciencePage.tsx`

## Phase 2: Notebook and Execution Reliability

- [ ] T004 Validate starter notebook path contract and ensure first-open discoverability from Data Science UI.
  Files: `src/typescript/react/src/pages/DataSciencePage.tsx`, `src/typescript/react/public/notebooks/banana-ds-starter.ipynb`

- [ ] T005 Add browser smoke validation for JupyterLite readiness and notebook open flow.
  Files: `tests/e2e/**/*` or existing frontend runtime checks

- [ ] T006 Add focused runtime check for baseline code-cell execution signal.
  Files: `tests/e2e/**/*` or Data Science validation helper scripts

## Validation

- [ ] T007 Run frontend type-check and build.
  Commands: `bun run --cwd src/typescript/react tsc --noEmit`, `bun run --cwd src/typescript/react build`

- [ ] T008 Validate Data Science page behavior in browser.
  Checks: JupyterLite ready, starter notebook opens, fallback controls work when load fails.

- [ ] T009 Validate production path.
  Checks: `https://banana.engineer/data-science` loads JupyterLite and notebook starter path resolves.

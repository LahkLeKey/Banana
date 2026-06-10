# Tasks: Notebook MMO Gameplay Client

**Input**: `.specify/specs/033-abi-notebook-playground/spec.md`

**Prerequisites**: spec.md, plan.md

## Phase 1: Setup

- [ ] T001 Confirm `.specify/feature.json` points to `.specify/specs/033-abi-notebook-playground`.
- [ ] T002 Recreate feature docs: `spec.md`, `plan.md`, `tasks.md`, `quickstart.md`, `contracts/`.
- [ ] T003 Validate notebook generation baseline with `bash scripts/scaffold-abi-notebook-workflow.sh`.

## Phase 2: Foundation

- [ ] T004 Create shared notebook client component directory under `src/typescript/react/src/components/notebook-client/`.
- [ ] T005 Add typed data models/hooks for manifest/notebook/session state under `src/typescript/react/src/lib/`.
- [ ] T006 Add API orchestration abstraction (fetchers + fallback adapter) for notebook gameplay bootstrap.

## Phase 3: User Story 1 - Main Menu Gameplay Shell (P1)

- [ ] T007 [US1] Implement `MainMenuPanel` component with launch CTA and readiness status.
- [ ] T008 [US1] Implement `NotebookGameplaySurface` component (file-selected code render view).
- [ ] T009 [US1] Wire `/` route to new orchestrator container that composes main menu + gameplay surface.
- [ ] T010 [US1] Add missing-asset fallback state with regeneration command guidance.

## Phase 4: User Story 2 - Shared Component Refactor (P2)

- [ ] T011 [US2] Extract `NotebookStatsCards` component.
- [ ] T012 [US2] Extract `NotebookFileExplorer` component with filter/search behavior.
- [ ] T013 [US2] Extract `NotebookHealthPanel` component for manifest/notebook/API status.
- [ ] T014 [US2] Keep page container slim by delegating state orchestration to custom hooks.

## Phase 5: User Story 3 - API Orchestration Integration (P3)

- [ ] T015 [US3] Scaffold API client methods for notebook-menu orchestration metadata.
- [ ] T016 [US3] Integrate API-first load path with local `/notebooks/*` fallback.
- [ ] T017 [US3] Add integration-safe retries/timeouts and explicit status messaging.

## Phase 6: Validation & Polish

- [ ] T018 Run `bun run build` in `src/typescript/react` and capture output.
- [ ] T019 Validate integrated browser flow for menu -> notebook gameplay interactions.
- [ ] T020 Validate notebook endpoints (`/notebooks/catalog-index.json`, `/notebooks/native-c-catalog.ipynb`) in dashboard mode.
- [ ] T021 Update quickstart and contract docs to reflect final shared-component architecture.

# Tasks: ABI Notebook Playground Pivot

**Input**: Design documents from `.specify/specs/033-abi-notebook-playground/`

**Prerequisites**: plan.md, spec.md

## Phase 1: Setup

- [ ] T001 Create artifact output directory for generated notebooks at `artifacts/notebooks/`.
- [ ] T002 Add `scripts/export-native-c-to-notebook.py` to generate consolidated notebook JSON from native C/H files.
- [ ] T003 Add `scripts/scaffold-abi-notebook-workflow.sh` wrapper to run the generator with sensible defaults.

## Phase 2: Foundational

- [ ] T004 Wire `.specify/feature.json` to `.specify/specs/033-abi-notebook-playground`.
- [ ] T005 Define notebook scaffold contract in `.specify/specs/033-abi-notebook-playground/contracts/abi-notebook-workflow.md`.

## Phase 3: User Story 1 - Native ABI Notebook Export (P1)

**Goal**: Enable interactive prototyping by exporting native C sources to a Jupyter notebook artifact.

**Independent Test**: Run `bash scripts/scaffold-abi-notebook-workflow.sh` and verify `artifacts/notebooks/native-c-catalog.ipynb` is generated with code cells for selected C/H files.

- [ ] T006 [US1] Implement source discovery and deterministic ordering in `scripts/export-native-c-to-notebook.py`.
- [ ] T007 [US1] Implement notebook JSON writer with markdown index + per-file code cells in `scripts/export-native-c-to-notebook.py`.
- [ ] T008 [US1] Add truncation policy and summary metadata in generated notebook output.

## Phase 4: User Story 2 - Frontend Playground Pivot (P2)

**Goal**: Replace marketing-first root page with data-science playground entry.

**Independent Test**: Run React smoke import and verify root route points to data science playground page.

- [ ] T009 [US2] Add `DataSciencePlaygroundPage` in `src/typescript/react/src/pages/DataSciencePlaygroundPage.tsx`.
- [ ] T010 [US2] Update root route in `src/typescript/react/src/lib/router.tsx` to point at the new page.
- [ ] T011 [US2] Preserve access to existing routes (`/marketing`, `/download`, `/login`, `/session-room`).

## Phase 5: Polish

- [ ] T012 Document generation and validation steps in `.specify/specs/033-abi-notebook-playground/quickstart.md`.
- [ ] T013 Validate native build and focused smoke tests after merge reconciliation.
- [ ] T014 Validate React import smoke and capture command evidence for branch PR notes.

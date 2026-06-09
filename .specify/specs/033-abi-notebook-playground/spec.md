# Feature Specification: ABI Notebook Playground Pivot

**Feature Branch**: `033-abi-notebook-playground`

**Created**: 2026-06-08

**Status**: Draft

**Input**: User description: "lock this refactor in and lets scaffold a new abi workflow, we want to load all of our c into jupytr notebooks for interactive prototyping instead of rushing a half baked dx12 client, our frontend deployment on vercel and be a data science playground instead of a marketing playground for now use specifiy to implement this on a new branch after you have merged all of this to main"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Export Native C Into Notebooks (Priority: P1)

As an engine/runtime contributor, I need a deterministic workflow that converts native C and header files into a notebook artifact so I can prototype and inspect runtime behavior interactively without depending on the DX12 client loop.

**Why this priority**: This is the core capability request and unlocks immediate exploratory development for native ABI changes.

**Independent Test**: Run `bash scripts/scaffold-abi-notebook-workflow.sh` and verify `artifacts/notebooks/native-c-catalog.ipynb` is generated with an index and per-file code cells.

**Acceptance Scenarios**:

1. **Given** native C files exist under `src/native`, **When** the scaffold script runs, **Then** a notebook artifact is generated with deterministic file ordering.
2. **Given** a file exceeds max lines per cell, **When** export runs, **Then** content is truncated with a clear marker instead of failing generation.

---

### User Story 2 - Vercel Root Becomes Data Science Playground (Priority: P2)

As a web user, I need the root frontend route to position Banana as a data science playground so the deployment reflects the temporary strategic focus rather than marketing copy.

**Why this priority**: The website messaging pivot is requested now and should not block the notebook workflow foundation.

**Independent Test**: Build/import-smoke the React app and verify `/` renders the new playground page while existing routes remain reachable.

**Acceptance Scenarios**:

1. **Given** I open `/` on the web app, **When** the router resolves the root path, **Then** I see data science playground messaging and notebook workflow links.
2. **Given** I open `/marketing`, **When** routing resolves, **Then** the prior marketing page remains accessible for fallback.

---

### Edge Cases

- Source-root path missing: script exits non-zero with actionable guidance.
- No `.c`/`.h` files discovered: generation aborts with explicit error.
- Extremely large files: exporter truncates per configurable line cap and marks truncation.
- Frontend deploy without notebook artifacts: page still renders and provides generation command fallback.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST provide a scriptable workflow that exports `src/native/**/*.c` and `src/native/**/*.h` into a Jupyter notebook artifact.
- **FR-002**: Export workflow MUST generate deterministic output order for reproducibility.
- **FR-003**: Export workflow MUST include a top-level markdown index cell listing source paths included in the notebook.
- **FR-004**: Export workflow MUST support per-file truncation with an explicit truncation marker.
- **FR-005**: React root route (`/`) MUST render a data science playground page instead of the marketing page.
- **FR-006**: Existing non-root routes (`/marketing`, `/download`, `/login`, `/session-room`) MUST remain available.
- **FR-007**: `.specify/feature.json` MUST point to this feature directory during implementation.

### Key Entities *(include if feature involves data)*

- **NativeNotebookExportManifest**: Source-root, discovered file list, line cap, generated timestamp, output path.
- **NotebookCellDescriptor**: Cell type, language metadata, source path label, emitted source lines.
- **PlaygroundLandingModel**: Frontend copy + canonical links to notebook artifacts and generation commands.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Running the scaffold script produces `artifacts/notebooks/native-c-catalog.ipynb` in under 30 seconds on current repo size.
- **SC-002**: Generated notebook contains at least one code cell for every discovered native source file up to configured cap.
- **SC-003**: React root route change compiles and preserves existing non-root route navigation.
- **SC-004**: Native build and focused feedback-loop smoke tests pass after integration changes.

## Assumptions

- Python 3 is available in contributor environments where notebook generation runs.
- Notebook generation is artifact-oriented and does not execute kernels as part of this feature.
- Marketing page code is retained as a fallback route, not deleted.
- Vercel continues serving static artifacts with existing rewrite/header behavior.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: This pivot affects web messaging only; no Steam storefront claim changes are introduced in this feature.
- **Cross-Domain Contracts**: Touches native source discovery (read-only), script orchestration, and React route composition.
- **Quality Gates**: Native compile and focused smoke tests plus React smoke import and notebook generation command.
- **Delivery Evidence**: Generated notebook artifact path and terminal evidence from build/test/generation commands.

## Storefront & Release Claims *(required when public Steam copy is affected)*

- Not applicable for this feature scope.

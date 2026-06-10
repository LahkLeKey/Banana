# Contract: Notebook Client Orchestration

## Objective

Define shared UI/component and data orchestration boundaries for notebook-first gameplay client behavior.

## UI Boundaries

- `MainMenuPanel`: launch actions, readiness state, high-level client controls.
- `NotebookStatsCards`: summary metrics (indexed files, cells, generated timestamp).
- `NotebookFileExplorer`: filter/search/select list for indexed source files.
- `NotebookGameplaySurface`: selected notebook cell code rendering and interaction shell.
- `NotebookHealthPanel`: API/local fallback status and actionable diagnostics.

## Data Contracts

### Manifest Payload

Source: `/notebooks/catalog-index.json` (fallback) and future API endpoint.

Required fields:

- `generated_at_utc`
- `source_root`
- `notebook_path`
- `file_count`
- `max_lines_per_file`
- `files[]`

### Notebook Payload

Source: `/notebooks/native-c-catalog.ipynb` (fallback) and future API endpoint.

Required behavior:

- Must include `cells[]` where code cells may start with `// source: <path>`.
- Runtime indexer maps source path -> code content for gameplay renderer panel.

## API Orchestration Hooks

- `fetchNotebookManifest()`
- `fetchNotebookDocument()`
- `fetchGameplayMenuState()`

Fallback rules:

1. Try API source first when configured.
2. On timeout/unavailable, use static `/notebooks/*` files.
3. Surface fallback status in `NotebookHealthPanel`.

## Acceptance Gate

- Shared components compile independently with typed props.
- Main menu launch flow works with both API-backed and static-fallback data.
- No raw JSON iframe is required for core notebook gameplay rendering.

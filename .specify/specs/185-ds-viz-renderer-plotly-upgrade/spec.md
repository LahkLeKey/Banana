# Feature Specification: DS Viz Renderer Plotly Upgrade (185)

**Feature Branch**: `171-ds-model-ops-complexity-scale-up`
**Created**: 2026-05-03
**Status**: In Progress
**Input**: Data Science tab is stuck at 90/10. Execution works; output rendering is primitive. Fix the last 10%.

## Problem Statement

The notebook runner executes Python via Pyodide correctly but renders charts as CSS bars and raw SVG — not real visualizations. Markdown cells show raw text. The output area lacks visual structure. The result feels like a broken prototype rather than a usable DS workbench.

## Root Cause

- `BANANA_VIZ::` renders via a hand-rolled CSS div/SVG renderer — no axes, no hover, no real chart behavior.
- Markdown cells render source text in a textarea even in read mode — not rendered HTML.
- No execution count on cells — no sense of notebook state.
- The viz protocol (`type: bar|line, x, y`) is too limited — no scatter, no multi-series, no layout control.

## In Scope

- E1: Replace custom chart renderer with Plotly.js (CDN-loaded, no new npm deps).
- E2: Upgrade `BANANA_VIZ::` protocol to accept full Plotly figure JSON (`{data, layout}`).
- E3: Keep `BANANA_TABLE::` intact; add `BANANA_PLOTLY::` as the preferred full-fidelity variant.
- E4: Markdown cells render as HTML (toggle edit ↔ preview per cell).
- E5: Cell output area styled clearly: dark/muted background, execution count badge, collapsible.
- E6: Update notebook 07 scaffold cells to emit valid Plotly JSON.

## Out of Scope

- Server-backed Jupyter kernel.
- nbformat MIME bundle full spec (overkill for this surface).
- Syntax highlighting (separate spike).

## Success Criteria

- SC-001: Running a bar chart cell renders a real Plotly chart (axes, hover, responsive).
- SC-002: Running a line chart cell renders a real Plotly line chart.
- SC-003: Markdown cells show rendered HTML in preview mode; double-click returns to edit.
- SC-004: Each executed cell shows an execution count `[1]`, `[2]`, etc.
- SC-005: Notebook 07 cells all produce visible charts/tables on Run All.

## Assumptions

- Plotly.js is loaded from CDN (`cdn.plot.ly`) on first chart render; no new npm dependencies are added.
- The React app already bundles Pyodide; this feature adds only a lazy CDN script tag for Plotly.
- `BANANA_VIZ::` compat shim is preserved so older notebooks continue to produce output.
- Markdown→HTML transform is inline (no external parsing library); basic syntax (headers, bold, code, bullets) is sufficient for notebook prose.
- Notebook 07 is the canonical validation surface; other notebooks are not in scope for cell rewrites.

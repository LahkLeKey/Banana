# Implementation Plan: DS Viz Renderer Plotly Upgrade (185)

**Branch**: `171-ds-model-ops-complexity-scale-up` | **Date**: 2026-05-03 | **Spec**: [spec.md](spec.md)

## Overview

Eliminate the 90/10 quality gap in the Data Science notebook tab. Execution already works. This plan upgrades output rendering from primitive CSS/SVG to real Plotly charts, adds markdown preview, and polishes cell UX.

## Architecture

The renderer upgrade follows the same CDN-lazy-load pattern already used for Pyodide:

```
window.Plotly  ← loaded from cdn.plot.ly once on first chart render
PlotlyChart    ← React component: useRef div + Plotly.newPlot/react
parseRichOutput← extended: BANANA_PLOTLY:: → {kind:"plotly", figure:{data,layout}}
                            BANANA_VIZ::   → compat shim → Plotly figure
```

Markdown preview:
```
cell.previewMode: boolean (per-cell UI state, not persisted)
onClick on rendered preview → exits to edit textarea
```

## Implementation Sequence

### Step 1 — Plotly CDN load + type stub
- Add `PLOTLY_SCRIPT = "https://cdn.plot.ly/plotly-3.0.0.min.js"` constant
- Add `window.Plotly` type stub (minimal: `newPlot`, `react`, `purge`)
- Add `ensurePlotly()` loader function (same lazy-script pattern as `ensureRuntime`)

### Step 2 — PlotlyChart component (inline, no separate file)
- Render into a `useRef<HTMLDivElement>` via `Plotly.newPlot` on mount
- `Plotly.react` on figure update (avoids full re-render)
- `Plotly.purge` on unmount
- Responsive: pass `{responsive: true}` config
- Height: `h-64` default, matches current chart area

### Step 3 — Upgrade parseRichOutput
- Add `BANANA_PLOTLY::` prefix → `{kind: "plotly", figure: {data, layout}}`
- Keep `BANANA_TABLE::` unchanged
- Upgrade `BANANA_VIZ::` compat shim: convert `{type, title, x, y}` → Plotly `{data: [{type, x, y, name}], layout: {title}}`

### Step 4 — Upgrade NotebookRichOutput type
```ts
| { kind: "plotly"; figure: { data: unknown[]; layout?: Record<string, unknown> } }
```

### Step 5 — Upgrade cell output renderer (JSX)
- Replace `output.kind === "viz"` block with `<PlotlyChart figure={output.figure} />`
- Keep table renderer unchanged

### Step 6 — Markdown preview mode
- Add `previewMode: boolean` to `NotebookCell` (default false for code, true for markdown on load)
- Markdown cells show `<div dangerouslySetInnerHTML>` with minimal inline Markdown→HTML transform (headers, bold, code, bullets — no external dep)
- Click on preview → `previewMode = false`; blur of textarea → `previewMode = true`

### Step 7 — Cell execution count
- Add `execCount?: number` to `NotebookCell`
- Increment a global counter ref on each successful cell execution
- Display `[n]` in the cell header next to the cell type badge

### Step 8 — Update notebook 07
- Replace `BANANA_VIZ::` emitting cells with `BANANA_PLOTLY::` emitting proper Plotly JSON
- Ensure all 5 Python cells produce valid output on Run All

## Validation

1. `bun run --cwd src/typescript/react tsc --noEmit` — zero errors
2. Load notebook 07 → Run All → all cells produce Plotly charts or tables
3. Click markdown cell → edit mode; blur → preview renders `#` as `<h1>`
4. Execution counts advance correctly on repeated runs

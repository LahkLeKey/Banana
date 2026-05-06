# Data Science Page Visualization Deep-Dive Index

Generated: 2026-05-03
Scope: Follow-up investigation specs focused specifically on Banana's Data Science page at https://banana.engineer/data-science.

## Static and Baseline Contracts

- `191-matplotlib-png-contract-spike`
- `192-seaborn-stat-chart-contract-spike`
- `193-static-svg-vs-png-export-spike`
- `194-rich-output-sizing-and-layout-spike`
- `195-report-mode-readability-spike`
- `196-output-persistence-ipynb-attachment-spike`
- `197-thumbnail-preview-generation-spike`
- `198-static-theme-and-branding-spike`
- `199-print-export-pdf-friendly-spike`
- `200-error-fallback-output-contract-spike`

## Plotly Path

- `201-plotly-figure-schema-normalization-spike`
- `202-plotly-react-component-embedding-spike`
- `203-plotly-large-dataset-performance-spike`
- `204-plotly-theme-token-bridge-spike`
- `205-plotly-crossfilter-state-spike`
- `206-plotly-animation-timeline-spike`
- `207-plotly-image-export-and-snapshot-spike`
- `208-plotly-subplots-and-linked-views-spike`
- `209-plotly-notebook-authoring-ux-spike`
- `210-plotly-security-sanitization-spike`

## Altair and Vega Path

- `211-altair-json-emission-contract-spike`
- `212-vega-lite-react-renderer-spike`
- `213-vega-theme-token-bridge-spike`
- `214-vega-interaction-selection-bridge-spike`
- `215-vega-transform-offload-boundary-spike`
- `216-vega-large-spec-persistence-spike`
- `217-vega-export-png-svg-spike`
- `218-vega-multi-view-dashboard-spike`
- `219-vega-security-expression-sandbox-spike`
- `220-altair-authoring-scaffold-spike`

## Bokeh and Realtime Path

- `221-bokeh-embed-bundle-contract-spike`
- `222-bokeh-streaming-data-contract-spike`
- `223-bokeh-serverless-browser-feasibility-spike`
- `224-bokeh-react-host-lifecycle-spike`
- `225-bokeh-widget-callback-interop-spike`
- `226-bokeh-theme-token-bridge-spike`
- `227-bokeh-large-timeseries-downsampling-spike`
- `228-bokeh-security-html-js-sanitization-spike`

## Widget and Control-Surface Path

- `229-ipywidgets-state-model-bridge-spike`
- `230-slider-dropdown-control-patterns-spike`
- `231-widget-driven-chart-recompute-spike`
- `232-ipywidgets-persistence-and-replay-spike`
- `233-ipympl-interactive-matplotlib-bridge-spike`
- `234-bqplot-react-host-feasibility-spike`
- `235-widget-accessibility-focus-contract-spike`
- `236-widget-security-kernel-message-contract-spike`

## Specialized Surface Path

- `237-geospatial-ipyleaflet-contract-spike`
- `238-network-graph-surface-contract-spike`
- `239-3d-webgl-notebook-surface-spike`
- `240-low-level-canvas-custom-viz-spike`

## Suggested Execution Order

1. Baseline contracts: 191-200
2. Primary charting decision: 201-220
3. Runtime-expanding options: 221-236
4. Specialized-surface gating: 237-240

## Program Intent

Use this deep-dive set to decide what the Banana Data Science page should become before more redesign or integration work lands in production code.

## Orchestration Gate

- `241-ds-prod-readiness-orchestration`
- Purpose: execute spikes 186-240 in dependency waves and produce one DS production go/no-go packet.

## Implementation Tracks

- `242-ds-wave0-foundation-implementation` (implements spike outputs 186-200)
- `243-ds-wave1-chart-contract-implementation` (implements spike outputs 201-220)
- `244-ds-wave2-runtime-options-implementation` (implements spike outputs 221-236)
- `245-ds-wave3-specialized-surface-implementation` (implements spike outputs 237-240)
- `246-ds-prod-release-execution` (final release gate execution)

Current execution start point: `242-ds-wave0-foundation-implementation`.
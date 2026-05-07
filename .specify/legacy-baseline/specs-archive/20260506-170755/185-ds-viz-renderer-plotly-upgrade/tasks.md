# Tasks: DS Viz Renderer Plotly Upgrade (185)

- [x] T1: Add `PLOTLY_SCRIPT` constant + `window.Plotly` type stub + `ensurePlotly()` loader
- [x] T2: Implement inline `PlotlyChart` component (useRef + newPlot/react/purge lifecycle)
- [x] T3: Extend `NotebookRichOutput` union with `plotly` kind; extend `parseRichOutput` with `BANANA_PLOTLY::` and compat shim for `BANANA_VIZ::`
- [x] T4: Replace viz output renderer in JSX with `<PlotlyChart>`
- [x] T5: Add `previewMode` + `execCount` fields to `NotebookCell`; wire markdown preview toggle (click ↔ blur); render inline Markdown→HTML
- [x] T6: Add execution counter ref; increment on successful cell run; display `[n]` in cell header
- [x] T7: Update notebook 07 cells to emit `BANANA_PLOTLY::` with proper Plotly figure JSON
- [x] T8: Deploy + live verify all 5 notebook 07 cells produce real Plotly charts

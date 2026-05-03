# Audit

## Scope Alignment
- Status: in-progress
- Notes:
	- Added first-class runtime-contract cards for gated families including Vega, Altair, Bokeh, ipywidgets, ipympl, and bqplot in the replacement Data Science page.
	- Extended notebook rich-output persistence so gated runtime families survive notebook save/import flows instead of degrading into generic stdout notices.
	- Updated the shipped 07 visualization workbench notebook to emit representative gated runtime markers for runtime-option validation.

## Risks and Constraints
- Status: in-progress
- Notes:
	- Runtime-option families remain intentionally non-executable in the replacement browser host; the page surfaces them as explicit gates instead of pretending support exists.
	- Integrated-browser validation for the replacement UI is still blocked on serving or deploying the local UI build.
	- Full Vite build remains blocked by an unrelated shared UI token import path failure outside this edit slice; TypeScript validation succeeded for the touched React page.

## Evidence
- Status: in-progress
- Evidence:
	- Code updates: src/typescript/react/src/pages/DataSciencePage.tsx
	- Notebook fixture updates: src/typescript/react/public/notebooks/07-visualization-workbench.ipynb
	- Validation: editor diagnostics clean on DataSciencePage.tsx and `bunx tsc -b --pretty false` completed successfully in src/typescript/react.

# Audit

## Scope Alignment
- Status: in-progress
- Notes:
	- Implemented Plotly-first rich output handling in the replacement Data Science page, including full figure normalization, large-trace downsampling, and explicit notice outputs for unsupported or malformed marker payloads.
	- Upgraded notebook import/export compatibility so chart-rich cells persist both Banana metadata and standard notebook output fallbacks.
	- Updated notebook scaffolds to emit BANANA_PLOTLY markers for the primary visualization path while retaining BANANA_TABLE support.

## Risks and Constraints
- Status: in-progress
- Notes:
	- The production DS page is still the older UI, so runtime checks there validate the production API target and marker behavior but not the replacement layout/interaction model.
	- Local interactive preview remains blocked by the Windows console allocation limit when trying to host the built React app from Git Bash.
	- Release confidence for Wave 1 remains gated on serving the replacement UI for browser verification.

## Evidence
- Status: in-progress
- Evidence:
	- Code updates: src/typescript/react/src/pages/DataSciencePage.tsx
	- Validation: bun run build (with VITE_BANANA_API_BASE_URL set) completed successfully after Wave 1 chart-contract changes.
	- Browser/API evidence: notebook execution against the production API rendered both chart and table artifacts from live swagger-derived data on the deployed DS route, confirming the chosen production API test target remains valid.

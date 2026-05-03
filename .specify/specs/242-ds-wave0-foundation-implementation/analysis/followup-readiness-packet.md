# Follow-Up Readiness Packet

## Next Implementation Slice
- Slice id: 243
- Slice name: DS Wave 1 Chart Contract Implementation

## Bounded Scope
- In scope: Plotly-first chart contract work, chart payload normalization, rich chart fallback notices, notebook 07 chart scaffold upgrades, and chart-host rendering lifecycle hardening.
- Out of scope: runtime-option families from spikes 221-236, specialized surfaces from spikes 237-240, and release promotion of the replacement UI.

## Validation Lane
- Required checks: bun build validation for src/typescript/react plus integrated-browser verification once the replacement UI can be served locally or deployed.
- Evidence paths: .specify/specs/242-ds-wave0-foundation-implementation/analysis/audit.md, src/typescript/react/src/pages/DataSciencePage.tsx

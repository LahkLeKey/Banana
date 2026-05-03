# Follow-Up Readiness Packet

## Next Implementation Slice
- Slice id: 246
- Slice name: DS Production Release Execution

## Bounded Scope
- In scope: replacement-UI deployment/promotion, runtime verification on the actual replacement surface, and release-governance closeout for the Data Science page.
- Out of scope: new visualization family implementation beyond the already explicit active, gated, and deferred boundaries.

## Validation Lane
- Required checks: TypeScript validation for src/typescript/react, integrated-browser verification of the replacement UI, and deployment-level confirmation that the live DS route matches the replacement source instead of the stale production page.
- Evidence paths: .specify/specs/245-ds-wave3-specialized-surface-implementation/analysis/audit.md, src/typescript/react/src/pages/DataSciencePage.tsx, src/typescript/react/public/notebooks/07-visualization-workbench.ipynb

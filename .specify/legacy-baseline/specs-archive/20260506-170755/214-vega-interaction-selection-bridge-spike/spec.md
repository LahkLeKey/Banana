# Feature Specification: Vega Interaction Selection Bridge Spike (214)

**Feature Branch**: `214-vega-interaction-selection-bridge-spike`
**Created**: 2026-05-03
**Status**: Ready for implementation
**Input**: Investigate whether Vega selections and interactive filters should bridge into Data Science page state on Banana.

## Problem Statement

Vega's interaction model is powerful, but the Data Science page needs a clear boundary for what interactive selections should mean at the page level. We need to determine whether notebook-scoped selections stay local or drive broader page behavior.

## In Scope

- Define the role of Vega selections and interactive filters for Banana notebooks.
- Evaluate DS-page interaction boundaries for local versus shared state.
- Document UX and implementation tradeoffs.

## Out of Scope

- Plotly crossfilter work.
- Full BI dashboard state management.
- Backend session coordination.

## Success Criteria

- A recommendation exists for Vega interaction bridging on the Data Science page.
- Shared-state tradeoffs are documented.
- Follow-up implementation scope is bounded.

## Assumptions

- Interaction richness only matters if it improves notebook workflows without collapsing clarity.
- State boundaries must be defined before broadening declarative interactivity.
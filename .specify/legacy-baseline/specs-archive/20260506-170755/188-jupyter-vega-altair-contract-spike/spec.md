# Feature Specification: Jupyter Vega Altair Contract Spike (188)

**Feature Branch**: `188-jupyter-vega-altair-contract-spike`
**Created**: 2026-05-03
**Status**: Ready for implementation
**Input**: Investigate Altair and Vega/Vega-Lite as declarative notebook visualization contracts for the Banana Data Science page.

## Problem Statement

Plotly is not the only viable React bridge for notebook outputs. Vega and Altair may provide a cleaner declarative contract, especially if Banana wants portable JSON specs, analytics dashboards, and stronger schema discipline. We need a spike to determine whether Vega-family tooling should be a first-class path.

## In Scope

- Define how Altair/Vega/Vega-Lite outputs would map from notebook execution into React.
- Compare declarative spec portability and governance against Plotly-oriented outputs.
- Document UI and contract implications for embedding Vega-family outputs inside Banana.

## Out of Scope

- Full multi-library implementation.
- Rich notebook widget behavior outside declarative chart specs.

## Success Criteria

- Vega-family output contracts are documented for Banana notebook rendering.
- Tradeoffs versus Plotly are explicit and decision-ready.
- The spike clarifies whether Banana should support Vega-family specs as a primary or secondary path.

## Assumptions

- Declarative JSON chart contracts are attractive if they improve consistency and portability.
- The Data Science redesign should evaluate more than one interactive chart path before standardizing.
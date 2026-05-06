# Feature Specification: Vega Transform Offload Boundary Spike (215)

**Feature Branch**: `215-vega-transform-offload-boundary-spike`
**Created**: 2026-05-03
**Status**: Ready for implementation
**Input**: Investigate which Vega transforms should run inside the Banana Data Science page versus being resolved before rendering.

## Problem Statement

Declarative charts can hide substantial transformation logic. We need to decide how much of that work the Data Science page should accept at render time before performance, safety, and maintainability start to break down.

## In Scope

- Define the transform complexity boundary for Vega on the Data Science page.
- Evaluate when transforms should stay in spec versus be precomputed.
- Document performance and product-scope tradeoffs.

## Out of Scope

- General ETL architecture.
- Plotly data transformation behavior.
- Server-side compute systems.

## Success Criteria

- A recommendation exists for Vega transform boundaries on the Data Science page.
- Performance and maintainability tradeoffs are documented.
- Follow-up implementation scope is bounded.

## Assumptions

- Declarative specs can become accidental compute engines if boundaries are not explicit.
- The Data Science page should prefer predictable rendering over hidden transformation cost.
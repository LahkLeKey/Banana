# Feature Specification: Jupyter Static Viz Baseline Spike (186)

**Feature Branch**: `186-jupyter-static-viz-baseline-spike`
**Created**: 2026-05-03
**Status**: Ready for implementation
**Input**: Investigate the static and scientific baseline for Jupyter visualization in Banana before committing the Data Science tab to a richer interactive stack.

## Problem Statement

The Data Science redesign currently assumes interactive browser-native charts are always the right answer. We need a spike that measures the lowest-complexity baseline first: how Matplotlib and Seaborn outputs should be represented, rendered, and persisted inside the Banana notebook workspace.

## In Scope

- Define how Matplotlib and Seaborn outputs would reach the React surface from notebook execution.
- Compare image, HTML, and serialized artifact contracts for static chart rendering.
- Document the minimum viable UX for static report-oriented notebooks.

## Out of Scope

- Full interactive dashboard implementation.
- Server-backed kernel infrastructure.
- New Python package shipping decisions outside the spike recommendation.

## Success Criteria

- Static notebook chart delivery options are documented with tradeoffs.
- A recommended baseline contract exists for scientific/report-style outputs.
- Follow-up implications for Data Science page layout are captured.

## Assumptions

- Banana will continue to use browser-hosted notebook execution for the near-term Data Science tab.
- The redesign must justify any heavier interactive dependency against a static baseline.
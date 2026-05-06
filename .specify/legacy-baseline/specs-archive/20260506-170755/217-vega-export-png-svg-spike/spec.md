# Feature Specification: Vega Export PNG SVG Spike (217)

**Feature Branch**: `217-vega-export-png-svg-spike`
**Created**: 2026-05-03
**Status**: Ready for implementation
**Input**: Investigate how Vega and Vega-Lite visuals should export to PNG or SVG on the Banana Data Science page.

## Problem Statement

Declarative charts are appealing partly because they can render clean static derivatives. We need to determine how the Data Science page should handle PNG and SVG export for Vega visuals, and how that affects notebook persistence and reporting.

## In Scope

- Define PNG and SVG export expectations for Vega visuals.
- Evaluate DS-page implications for persistence and reporting.
- Document when static derivatives should be preferred.

## Out of Scope

- Plotly export systems.
- Server-side publishing pipelines.
- Non-visual notebook export types.

## Success Criteria

- A recommendation exists for Vega PNG/SVG export behavior.
- DS-page persistence and report-mode implications are documented.
- Follow-up implementation scope is bounded.

## Assumptions

- Static derivatives may be a major strength of declarative charts for Banana.
- Export behavior should be explicit and intentional.
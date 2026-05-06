# Feature Specification: Vega Theme Token Bridge Spike (213)

**Feature Branch**: `213-vega-theme-token-bridge-spike`
**Created**: 2026-05-03
**Status**: Ready for implementation
**Input**: Investigate how Vega and Vega-Lite charts should inherit Banana Data Science page theme tokens and framing.

## Problem Statement

Declarative charts still need to look like part of Banana. We need to determine how the Data Science page should bridge theme tokens into Vega without making chart specs brittle or visually inconsistent.

## In Scope

- Define how Vega charts should align with Banana page theming.
- Evaluate where chart-level versus container-level visual ownership should live.
- Document readability and brand-consistency implications.

## Out of Scope

- Global app theming overhaul.
- Plotly theming work.
- Per-chart authoring systems beyond boundary definition.

## Success Criteria

- A recommended Vega theme bridge exists for the Data Science page.
- Brand and readability implications are documented.
- Follow-up implementation scope is bounded.

## Assumptions

- The Data Science page should feel cohesive regardless of charting stack.
- Theme ownership should be explicit before deeper Vega adoption.
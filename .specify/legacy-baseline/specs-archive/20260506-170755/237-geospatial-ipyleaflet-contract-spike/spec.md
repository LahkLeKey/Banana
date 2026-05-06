# Feature Specification: Geospatial Ipyleaflet Contract Spike (237)

**Feature Branch**: `237-geospatial-ipyleaflet-contract-spike`
**Created**: 2026-05-03
**Status**: Ready for implementation
**Input**: Investigate whether geospatial notebook visuals such as ipyleaflet belong on the Banana Data Science page at https://banana.engineer/data-science.

## Problem Statement

Geospatial visuals can be high-value, but they also pull the Data Science page toward a specialized mapping surface. We need to determine whether map-centric notebook outputs belong in Banana's DS-page boundary and under what constraints.

## In Scope

- Define the role of geospatial notebook visuals for Banana notebooks.
- Evaluate DS-page fit for map rendering, controls, and persistence.
- Document scope, UX, and runtime tradeoffs.

## Out of Scope

- Full GIS productization.
- General mapping backends or tile infrastructure.
- Non-geospatial specialized visual families.

## Success Criteria

- A recommendation exists for geospatial notebook support on the Data Science page.
- UX and scope tradeoffs are documented.
- Follow-up implementation scope is bounded.

## Assumptions

- Maps should only enter the DS-page roadmap if they serve realistic Banana workflows.
- Specialized surfaces need stricter product-boundary decisions than baseline charts.
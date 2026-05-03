# Feature Specification: Matplotlib PNG Contract Spike (191)

**Feature Branch**: `191-matplotlib-png-contract-spike`
**Created**: 2026-05-03
**Status**: Ready for implementation
**Input**: Investigate how Matplotlib PNG-style outputs should be emitted, rendered, and persisted inside the Banana Data Science page at https://banana.engineer/data-science.

## Problem Statement

The Banana Data Science page needs a grounded contract for foundational scientific chart output before broader UI redesign work continues. We need to determine how Matplotlib PNG artifacts should move from notebook execution into the page without creating layout drift, persistence gaps, or export ambiguity.

## In Scope

- Define how Matplotlib PNG output should be represented in Banana notebook cell output state.
- Evaluate rendering behavior for PNG outputs inside the Data Science page notebook canvas.
- Document persistence and export implications for PNG-based notebook visuals.

## Out of Scope

- Full interactive chart tooling.
- Server-hosted kernel infrastructure.
- General-purpose redesign work outside Matplotlib PNG output handling.

## Success Criteria

- A recommended Matplotlib PNG output contract exists for Banana notebooks.
- DS-page rendering implications for PNG outputs are documented.
- Follow-up implementation boundaries are captured for the next slice.

## Assumptions

- The near-term Banana Data Science page will keep browser-hosted notebook execution.
- Matplotlib remains a baseline candidate even if richer libraries are adopted later.
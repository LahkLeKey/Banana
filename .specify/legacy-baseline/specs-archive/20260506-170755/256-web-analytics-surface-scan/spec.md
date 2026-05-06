# Feature Specification: Web Analytics Surface Scan (256)

**Feature Branch**: `256-web-analytics-surface-scan`  
**Created**: 2026-05-05  
**Status**: Draft

## Problem Statement

We enabled Vercel Web Analytics for production, but we do not have a repeatable way to verify that analytics coverage remains consistent across the React route tree and high-value interaction points as pages evolve.

## Goal

Define a scanning workflow that inventories analytics instrumentation coverage and highlights missing or drifted analytics hooks before release.

## In Scope

- Route-level analytics coverage scan for React workspace routes.
- Interaction-surface scan for key user actions (navigation, classifier submit, operator actions).
- CI-friendly report artifact with machine-readable and human-readable outputs.
- Coverage thresholds that can fail a gate when required surfaces are uninstrumented.

## Out of Scope

- Replacing Vercel Analytics transport/provider.
- Building full product analytics dashboards.
- Non-web channels (Electron, React Native) unless explicitly added later.

## Functional Requirements

- FR-256-01: Scanner MUST discover all active web routes and report whether route view instrumentation exists.
- FR-256-02: Scanner MUST evaluate configured key interaction surfaces and report whether event instrumentation exists.
- FR-256-03: Scanner MUST output a deterministic report (JSON + Markdown summary) under artifacts.
- FR-256-04: Scanner MUST support strict mode that fails with non-zero exit code when required coverage thresholds are not met.

## Success Criteria

- SC-001: Route coverage report includes every route defined in the React router tree.
- SC-002: At least one required interaction event is tracked per critical workflow category.
- SC-003: CI can enforce strict coverage without flaky or non-deterministic output.

## Assumptions

- Analytics events are emitted through shared helper utilities for stable scanning.
- Initial threshold can be conservative and tightened over time.

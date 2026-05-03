# Implementation Plan: BananaAI Explainability Pack (179)

**Branch**: `179-bananaai-explainability-pack` | **Date**: 2026-05-03 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/179-bananaai-explainability-pack/spec.md`

## Overview

Add five explainability surfaces to BananaAI: feature attribution, calibration hint, lane contribution chart, OOD reason list, and one-click verdict JSON copy.

## Technical Context

**Language/Version**: TypeScript, React
**Primary Dependencies**: React app under `src/typescript/react`
**Storage**: No new persistence required
**Testing**: Type-check and focused render interaction checks
**Target Platform**: Browser runtime on banana.engineer
**Project Type**: Frontend explainability enhancement
**Constraints**: Preserve existing verdict API contract; additive UI rendering only
**Scale/Scope**: BananaAI page and helper formatters

## Constitution Check

- Domain layering preserved: PASS.
- Spec-first workflow preserved: PASS.
- Validation path defined: PASS.

## Enhancement Bundle

1. Top feature attribution list.
2. Confidence calibration hint.
3. Ensemble lane contribution chart.
4. Out-of-domain trigger reasons.
5. One-click copy of structured verdict JSON.

## Validation Approach

1. Verify explainability elements render from verdict payload.
2. Verify calibration hint appears for low/medium/high confidence bands.
3. Verify OOD reasons display when present.
4. Verify copied JSON parses cleanly.

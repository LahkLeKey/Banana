# Implementation Plan: Knowledge Reliability Upgrade (177)

**Branch**: `177-knowledge-reliability-upgrade` | **Date**: 2026-05-03 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/177-knowledge-reliability-upgrade/spec.md`

## Overview

Upgrade the Knowledge page into a reliability workspace by adding health badges, lane sparklines, run detail drawer, stale-data banner, and copy/export actions.

## Technical Context

**Language/Version**: TypeScript, React
**Primary Dependencies**: React app under `src/typescript/react`
**Storage**: Existing API history responses; no new persistence required
**Testing**: Type-check plus focused interaction checks
**Target Platform**: Browser runtime on banana.engineer
**Project Type**: Frontend reliability UX
**Constraints**: Keep existing API contracts stable
**Scale/Scope**: Knowledge page and supporting helpers

## Constitution Check

- Domain layering preserved: PASS.
- Spec-first workflow preserved: PASS.
- Validation path defined: PASS.

## Enhancement Bundle

1. Lane health badges.
2. History sparkline per lane.
3. Drill-in run details drawer.
4. Stale-data warning banner.
5. Copy/export visible metrics.

## Validation Approach

1. Verify badges and sparklines map to lane history data.
2. Verify drawer opens with selected run details.
3. Verify stale banner appears on old data.
4. Verify export contains rendered metrics.

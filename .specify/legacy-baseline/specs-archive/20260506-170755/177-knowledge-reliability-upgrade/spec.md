# Feature Specification: Knowledge Reliability Upgrade (177)

**Feature Branch**: `177-knowledge-reliability-upgrade`
**Created**: 2026-05-03
**Status**: Draft
**Input**: Enhancement spike 172 from Review Spikes page.

## Problem Statement

Knowledge surfaces are informative but not consistently diagnostic. Operators need clear health signal, trend visibility, and direct drill-in without context switching.

## In Scope

- E1: Lane health badges.
- E2: History sparkline per lane.
- E3: Drill-in run details drawer.
- E4: Stale-data warning banner.
- E5: Copy/export of visible metrics.

## Out of Scope

- New data warehouse pipelines.
- Long-term analytics storage redesign.
- Non-knowledge page redesign.

## User Scenarios & Testing

### User Story 1 - Detect Lane Health Quickly (Priority: P1)

As an operator, I can see lane health badges and trend sparklines at a glance.

**Why this priority**: Fast health checks reduce delayed incident response.

**Independent Test**: Load page with mixed lane states and confirm badges + sparklines align with inputs.

**Acceptance Scenarios**:

1. **Given** lane metrics exist, **When** page renders, **Then** badge status and sparkline are shown for each lane.

---

### User Story 2 - Drill Into Runs (Priority: P2)

As an operator, I can open a run details drawer from lane cards.

**Why this priority**: Reduces manual navigation and preserves context.

**Independent Test**: Select a lane run and verify drawer shows run metadata and metrics.

**Acceptance Scenarios**:

1. **Given** a run row is selected, **When** drawer opens, **Then** run details appear without route change.

## Requirements

- FR-001: Lane cards MUST expose health badge state.
- FR-002: Each lane MUST render a history sparkline if history exists.
- FR-003: Run details drawer MUST open inline from lane context.
- FR-004: Stale-data warning MUST appear when data age threshold is exceeded.
- FR-005: Visible metrics MUST be copyable/exportable in structured form.

## Success Criteria

- SC-001: Operators can identify unhealthy lanes within one page load.
- SC-002: Drill-in completes without full-page navigation.
- SC-003: Export includes all currently visible lane metrics.

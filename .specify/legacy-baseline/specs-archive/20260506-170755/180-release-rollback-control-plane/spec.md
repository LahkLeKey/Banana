# Feature Specification: Release and Rollback Control Plane (180)

**Feature Branch**: `180-release-rollback-control-plane`
**Created**: 2026-05-03
**Status**: Draft
**Input**: Enhancement spike 175 from Review Spikes page.

## Problem Statement

Release promotion and rollback decisions are not centralized enough for predictable operations. Teams need explicit gates and a single control surface.

## In Scope

- E1: Release checklist card.
- E2: Canary health gate.
- E3: Rollback playbook action.
- E4: Environment drift summary.
- E5: Post-release verification matrix.

## Out of Scope

- Full deployment platform replacement.
- New CI runner infrastructure.
- Unrelated product UI changes.

## User Scenarios & Testing

### User Story 1 - Gate Promotion Safely (Priority: P1)

As an operator, I can verify checklist completion and canary health before release promotion.

**Why this priority**: Prevents risky promotions and reduces rollback frequency.

**Independent Test**: Simulate failing canary and verify promotion gate blocks action.

**Acceptance Scenarios**:

1. **Given** canary health is red, **When** promote is requested, **Then** gate blocks with reason.
2. **Given** checklist is incomplete, **When** promote is requested, **Then** release remains blocked.

---

### User Story 2 - Roll Back with Confidence (Priority: P1)

As an operator, I can trigger rollback using a guided playbook action.

**Why this priority**: Speeds recovery and reduces operator error.

**Independent Test**: Trigger rollback and verify playbook steps and post-checks are surfaced.

**Acceptance Scenarios**:

1. **Given** a degraded release, **When** rollback action runs, **Then** guided steps and status are shown.

## Requirements

- FR-001: Release checklist card MUST track required gate items.
- FR-002: Canary health gate MUST block promotion on unhealthy status.
- FR-003: Rollback playbook action MUST render ordered recovery steps.
- FR-004: Environment drift summary MUST show config differences.
- FR-005: Verification matrix MUST capture post-release checks and outcomes.

## Success Criteria

- SC-001: Promotions are blocked when required gates fail.
- SC-002: Rollback initiation time decreases with guided action.
- SC-003: Post-release verification completion is visible in one matrix view.

# Feature Specification: BananaAI Explainability Pack (179)

**Feature Branch**: `179-bananaai-explainability-pack`
**Created**: 2026-05-03
**Status**: Draft
**Input**: Enhancement spike 174 from Review Spikes page.

## Problem Statement

BananaAI results are useful but can be opaque during audit and troubleshooting. Operators need concise explanation surfaces for confidence and lane contributions.

## In Scope

- E1: Top feature attribution list.
- E2: Confidence calibration hint.
- E3: Ensemble lane contribution chart.
- E4: Out-of-domain trigger reasons.
- E5: One-click copy of structured verdict JSON.

## Out of Scope

- Retraining or model architecture changes.
- New inference APIs for external consumers.
- Non-BananaAI page redesign.

## User Scenarios & Testing

### User Story 1 - Understand Verdict Drivers (Priority: P1)

As an operator, I can see the top factors and lane contributions behind a verdict.

**Why this priority**: Auditability and trust improve when outputs are explainable.

**Independent Test**: Run a verdict and confirm attribution list and lane contribution chart render with data.

**Acceptance Scenarios**:

1. **Given** a verdict response, **When** details expand, **Then** top feature attribution and lane contributions are visible.

---

### User Story 2 - Handle OOD Cases (Priority: P2)

As an operator, I can see why a sample is marked out-of-domain.

**Why this priority**: Enables fast escalation and safer handling.

**Independent Test**: Trigger OOD sample and verify explicit reasons are shown.

**Acceptance Scenarios**:

1. **Given** an OOD trigger occurs, **When** verdict renders, **Then** trigger reasons are listed.

## Requirements

- FR-001: Attribution list MUST show ranked top factors.
- FR-002: Calibration hint MUST map confidence to qualitative guidance.
- FR-003: Contribution chart MUST render per-lane weights.
- FR-004: OOD reason list MUST display normalized trigger reasons.
- FR-005: Structured verdict JSON MUST be copyable in one action.

## Success Criteria

- SC-001: Operators can explain verdict rationale without external logs.
- SC-002: OOD escalations include explicit reason text.
- SC-003: Copied verdict JSON is valid and complete for ticket attachments.

# Feature Specification: Operator Command Center UX (176)

**Feature Branch**: `176-operator-command-center-ux`
**Created**: 2026-05-03
**Status**: Draft
**Input**: Enhancement spike 171 from Review Spikes page.

## Problem Statement

Operator workflows require too many clicks and manual retries. Session recovery, searchability, and repeated actions are slow under load.

## In Scope

- E1: Pinned recent sessions.
- E2: Message search and filter.
- E3: Token usage meter per active session.
- E4: Retry failed turn button.
- E5: Quick command chips for common operator actions.

## Out of Scope

- Multi-tenant role policy changes.
- Backend model behavior changes.
- New billing or quota contracts.

## User Scenarios & Testing

### User Story 1 - Recover Active Work Fast (Priority: P1)

As an operator, I can pin and reopen recent sessions without manual lookup.

**Why this priority**: Reduces time-to-recovery during incident response.

**Independent Test**: Pin two sessions, reload, verify pinned list remains and opens correctly.

**Acceptance Scenarios**:

1. **Given** a session is open, **When** I pin it, **Then** it appears in pinned sessions.
2. **Given** pinned sessions exist, **When** I reload, **Then** pinned sessions persist.

---

### User Story 2 - Find Messages Quickly (Priority: P1)

As an operator, I can search and filter messages to isolate relevant turns.

**Why this priority**: Speeds root-cause triage.

**Independent Test**: Search by keyword and filter by role; only matching messages remain visible.

**Acceptance Scenarios**:

1. **Given** a session with mixed turns, **When** I apply search and role filter, **Then** only matching turns render.

---

### User Story 3 - Recover Failed Turns (Priority: P2)

As an operator, I can retry a failed turn directly from the conversation.

**Why this priority**: Prevents copy/paste rework.

**Independent Test**: Fail one turn and retry; a new successful turn is appended.

**Acceptance Scenarios**:

1. **Given** a failed turn exists, **When** I click retry, **Then** a rerun starts using the same input.

## Requirements

### Functional Requirements

- FR-001: Pinned session list MUST persist across page reloads.
- FR-002: Search and filters MUST apply without full page reload.
- FR-003: Token meter MUST show current session token usage or fallback state.
- FR-004: Retry action MUST preserve original prompt payload.
- FR-005: Quick command chips MUST execute mapped operator actions.

## Success Criteria

- SC-001: Session recovery actions complete in under 2 clicks for pinned sessions.
- SC-002: Message search returns expected subset for keyword and role filter.
- SC-003: Retry success rate improves versus manual re-entry baseline.

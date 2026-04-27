# Feature Specification: Chat Bootstrap Fetch Failure Follow-up

**Feature Branch**: `[046-chat-bootstrap-fetch-failure]`  
**Created**: 2026-04-26  
**Status**: Draft  
**Input**: User report: "new error: Chat bootstrap failed: Failed to fetch"

## In Scope *(mandatory)*

- Diagnose and fix frontend chat bootstrap failure surfaces where startup reports `Chat bootstrap failed: Failed to fetch`.
- Ensure bootstrap failure messaging differentiates network/transport failures from configuration-resolution failures.
- Improve recovery behavior so chat/session state can recover when runtime dependencies become available.
- Add deterministic validation flow for runtime+apps startup order and rendered bootstrap status.
- Preserve existing one-window integrated browser verification expectations while addressing this specific failure mode.

## Out of Scope *(mandatory)*

- Redesigning chat UX unrelated to bootstrap/network failure handling.
- Changing backend business behavior beyond availability/contract verification required for bootstrap.
- Replacing compose profiles or runtime architecture.
- Production deployment hardening outside local runtime workflows.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Bootstrap succeeds when runtime is healthy (Priority: P1)

As a developer launching Banana v2 with canonical runtime and apps profiles, I need chat bootstrap to succeed without fetch failure errors.

**Why this priority**: This failure blocks chat usability and is currently the most visible remaining issue.

**Independent Test**: Start runtime and apps using canonical compose flow and verify session initializes without bootstrap fetch error.

**Acceptance Scenarios**:

1. **Given** runtime API and frontend app are both healthy, **When** the page loads, **Then** bootstrap does not display `Failed to fetch` and session transitions to ready.
2. **Given** healthy runtime stack, **When** user enters a prompt, **Then** chat input/send controls are usable and respond through normal lifecycle.

---

### User Story 2 - Network/bootstrap errors are accurate and actionable (Priority: P1)

As a developer troubleshooting startup issues, I need fetch-related bootstrap errors to explicitly indicate transport/runtime availability problems.

**Why this priority**: Current message is generic and can be confused with config-resolution regressions.

**Independent Test**: Intentionally stop runtime API while frontend remains up and verify bootstrap error classification and remediation messaging.

**Acceptance Scenarios**:

1. **Given** API endpoint is unreachable, **When** bootstrap runs, **Then** UI reports transport/bootstrap failure with remediation aligned to runtime bring-up.
2. **Given** API becomes reachable after remediation, **When** app retries or reloads, **Then** transport/bootstrap error clears and session bootstrap can proceed.

---

### User Story 3 - Startup order and drift diagnostics prevent recurrence (Priority: P2)

As a maintainer, I need validation coverage that catches startup ordering and availability mismatches before task closure.

**Why this priority**: Regressions can reappear if apps are up but runtime API is not yet healthy.

**Independent Test**: Run diagnostic flow and verify it flags runtime availability/bootstrap readiness mismatches with clear pass/fail outcomes.

**Acceptance Scenarios**:

1. **Given** runtime is unavailable, **When** diagnostics execute, **Then** output identifies the bootstrap dependency failure layer.
2. **Given** runtime and apps are healthy, **When** diagnostics execute, **Then** output confirms bootstrap prerequisites and rendered checks pass.

### Edge Cases

- Frontend resolves API base but API process is unreachable due to runtime profile not ready.
- Browser keeps stale page state after runtime recovery and still shows prior fetch error.
- Bootstrap retries race with delayed API readiness and produce transient failure states.
- Bootstrap error handling masks non-network API errors that should be surfaced distinctly.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST classify bootstrap fetch failures as transport/runtime-availability errors when API endpoint is unreachable.
- **FR-002**: System MUST keep configuration-resolution errors distinct from bootstrap transport errors in rendered messaging.
- **FR-003**: System MUST provide remediation guidance for bootstrap fetch failures that references canonical runtime/apps startup order.
- **FR-004**: System MUST recover from bootstrap fetch failure once runtime API is healthy, without requiring manual source edits.
- **FR-005**: System MUST include validation coverage for healthy bootstrap path and failure/recovery path.
- **FR-006**: System MUST preserve one-window integrated-browser verification for closure evidence.

### Key Entities *(include if feature involves data)*

- **ChatBootstrapState**: Lifecycle state for session initialization with phases and error classification.
- **BootstrapTransportErrorState**: Error model for endpoint-unreachable bootstrap failures and recovery guidance.
- **RuntimeDependencyReadiness**: Availability contract linking runtime profile health to frontend bootstrap preconditions.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Under healthy canonical startup, bootstrap fetch failure is absent in 100% of validation runs.
- **SC-002**: Transport/bootstrap failures surface distinct, actionable messaging from config-resolution failures.
- **SC-003**: After runtime remediation, session bootstrap reaches ready state without manual code edits.
- **SC-004**: Validation flow detects runtime-unavailable startup scenarios before task closure.

## Assumptions

- Canonical compose runtime/apps tasks remain the required startup path for local verification.
- Frontend API-base resolution contract from prior slices remains in effect.
- Existing diagnostics and one-window validation flow can be extended for bootstrap availability checks.
- Reported bootstrap fetch failure is reproducible in current local runtime environment.

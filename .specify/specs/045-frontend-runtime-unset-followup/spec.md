# Feature Specification: Frontend Runtime Unset Follow-up

**Feature Branch**: `[045-frontend-runtime-unset-followup]`
**Created**: 2026-04-26
**Status**: Draft
**Input**: User report: "frontend improved but still shows API base unset and runtime configuration error in Banana v2"

## In Scope *(mandatory)*

- Fix remaining frontend runtime defects where Banana v2 still renders API base as unset under the canonical compose startup path.
- Ensure runtime error messaging appears only when configuration is truly unresolved and not when canonical startup produced a valid API base.
- Restore usable frontend interaction path for web session bootstrap and chat controls when API base is valid.
- Add deterministic validation steps that confirm rendered runtime state, not just container environment values.
- Capture remediation behavior as part of acceptance so repeated down/up cycles do not leave the app in a stale unresolved state.

## Out of Scope *(mandatory)*

- Redesigning frontend UX unrelated to runtime/API-base resolution behavior.
- Changing backend business logic or native domain calculations.
- Replacing compose profile architecture.
- Production deployment hardening outside local runtime flow.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Canonical startup yields resolved API base (Priority: P1)

As a developer using the compose frontend startup flow, I need the UI to show a resolved API base so core features are not blocked.

**Why this priority**: The unresolved API base blocks chat bootstrap and leaves session state missing.

**Independent Test**: Run canonical compose runtime/apps startup and verify the rendered page does not show API base unset.

**Acceptance Scenarios**:

1. **Given** runtime and apps profiles are launched through canonical compose tasks, **When** Banana v2 loads, **Then** API base renders as a concrete URL and not unset.
2. **Given** API base resolves successfully, **When** the app initializes, **Then** runtime configuration error banner does not render.

---

### User Story 2 - Error state is accurate and actionable (Priority: P1)

As a developer troubleshooting config issues, I need error messaging to match real runtime state so I can trust remediation guidance.

**Why this priority**: Current mismatch reports unresolved configuration while compose wiring may already be correct.

**Independent Test**: Introduce an invalid API-base configuration and verify the expected error state and remediation text render correctly.

**Acceptance Scenarios**:

1. **Given** API base is truly missing, **When** the app initializes, **Then** configuration error appears with remediation steps that recover the state.
2. **Given** remediation steps are run and API base becomes valid, **When** app reloads, **Then** error state clears and normal interactive state returns.

---

### User Story 3 - Session/bootstrap path recovers after configuration fix (Priority: P2)

As a developer, I need chat session bootstrap to recover after config remediation so the app is usable without manual source edits.

**Why this priority**: Session missing and disabled chat controls indicate unresolved bootstrap even after startup cycles.

**Independent Test**: Reproduce unresolved state, apply documented remediation, and confirm session/bootstrap flows proceed.

**Acceptance Scenarios**:

1. **Given** an unresolved configuration state was present, **When** remediation is executed and app is relaunched, **Then** session bootstrap runs and no session-missing dead-end remains.
2. **Given** resolved API base, **When** user types a chat prompt, **Then** input/send controls are enabled according to normal bootstrap lifecycle.

### Edge Cases

- Container image/source drift causes stale frontend code path to persist after compose relaunch.
- Environment appears correct in container but rendered frontend still shows unresolved state.
- API base string contains whitespace or malformed value that passes shallow checks but fails runtime behavior.
- Session bootstrap failure path incorrectly reports only configuration error and masks transport/API errors.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST resolve and display non-empty API base during canonical compose startup for web frontend.
- **FR-002**: System MUST render runtime configuration error only when effective API base resolution is truly invalid.
- **FR-003**: System MUST provide deterministic remediation steps that clear unresolved runtime state after compose relaunch.
- **FR-004**: System MUST restore session bootstrap and chat availability when API base becomes valid.
- **FR-005**: System MUST include rendered-state validation checks to verify unresolved-state regressions are caught.
- **FR-006**: System MUST preserve current explicit messaging for true misconfiguration cases.
- **FR-007**: System MUST distinguish configuration-resolution failures from downstream API/session bootstrap failures in user-visible error handling.

### Key Entities *(include if feature involves data)*

- **ApiBaseResolutionState**: Effective runtime state describing whether frontend API base is valid, invalid, or unresolved.
- **FrontendRuntimeErrorState**: Rendered error model containing error type, message, remediation, and clear conditions.
- **BootstrapSessionState**: Session lifecycle state from initialization through chat readiness.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: In canonical runtime/apps startup validation, rendered API base is non-empty in 100% of verification runs.
- **SC-002**: Configuration error banner appears only in intentionally misconfigured scenarios.
- **SC-003**: After documented remediation, session bootstrap reaches usable state without manual code changes.
- **SC-004**: Regression checks detect unresolved-state reintroduction before task closure.

## Assumptions

- Compose startup/down tasks remain canonical entry points for local frontend runtime.
- Frontend runtime config contract continues to use VITE_BANANA_API_BASE_URL for web and BANANA_API_BASE_URL for Electron preload.
- Existing drift diagnostics script remains available and may be extended for rendered-state assertions.
- Reported issue is reproducible under current local environment and not limited to a single transient machine state.

# Feature Specification: One-Window Interactive Baseline

**Feature Branch**: `[044-one-window-interactive-baseline]`  
**Created**: 2026-04-26  
**Status**: Draft  
**Input**: User description: "make the VS Code web view flow a baseline so development stays interactive in one window"

## In Scope *(mandatory)*

- Establish a canonical one-window workflow for frontend and Electron validation using VS Code integrated browser plus compose-backed runtime services.
- Define closure criteria requiring rendered UI validation in the integrated browser before frontend/Electron tasks are marked done.
- Add explicit runbook guidance for the one-window loop: startup, interactive validation, diagnostics, and teardown.
- Add governance language so future feature slices treat this workflow as default, not optional.
- Ensure the baseline is framed for developer experience and drift reduction, not just launch mechanics.

## Out of Scope *(mandatory)*

- Replacing existing compose/runtime architecture.
- Redesigning frontend visuals or business behavior unrelated to the workflow baseline.
- Introducing production deployment changes.
- Mandating this workflow for non-frontend domains where interactive browser verification is not relevant.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Validate UI behavior without leaving VS Code (Priority: P1)

As a developer, I want startup, diagnostics, and rendered UI checks in one VS Code window so I can close frontend tasks faster with less context switching.

**Why this priority**: This directly addresses the current friction and is the primary user value requested.

**Independent Test**: Start the standard frontend runtime path, open the app in VS Code integrated browser, and complete a validation pass without external browser tooling.

**Acceptance Scenarios**:

1. **Given** runtime and app profiles are started from canonical tasks, **When** the developer opens the frontend in VS Code integrated browser, **Then** rendered behavior can be validated alongside terminal/task output in one window.
2. **Given** configuration is wrong, **When** the developer validates in the integrated browser, **Then** the failure can be diagnosed and remediated from the same workspace flow.

---

### User Story 2 - Task closure requires interactive rendered verification (Priority: P1)

As a maintainer, I want a repeatable closure rule for frontend slices so tasks are not marked done based only on static or container-level checks.

**Why this priority**: Prior drift incidents passed env checks while runtime rendering still failed.

**Independent Test**: Review a frontend task checklist and confirm rendered integrated-browser verification evidence is required before closure.

**Acceptance Scenarios**:

1. **Given** a frontend or Electron behavior change, **When** the task is closed, **Then** the workflow requires integrated-browser rendered verification, not only environment inspection.
2. **Given** a contributor skips rendered verification, **When** review occurs, **Then** the change is considered incomplete until one-window interactive validation is performed.

---

### User Story 3 - New contributors can follow one-window baseline quickly (Priority: P2)

As a new contributor, I want concise runbook steps for the one-window loop so I can execute the baseline without guessing tool order.

**Why this priority**: Adoption depends on clear, low-friction instructions.

**Independent Test**: A new contributor follows the runbook and completes startup, validation, and teardown in one pass.

**Acceptance Scenarios**:

1. **Given** no prior knowledge of the recent drift fixes, **When** the runbook is followed, **Then** the contributor can launch and validate frontend behavior through VS Code integrated browser.
2. **Given** stale runtime behavior, **When** runbook troubleshooting is followed, **Then** the contributor can recover with documented rebuild/relaunch steps.

### Edge Cases

- Integrated browser validates old frontend assets because containers/images were not rebuilt after source changes.
- Runtime services are healthy but frontend environment resolution is still incorrect at render time.
- User launches only part of the profile stack and expects complete one-window validation.
- Electron route validates preload fallback incorrectly if renderer and API runtime are out of sync.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST define the VS Code integrated browser as the default interactive validation surface for frontend and Electron UX checks.
- **FR-002**: System MUST require rendered-state verification in the integrated browser before frontend/Electron tasks are considered complete.
- **FR-003**: System MUST document a one-window validation loop covering startup, rendered checks, diagnostics, and teardown.
- **FR-004**: System MUST preserve compatibility with existing compose profile entry points and runtime readiness checks.
- **FR-005**: System MUST include remediation guidance for stale image/container cases that can mask updated runtime behavior.
- **FR-006**: System MUST align governance and contributor guidance so the one-window baseline is applied consistently across new frontend slices.
- **FR-007**: System MUST require closure evidence to record integrated browser surface, rendered assertion outcome, and remediation result when a drift signal is encountered.

### Key Entities *(include if feature involves data)*

- **One-Window Validation Loop**: Ordered steps for compose startup, integrated browser validation, in-window diagnostics, and shutdown.
- **Interactive Closure Evidence**: Proof that rendered behavior was validated in the integrated browser before task completion.
- **Frontend Runtime Drift Signal**: Observable mismatch between expected and rendered runtime behavior requiring remediation.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Frontend/Electron slice closures include integrated-browser rendered verification evidence in 100% of reviewed tasks.
- **SC-002**: Developers can complete startup + rendered check + teardown without leaving VS Code for at least 90% of local validation flows.
- **SC-003**: Drift triage time for frontend runtime mismatches is reduced by at least 30% versus prior ad-hoc multi-window flow.
- **SC-004**: New contributor onboarding can execute the documented one-window loop on first attempt with no missing-step blockers.

## Assumptions

- Existing compose profile tasks remain the canonical startup/teardown mechanism.
- VS Code integrated browser remains available in the contributor environment.
- This baseline applies primarily to frontend and Electron UX behavior, not all backend-only work.
- Teams continue to maintain runbook and constitution updates when baseline workflow expectations evolve.

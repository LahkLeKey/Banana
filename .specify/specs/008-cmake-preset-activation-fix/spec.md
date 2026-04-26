# Feature Specification: CMake Preset Activation Fix

**Feature Branch**: `[008-cmake-preset-activation-fix]`  
**Created**: 2026-04-26  
**Status**: Draft  
**Input**: User description: "make a follow up spec for fixing this bug"

## In Scope *(mandatory)*

- Remove the current multi-root CMake preset activation failure that prevents CMake Tools from configuring the Banana native project.
- Ensure native build target and test discovery are available without manual one-off workaround steps.
- Preserve existing native build and test behavior while stabilizing the workspace/preset activation contract.
- Produce deterministic troubleshooting signals when preset activation fails.

## Out of Scope *(mandatory)*

- Refactoring native C source behavior unrelated to configuration activation.
- Redesigning the full Banana build pipeline outside CMake preset activation and context selection.
- Introducing new native feature behavior unrelated to the bug.
- Replacing CMake Tools with a different development workflow.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Configure Works Without Manual Preset Rescue (Priority: P1)

As a developer, I need CMake Tools configure/build commands to activate the intended preset automatically so native validation is not blocked.

**Why this priority**: This failure currently blocks native lane completion and delays release-ready validation.

**Independent Test**: Can be fully tested by invoking CMake Tools configure/build from the workspace and confirming successful configure without manual preset selection.

**Acceptance Scenarios**:

1. **Given** the Banana workspace is opened, **When** CMake Tools build is requested, **Then** configure succeeds using the intended preset.
2. **Given** the intended preset exists, **When** CMake Tools configures from either monorepo root or tests folder context, **Then** activation resolves to the same valid project context.

---

### User Story 2 - Native Targets and Tests Are Discoverable (Priority: P2)

As a developer, I need native targets and tests to be discoverable in CMake Tools so I can execute the expected native validation lane.

**Why this priority**: Discoverability is required to run the required validation lanes directly from tool-integrated flows.

**Independent Test**: Can be tested by listing targets/tests in CMake Tools and confirming expected native entries are available.

**Acceptance Scenarios**:

1. **Given** configure succeeded, **When** build targets are listed, **Then** native targets are visible.
2. **Given** configure succeeded, **When** tests are listed, **Then** native tests (including DAL contract test) are visible.

---

### User Story 3 - Actionable Failure Diagnostics (Priority: P3)

As a maintainer, I need deterministic diagnostics when preset activation fails so triage is quick and repeatable.

**Why this priority**: Current failure signals are partially silent and increase time-to-resolution.

**Independent Test**: Can be tested by intentionally misconfiguring preset activation and verifying deterministic error guidance is emitted.

**Acceptance Scenarios**:

1. **Given** preset activation is invalid, **When** CMake Tools configure is attempted, **Then** diagnostics identify missing/invalid preset activation state.
2. **Given** workspace context is mismatched, **When** configure fails, **Then** guidance identifies the expected workspace/project binding.

### Edge Cases

- What happens when the preset file exists but the default preset name is missing?
- How does the workspace behave when the tests folder is active first in a multi-root session?
- What happens after VS Code reload when prior CMake extension state is stale?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST activate a valid configure preset for CMake Tools without requiring ad hoc manual rescue steps in normal workflow.
- **FR-002**: System MUST keep workspace-folder-to-project binding deterministic for CMake configuration in this multi-root setup.
- **FR-003**: System MUST make native build targets discoverable after successful configure.
- **FR-004**: System MUST make native tests discoverable after successful configure.
- **FR-005**: System MUST emit deterministic diagnostics when preset activation is missing, invalid, or context-mismatched.
- **FR-006**: System MUST preserve successful native lane execution behavior once configure succeeds.
- **FR-007**: System MUST not require changing Banana native feature contracts to resolve this bug.

### Key Entities *(include if feature involves data)*

- **PresetActivationState**: Captures whether the expected configure/build/test presets are resolvable and active for the current workspace context.
- **WorkspaceProjectBinding**: Represents the mapping between active workspace folder context and the intended CMake project root.
- **NativeLaneDiscoveryState**: Represents target/test discovery readiness after configuration.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of local validation attempts in the documented workflow can configure through CMake Tools without manual preset recovery.
- **SC-002**: 100% of required native targets and native tests are discoverable after configuration in supported workspace contexts.
- **SC-003**: Native lane can be executed to completion from tool-integrated workflow with no preset activation blocker.
- **SC-004**: Preset activation failures surface deterministic, actionable diagnostics in one attempt.

## Assumptions

- Banana continues to use CMake presets with `default` as the primary developer preset.
- Multi-root workspace usage remains part of the supported local workflow.
- Native build/test contract remains based on existing CMake targets and tests/native suite.
- No additional runtime service dependencies are required solely to activate presets.

# Feature Specification: Frontend Config Drift Closure

**Feature Branch**: `[043-frontend-config-drift-closure]`
**Created**: 2026-04-26
**Status**: Draft
**Input**: User description: "add a spec for making the frontend work; we still have some configuration drift and the app still shows API base: <unset>"

## In Scope *(mandatory)*

- Define a deterministic runtime contract so React, Electron, and React Native web always resolve a non-empty API base URL when launched through the documented VS Code and Compose profiles.
- Add a frontend-visible runtime assertion path so `API base: <unset>` is treated as a configuration contract failure, not a silent degraded state.
- Eliminate drift between declared env wiring (Compose, launch/tasks) and actual frontend runtime values consumed by `resolveApiBaseUrl()`.
- Add diagnostics and runbook steps that let developers identify whether failure came from missing env injection, stale container/image state, or incorrect launch entry point.
- Add verification checks that validate rendered UI/runtime behavior (not only `printenv` inside containers).

## Out of Scope *(mandatory)*

- Redesigning frontend product UX beyond minimal configuration-error messaging.
- Changing API endpoint semantics or backend business logic.
- Replacing Docker Compose profile architecture introduced in prior slices.
- Building production-only deployment behavior; this slice targets local dev/runtime parity.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - React launch never shows `<unset>` on healthy startup (Priority: P1)

As a developer launching the web slice, I need the app to reliably resolve the API base so the Banana assistant is usable immediately.

**Why this priority**: `API base: <unset>` blocks all core frontend functionality and is the most visible failure in the current workflow.

**Independent Test**: Launch `Frontends: API + React` from VS Code on a clean Docker state and verify the rendered UI shows a concrete API base URL.

**Acceptance Scenarios**:

1. **Given** runtime + apps profiles are started through the documented launch path, **When** the React page loads, **Then** the UI must show `API base: http://localhost:8080` (or explicit override), never `<unset>`.
2. **Given** API base resolution fails, **When** the page initializes, **Then** the app must display a clear configuration error state with exact remediation (which env var and which launch task/profile to use).

---

### User Story 2 - Frontend launch path drift is detectable and actionable (Priority: P1)

As a developer, I need fast diagnostics that tell me where config drift occurred so I can recover without trial-and-error.

**Why this priority**: Current failures are ambiguous; container env can look correct while rendered frontend still reports unset.

**Independent Test**: Run a single troubleshooting command flow and get a concrete PASS/FAIL report for runtime source-of-truth values.

**Acceptance Scenarios**:

1. **Given** a frontend config failure, **When** diagnostics run, **Then** output identifies the failing layer (launch profile, compose env injection, Vite runtime env load, bridge fallback).
2. **Given** stale containers or stale frontend runtime state, **When** diagnostics run, **Then** output includes a deterministic cleanup/restart sequence to restore parity.

---

### User Story 3 - Cross-slice API base contract stays aligned (Priority: P2)

As a maintainer, I need one explicit contract for React/Electron/React Native web API base variables so drift does not reappear after future edits.

**Why this priority**: Multiple slices and env names are now involved; lack of one authoritative contract causes recurring divergence.

**Independent Test**: Contract docs and checks can be used to validate all slices in one pass.

**Acceptance Scenarios**:

1. **Given** compose and launch configuration changes, **When** contract validation is executed, **Then** all slices must report expected env mapping and startup order.
2. **Given** an accidental change that removes/renames a required env var, **When** validation runs, **Then** it fails with a targeted message naming the missing contract key.

### Edge Cases

- Launching frontend slice from a non-canonical entry point (manual `bun run dev`, stale task, wrong profile) while Compose containers are running.
- Existing container/image cache retains outdated runtime/env behavior after config changes.
- Host/browser sees stale frontend assets while container env has already changed.
- Runtime is healthy but frontend bridge/env resolution order produces empty string due to fallback mismatch.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST define one canonical frontend runtime API-base contract across React, Electron, and React Native web with explicit default and override keys.
- **FR-002**: System MUST verify frontend runtime API base using rendered/runtime resolution checks, not just container environment inspection.
- **FR-003**: System MUST surface a clear runtime configuration error when effective API base resolves empty.
- **FR-004**: System MUST provide a deterministic recovery sequence (cleanup + relaunch) for drift scenarios.
- **FR-005**: System MUST document and enforce canonical launch entry points for frontend slices.
- **FR-006**: System MUST include an automated guard (script/test/check) that fails when frontend config contract drifts from launch/compose wiring.
- **FR-007**: Users MUST be able to run one command or task that reports API-base resolution status for each frontend slice.

### Key Entities *(include if feature involves data)*

- **Frontend API Base Contract**: Mapping of slice -> env source(s) -> fallback precedence -> expected rendered value.
- **Launch Profile Contract**: Mapping of VS Code launch/task entry points to required compose profile startup/readiness sequence.
- **Drift Diagnostic Report**: Structured output identifying pass/fail per layer and concrete remediation steps.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: On clean startup via canonical React launch profile, UI displays non-empty API base in 100% of validation runs.
- **SC-002**: Drift diagnostics identify the failing layer and remediation in under 2 minutes for local repro cases.
- **SC-003**: Contract guard fails fast when required frontend env mapping or launch ordering is changed incorrectly.
- **SC-004**: Reported `API base: <unset>` incidents from canonical launch flow are reduced to zero for this slice.

## Assumptions

- Current compose and launch infrastructure from prior spec slices remains the base implementation and is incrementally hardened here.
- Local development continues to target host-accessible API URL defaults (`http://localhost:8080`) unless explicitly overridden.
- Bun/Vite/Electron runtime behavior remains compatible with environment-based configuration at process startup.
- The team accepts explicit runtime error messaging for misconfiguration as preferable to silent degraded behavior.

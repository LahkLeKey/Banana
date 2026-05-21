# Feature Specification: C Runtime Parallel Foundation

**Feature Branch**: `[055-c-runtime-parallel-foundation]`
**Created**: 2026-05-19
**Status**: Draft
**Jurisdiction**: package
**Agent of Record**: native core + API interop + React runtime bridge
**Supersedes**: none
**Input**: User description: "Break engine tick into parallelizable work, migrate runtime ownership from TypeScript to native C where possible, and keep renderer-agnostic C boundaries."

## In Scope *(mandatory)*

<!--
  ACTION REQUIRED: List the specific outcomes included in this feature slice.
  Keep these bounded and directly tied to user value.
-->

- Define a phase-oriented native tick pipeline with explicit C-owned stage boundaries that can be parallelized safely.
- Move multiplayer runtime reconciliation responsibilities from TypeScript loops toward native C synchronization APIs.
- Preserve renderer-agnostic runtime contracts so future renderer swaps do not require gameplay/session rewrites.

## Authority and Lineage *(mandatory)*

<!--
  ACTION REQUIRED: State the bounded context this spec governs, the agent/team
  that may execute within that jurisdiction, and whether any prior specs are
  absorbed and must be archived from the active baseline.
-->

- **Bounded Context**: `src/native/engine` runtime loop plus TypeScript interop contracts that feed authoritative player/session state.
- **Executing Authority**: Native engine owners with coordinated API/React contract updates.
- **Supersession Rule**: Extends prior overworld runtime behavior; does not archive existing 052/054 specs.

## Out of Scope *(mandatory)*

<!--
  ACTION REQUIRED: List explicit exclusions for this slice.
  Use this section to prevent implementation drift.
-->

- Replacing gameplay rules, anti-cheat policy, or room/session semantics.
- Full visual redesign of UI overlays.
- Binding to a specific alternate renderer implementation in this slice.

## User Scenarios & Testing *(mandatory)*

<!--
  IMPORTANT: User stories should be PRIORITIZED as user journeys ordered by importance.
  Each user story/journey must be INDEPENDENTLY TESTABLE - meaning if you implement just ONE of them,
  you should still have a viable MVP (Minimum Viable Product) that delivers value.

  Assign priorities (P1, P2, P3, etc.) to each story, where P1 is the most critical.
  Think of each story as a standalone slice of functionality that can be:
  - Developed independently
  - Tested independently
  - Deployed independently
-->


### Test Organization

- Describe how automated tests map to bounded domains or user stories.
- Prefer suite names that reflect the owned behavior (for example physics, AI FSM, navigation, render, world) rather than implementation phases.
- If the feature introduces native code, specify whether tests are split into domain-contract suites with a thin aggregate orchestrator.
- Call out any required coverage, sanitizer, leak, or static-analysis gates that must pass before the feature is considered complete.
### User Story 1 - Native Tick Decomposition (Priority: P1)

As an engine maintainer, I need `engine_tick` split into clear C-owned phases so parallel execution can be enabled incrementally without changing gameplay behavior.

**Why this priority**: Tick decomposition is the prerequisite for safe parallelism and long-term runtime portability.

**Independent Test**: Build native runtime and verify existing tick behavior remains deterministic while phase boundaries are explicit and callable.

**Acceptance Scenarios**:

1. **Given** an initialized runtime, **When** one tick executes, **Then** update and render phase order is explicit in C helper boundaries rather than one monolithic body.
2. **Given** the same authoritative inputs, **When** the refactored tick runs repeatedly, **Then** entity state outcomes remain stable relative to pre-refactor behavior.

---

### User Story 2 - C-First Multiplayer Sync Ownership (Priority: P2)

As a runtime developer, I need multiplayer entity reconciliation logic to live in C APIs so TypeScript is reduced to transport and UI glue.

**Why this priority**: C-first ownership reduces duplication, lowers browser-specific edge cases, and improves reuse across renderer targets.

**Independent Test**: Client snapshot handling invokes native synchronization boundaries without TypeScript-side stale-entity bookkeeping loops.

**Acceptance Scenarios**:

1. **Given** authoritative snapshot data, **When** client reconciliation runs, **Then** C APIs own remote player activation/deactivation and transform application.

---

### User Story 3 - Renderer-Agnostic Runtime Contract (Priority: P3)

As a platform engineer, I need gameplay/runtime contracts to remain renderer-agnostic so future rendering backend changes require minimal gameplay rewrites.

**Why this priority**: Long-lived architecture should decouple gameplay state progression from specific renderer choices.

**Independent Test**: Runtime APIs and docs clearly separate simulation/state ownership from presentation-channel specifics.

**Acceptance Scenarios**:

1. **Given** runtime integration surfaces, **When** reviewed, **Then** state/simulation APIs do not require renderer-specific branching for core tick logic.

---

[Add more user stories as needed, each with an assigned priority]

### Edge Cases

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right edge cases.
-->

- What happens when parallel tick mode is enabled but a stage reports unsupported thread safety?
- How does runtime recover when authoritative snapshot data is partial for a tick (missing one player update)?
- How does player reconciliation behave when a remote player disconnects mid-sync phase?

## Requirements *(mandatory)*

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right functional requirements.
-->

### Functional Requirements

- **FR-001**: Native runtime MUST expose explicit tick-stage boundaries in C so work can be parallelized incrementally.
- **FR-002**: Baseline tick mode MUST remain behaviorally equivalent after decomposition when parallel mode is disabled.
- **FR-003**: Multiplayer remote-player reconciliation MUST be owned by native C APIs rather than TypeScript-side stale bookkeeping loops.
- **FR-004**: TypeScript runtime bridge MUST limit itself to transport parsing, command routing, and non-simulation UI concerns.
- **FR-005**: Runtime contracts MUST remain renderer-agnostic, with simulation/state progression not hard-bound to a single renderer backend.
- **FR-006**: Native C runtime modules MUST follow DDD/SOLID decomposition so `engine.c` does not continue accumulating unrelated responsibilities.
- **FR-007**: Tick decomposition and multiplayer sync boundaries MUST expose small, testable units with clear ownership and dependency direction.
- **FR-008**: Native test coverage MUST improve through domain-focused tests, with coverage evidence captured from `scripts/run-native-c-tests-with-coverage.sh`.
- **FR-009**: Orchestration MUST run extension-health preflight before execution and surface failures explicitly.
- **FR-010**: Orchestration MUST enforce autonomous continuation only when confidence is >= 80%.
- **FR-011**: Orchestration MUST append heartbeat evidence for each major step with confidence and checkpoint decision.

### Orchestration Contract *(required for Spec Kit workflow slices)*

- **Preflight Command**: `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- **Confidence Gate Command**: `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- **Pause Rule**: If confidence is < 80%, stop and request human input.
- **Evidence**: Heartbeat entries in `heartbeat-log.md` and validator reports in `artifacts/spec-validation/`.

*Example of marking unclear requirements:*

- **FR-006**: System MUST authenticate users via [NEEDS CLARIFICATION: auth method not specified - email/password, SSO, OAuth?]
- **FR-007**: System MUST retain user data for [NEEDS CLARIFICATION: retention period not specified]

### Key Entities *(include if feature involves data)*

- **Tick Stage**: A named simulation or render phase with clear preconditions, outputs, and thread-safety notes.
- **Runtime Sync Window**: Native C-managed reconciliation scope for authoritative remote player updates in a frame.
- **Renderer Adapter Boundary**: Contract that consumes simulation state for presentation while keeping simulation APIs backend-neutral.

## Success Criteria *(mandatory)*

<!--
  ACTION REQUIRED: Define measurable success criteria.
  These must be technology-agnostic and measurable.
-->

### Measurable Outcomes

- **SC-001**: `engine_tick` execution flow is decomposed into explicit C stage functions with documented ownership and ordering.
- **SC-002**: At least one multiplayer reconciliation responsibility currently in TypeScript is migrated to native C APIs and validated in local runtime.
- **SC-003**: Native observability reports active player entities and remains consistent with connected-player expectations during two-client sessions.
- **SC-004**: Architecture artifacts (spec/checklist/tasks) explicitly classify simulation versus renderer boundary responsibilities.
- **SC-005**: At least one bloated concern from `engine.c` is extracted to a bounded native module with passing contract tests.
- **SC-006**: Coverage report demonstrates maintained or improved native line coverage for touched domains and is logged as evidence.

## Assumptions

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right assumptions based on reasonable defaults
  chosen when the feature description did not specify certain details.
-->

- Existing authoritative session APIs remain the source of truth for player state and input ordering.
- Parallel execution may initially be guarded behind compile/runtime flags until thread-safety validation is complete.
- Current renderer may remain visually dated during this slice as long as runtime ownership shifts to C.
- Compose/local runtime channels continue to be the primary verification environment.

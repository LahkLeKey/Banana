# Feature Specification: V3 Runtime Baseline Reset

**Feature Branch**: `[864-v3-pure-c-typescript-reset]`
**Created**: 2026-05-18
**Status**: Draft
**Jurisdiction**: federal
**Agent of Record**: Platform Architecture
**Supersedes**: 050-multiplayer-server-authority, 051-ai-gameplay-loop
**Input**: User description: ".specify/specs/052-v3-pure-c-typescript-reset"

## In Scope *(mandatory)*

- Establish one active V3 runtime baseline authority for planning and delivery decisions.
- Reframe archived or speculative lineage as reference-only input when it disagrees with the active repository baseline.
- Define the retained customer-facing gameplay scope that is allowed to move forward under the reset baseline.
- Organize downstream work into four bounded execution lanes: native gameplay, API gameplay, frontend gameplay, and shared gameplay contracts.
- Produce planning-ready guidance that lets downstream work inherit the reset baseline without reintroducing stale authority.

## Authority and Lineage *(mandatory)*

- **Bounded Context**: Cross-repository governance for the V3 runtime baseline, retained gameplay scope, and downstream planning handoff.
- **Executing Authority**: Platform Architecture coordinating the owning Banana delivery agents for native gameplay, API gameplay, frontend gameplay, and shared gameplay contracts.
- **Supersession Rule**: Absorbs 050-multiplayer-server-authority and 051-ai-gameplay-loop and keeps them as archived lineage rather than active planning authority.

## Out of Scope *(mandatory)*

- Reinstating archived implementation scope simply because it existed in prior lineage.
- Expanding V3 beyond customer-facing gameplay and the contracts needed to plan that work.
- Deleting legacy artifacts during this specification pass.
- Redesigning delivery, workflow, or internal-only initiatives that are not required to establish the active baseline.

## User Scenarios & Testing *(mandatory)*

### Test Organization

- Validate this slice through artifact review grouped by baseline authority, evidence traceability, and execution-lane readiness.
- Keep validation aligned to the owned behaviors of the reset: one active authority, evidence-backed retained scope, and bounded downstream lane ownership.
- Treat downstream planning as ready only when reviewers can confirm scope and ownership without relying on superseded active specs.

### User Story 1 - Establish one active baseline (Priority: P1)

As a platform planner, I want one authoritative V3 baseline so that new work starts from verified repository reality instead of conflicting active narratives.

**Why this priority**: Planning stays blocked until teams can tell which baseline is authoritative and which prior materials are lineage only.

**Independent Test**: Review the active spec set and confirm that one reset baseline is active, superseded active specs are archived from the baseline, and the retained gameplay scope is clearly stated.

**Acceptance Scenarios**:

1. **Given** competing active V3 narratives exist, **When** the reset baseline is adopted, **Then** one active authority remains and the competing narratives are treated as archived lineage.
2. **Given** a delivery lead needs to confirm the current V3 baseline, **When** they review the reset specification, **Then** they can identify what is authoritative without consulting superseded specs.

---

### User Story 2 - Build evidence-backed planning inputs (Priority: P2)

As a delivery lead, I want retained V3 scope grounded in current evidence so that downstream planning inherits verified capabilities instead of assumed ones.

**Why this priority**: Once authority is clear, the next risk is planning against capability claims that are no longer supported by the active baseline.

**Independent Test**: Review the baseline inventory and confirm that retained gameplay scope, excluded stale pathways, and planning guidance are all traceable to current evidence.

**Acceptance Scenarios**:

1. **Given** a retained customer-facing capability, **When** a planner reviews the reset artifacts, **Then** they can trace why it remains in scope and how it inherits from the active baseline.
2. **Given** a stale or speculative pathway, **When** a planner reviews the reset artifacts, **Then** it is clearly labeled as non-authoritative for current planning decisions.

---

### User Story 3 - Launch bounded execution lanes (Priority: P3)

As a domain owner, I want retained V3 work divided into bounded lanes so that teams can plan in parallel without overlapping authority or reintroducing excluded scope.

**Why this priority**: Parallel planning only works when ownership is explicit and every retained capability belongs to one lane until a defined stitch point.

**Independent Test**: Review the downstream planning guidance and confirm that each retained capability is assigned to one lane with clear ownership boundaries.

**Acceptance Scenarios**:

1. **Given** a retained V3 capability, **When** planners prepare downstream work, **Then** the capability maps to exactly one execution lane with a clear owner boundary.
2. **Given** work that depends on excluded or stale scope, **When** it is reviewed against the reset baseline, **Then** it is kept out of the execution lanes until a future authority expands scope.

---

### Edge Cases

- What happens when an archived lineage artifact contains useful context but conflicts with the active baseline?
- How is a capability handled when it appears customer-facing but cannot be supported by current baseline evidence?
- What happens when a retained capability appears to span multiple execution lanes and ownership is ambiguous?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The system MUST define one active baseline authority for V3 planning and delivery decisions.
- **FR-002**: The system MUST archive conflicting active baseline narratives as lineage-only references.
- **FR-003**: The system MUST state the retained customer-facing gameplay scope that remains eligible for V3 work under the reset baseline.
- **FR-004**: The system MUST distinguish authoritative baseline inputs from archived, stale, or speculative lineage inputs.
- **FR-005**: The system MUST provide evidence-backed guidance for why retained capabilities remain in scope.
- **FR-006**: The system MUST identify stale pathways and document that they do not govern current baseline decisions.
- **FR-007**: The system MUST organize retained work into exactly four execution lanes: native gameplay, API gameplay, frontend gameplay, and shared gameplay contracts.
- **FR-008**: The system MUST require each retained capability to belong to exactly one execution lane until an explicit stitch slice is defined.
- **FR-009**: The system MUST support downstream planning without requiring superseded active specs to resolve baseline-scope decisions.
- **FR-010**: The system MUST preserve lineage traceability from the reset baseline to archived superseded specs.
- **FR-011**: The multiplayer runtime MUST be server-authoritative and run a single static shared room (`overworld`) for all active clients in this baseline.
- **FR-012**: Player identity MUST be GUID-backed and stable across reconnects so anti-cheat and validation events can be attributed to one immutable identity.
- **FR-013**: Authoritative session payloads MUST expose server timing telemetry including target TPS, current TPS, average tick duration, and lag budget overrun.
- **FR-014**: Local runtime validation MUST support Docker Compose orchestration with container-level logs for API and frontend gameplay channels.
- **FR-015**: Native gameplay evolution MUST introduce a base `PlayerEntity` with immutable GUID identity and controller strategy injection (`human`, AI/NPC, automation) while preserving shared movement/state serialization.

### Orchestration Contract *(required for Spec Kit workflow slices)*

- **Preflight Command**: `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- **Confidence Gate Command**: `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- **Pause Rule**: If confidence is < 80%, stop and request human input.
- **Evidence**: Heartbeat entries in `heartbeat-log.md` and validator reports in `artifacts/spec-validation/`.

### Key Entities *(include if feature involves data)*

- **Baseline Authority**: The single active source of truth that governs what counts as current V3 scope.
- **Retained Capability**: A customer-facing gameplay behavior that remains eligible for downstream V3 planning.
- **Evidence Record**: A traceable reference that explains why a retained capability is still authoritative.
- **Execution Lane**: A bounded planning stream that owns a retained capability set without overlapping authority.
- **Archived Lineage Reference**: A superseded artifact kept for traceability but not for active planning decisions.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of active V3 planning artifacts reference one reset baseline authority, and zero conflicting baseline specs remain active.
- **SC-002**: 100% of retained capabilities in the reset inventory are assigned to exactly one execution lane.
- **SC-003**: In review, a delivery lead can trace every sampled retained capability to its supporting evidence within 10 minutes.
- **SC-004**: In a sample of 10 downstream planning decisions, at least 9 are resolved from the reset artifacts without reopening superseded active specs for baseline authority.

## Assumptions

- Archived superseded specs remain available for historical context after they are removed from the active baseline.
- Customer-facing gameplay remains the only work category eligible for inclusion in this reset unless a later authority expands scope.
- Artifact review is sufficient to establish planning readiness for this slice before any broader implementation work begins.
- Current baseline evidence remains stable long enough for downstream planning artifacts to inherit the reset decisions.

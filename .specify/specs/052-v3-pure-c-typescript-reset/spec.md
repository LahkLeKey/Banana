# Feature Specification: V3 Runtime Baseline Reset

**Feature Branch**: `[864-v3-pure-c-typescript-reset]`
**Created**: 2026-05-16
**Status**: Ready for downstream planning
**Jurisdiction**: federal
**Agent of Record**: Platform Architecture
**Supersedes**: 050-multiplayer-server-authority, 051-ai-gameplay-loop
**Input**: User description: "Reset V3 planning to a single repository-grounded runtime baseline and rebuild downstream planning artifacts from verified evidence."

## In Scope *(mandatory)*

- Establish one authoritative active baseline for V3 planning and archive conflicting active baseline specs.
- Define the retained runtime baseline in terms of native gameplay responsibilities and application orchestration responsibilities.
- Exclude stale viewport-first assumptions from baseline correctness decisions.
- Produce an evidence-backed inventory of current modules, contracts, and runtime entry points needed for customer-facing gameplay.
- Create fresh V3 planning surfaces that inherit from the new baseline authority.
- Organize retained V3 work into explicit parallel execution lanes with bounded ownership.

## Authority and Lineage *(mandatory)*

- **Bounded Context**: Cross-repository governance for the V3 gameplay baseline, evidence inventory, and planning handoff.
- **Executing Authority**: Platform Architecture and the owning Banana delivery agents for native gameplay, API gameplay, frontend gameplay, and shared gameplay contracts.
- **Supersession Rule**: Absorbs 050-multiplayer-server-authority and 051-ai-gameplay-loop and archives them from the active baseline.
- **Authoritative Inputs**: The active Spec 052 artifact set, `.specify/feature.json`, `.specify/specs/README.md`, and the live scaffold paths under `src/native/**` and `src/typescript/**`.
- **Lineage-Only Inputs**: `.specify/legacy-baseline/**`, `.legacy/**`, stale viewport-first planning notes, and any heartbeat or planning claim that is not backed by a live source path.

### Active Baseline Evidence

The retained V3 runtime baseline is intentionally scaffold-first and limited to live repository evidence:

- `src/native/CMakeLists.txt`
- `src/native/include/banana_native_v3.h`
- `src/native/scaffold/native_entry.c`
- `src/typescript/api/src/index.ts`
- `src/typescript/react/src/index.ts`
- `src/typescript/electron/main.js`
- `src/typescript/react-native/index.ts`
- `src/typescript/shared/ui/src/index.ts`

Stale viewport-first or legacy-rich assumptions are not required for baseline correctness unless they are explicitly reintroduced by a future active slice.

## Out of Scope *(mandatory)*

- Deleting legacy engine or viewport surfaces during this specification pass.
- Redesigning model, training, or workbench capabilities that are not required for customer-facing gameplay.
- Broad production delivery redesign beyond what is needed to clarify the active runtime baseline.
- Reprioritizing non-gameplay internal initiatives into the V3 roadmap.

## User Scenarios & Testing *(mandatory)*

### Test Organization

- Validate the feature through artifact-based checks grouped by lineage governance, evidence traceability, and lane readiness.
- Map each validation pass to an owned behavior: baseline authority, retained capability inventory, stale-path decisions, and planning-lane assignment.
- Require review evidence that each retained capability, archived lineage reference, and planning surface can be traced back to the reset spec without consulting superseded active specs.

### User Story 1 - Establish trusted baseline authority (Priority: P1)

As a platform planner, I want one authoritative baseline for V3 so that new work starts from verified repository reality instead of conflicting speculative specs.

**Why this priority**: Planning cannot safely continue while multiple active baseline narratives compete for authority.

**Independent Test**: Review the active spec set and confirm there is one authoritative reset spec, the superseded specs are archived from the active baseline, and the retained runtime scope is clearly stated.

**Acceptance Scenarios**:

1. **Given** conflicting active baseline specs exist, **When** the reset spec is adopted, **Then** one authoritative baseline remains active and the conflicting specs are retained only as archived lineage.
2. **Given** a delivery lead needs to confirm what counts as baseline truth, **When** they read the reset spec, **Then** they can identify retained runtime responsibilities and excluded stale assumptions without consulting archived active specs.

---

### User Story 2 - Build evidence-backed planning inputs (Priority: P2)

As a delivery lead, I want a codebase-backed inventory of retained capabilities so that V3 planning can be generated from verified facts and traceable evidence.

**Why this priority**: Once authority is clear, the next planning risk is inventing scope that does not match the current repository.

**Independent Test**: Inspect the supporting inventory and verify that each retained capability links to an evidence path and that stale pathways are explicitly identified.

**Acceptance Scenarios**:

1. **Given** a retained customer-facing capability, **When** the delivery lead reviews the inventory, **Then** they can find the evidence path that supports keeping it in the V3 baseline.
2. **Given** a stale pathway that should not govern future work, **When** the delivery lead reviews the reset artifacts, **Then** the pathway is labeled as non-authoritative with a documented migration decision.

---

### User Story 3 - Launch parallel V3 planning lanes (Priority: P3)

As a domain owner, I want retained V3 work divided into bounded execution lanes so that teams can plan in parallel without reintroducing non-gameplay coupling.

**Why this priority**: Parallel planning only works when capability boundaries are explicit and shared assumptions are stable.

**Independent Test**: Review the bootstrap planning surfaces and verify that each retained capability is assigned to one lane with clear ownership and no dependency on excluded non-gameplay flows.

**Acceptance Scenarios**:

1. **Given** a retained V3 capability, **When** planners create downstream work, **Then** the capability belongs to one defined execution lane with an explicit owner boundary.
2. **Given** a planning topic that depends on excluded non-gameplay work, **When** it is reviewed against the reset scope, **Then** it is kept out of the V3 planning lanes until a separate authority includes it.

---

### Edge Cases

- What happens when an archived spec contains details that are still useful for context but no longer authoritative for planning?
- How does the reset handle a capability that is customer-facing but currently depends on a stale pathway that should not remain authoritative?
- What happens when a retained capability appears to fit multiple planning lanes and ownership is ambiguous?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The system MUST define one authoritative active baseline for V3 planning and archive conflicting active baseline specs into lineage-only status.
- **FR-002**: The system MUST describe the retained runtime baseline in terms of the responsibilities that remain authoritative for customer-facing gameplay delivery.
- **FR-003**: The system MUST explicitly state that stale viewport-first assumptions are not required for baseline correctness.
- **FR-004**: The system MUST provide an evidence-backed inventory of retained modules, contracts, and runtime entry points that inform V3 planning.
- **FR-005**: The system MUST distinguish retained customer-facing gameplay capabilities from excluded internal-only, training, or workbench capabilities.
- **FR-006**: The system MUST create fresh planning surfaces for downstream execution that inherit from the reset baseline.
- **FR-007**: The system MUST assign each retained V3 capability to exactly one execution lane: native gameplay, API gameplay, frontend gameplay, or shared gameplay contracts.
- **FR-008**: The system MUST preserve traceable lineage from the reset baseline to each archived superseded spec.
- **FR-009**: The system MUST document stale pathways and the migration decisions that prevent them from remaining authoritative.
- **FR-010**: The system MUST support downstream planning without requiring archived speculative specs to resolve baseline-scope decisions.

### Orchestration Contract *(required for Spec Kit workflow slices)*

- **Preflight Command**: `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json`
- **Confidence Gate Command**: `.specify/scripts/bash/spec-confidence-gate.sh --confidence <n> --step "go-copilot-start" --threshold 80 --notes "startup gate"`
- **Pause Rule**: If confidence is < 80%, stop and request human input.
- **Evidence**: Heartbeat entries in `heartbeat-log.md` and validator reports in `artifacts/spec-validation/`.

### Key Entities *(include if feature involves data)*

- **Baseline Authority**: The single active source of truth that governs what is in scope for V3 planning.
- **Retained Capability**: A customer-facing gameplay behavior that remains eligible for V3 planning and must be backed by repository evidence.
- **Evidence Record**: A traceable reference that points planners to the current module, contract, or runtime entry point supporting a retained capability.
- **Execution Lane**: A bounded planning stream that owns a retained capability set without overlapping authority.
- **Archived Lineage Reference**: A superseded spec retained for traceability but not for active decision-making.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of active V3 planning artifacts reference one reset baseline as their authority, and zero conflicting baseline specs remain active.
- **SC-002**: 100% of retained customer-facing capabilities in the inventory are mapped to exactly one execution lane with no duplicate ownership.
- **SC-003**: During a planning review, a delivery lead can trace every sampled retained capability to an evidence record within 10 minutes.
- **SC-004**: In a sample of 10 downstream planning decisions, at least 9 are resolved from the reset artifacts without reopening archived speculative specs for baseline authority.

## Assumptions

- Archived superseded specs remain accessible for historical context after they are removed from the active baseline.
- Customer-facing gameplay remains the only work category eligible for inclusion in V3 planning unless a future authority expands scope.
- Artifact-based evidence review is sufficient to establish planning readiness before any broad code deletion occurs.
- Existing repository paths and capability evidence remain stable long enough to produce the reset inventory and downstream planning surfaces.

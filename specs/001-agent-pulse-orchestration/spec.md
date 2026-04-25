# Feature Specification: Agent Pulse Orchestration

**Feature Branch**: `001-agent-pulse-orchestration`  
**Created**: 2026-04-24  
**Status**: Draft  
**Input**: User description: "I would expect to see way more commits from an entire suite of agents in the GitHub Pulse page, and it needs to be management friendly so we can deep dive what agents do what. Use a custom C native implementation to guide Spec Kit with a deterministic model."

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Management Agent Visibility (Priority: P1)

A delivery manager can quickly see meaningful agent-attributed autonomous activity and understand what each agent is responsible for.

**Why this priority**: The request is explicitly management-focused and requires immediate visibility into ownership and contribution patterns.

**Independent Test**: Trigger one autonomous cycle run with the default plan and confirm management can identify multiple distinct agents, what they did, and where to inspect deeper context.

**Acceptance Scenarios**:

1. **Given** a default autonomous cycle run completes, **When** a manager inspects repository activity and the management summary, **Then** they can identify contributions from multiple distinct agents.
2. **Given** a manager sees agent-attributed commits, **When** they open the related trace artifact, **Then** they can understand each agent's purpose and affected scope without reading raw workflow internals.

---

### User Story 2 - Operational Traceability (Priority: P2)

An operator can trace each autonomous increment to the owning agent, run context, and intent for audit and triage.

**Why this priority**: Operational reliability and explainability are required for autonomous workflows, especially when changes are frequent.

**Independent Test**: Review outputs from a single run and verify every increment has an ownership marker, run metadata, and a human-readable activity record.

**Acceptance Scenarios**:

1. **Given** an autonomous increment executes, **When** an operator reviews generated records, **Then** ownership, run metadata, and intent are all present.
2. **Given** an increment is skipped due to no effective change, **When** the operator reviews activity records, **Then** the skip outcome is still visible with ownership context.

---

### User Story 3 - Spec-Driven Plan Maintainability (Priority: P3)

An engineer can update the autonomous increment catalog in one management-friendly place without editing complex inline workflow JSON.

**Why this priority**: Maintainability supports long-term velocity and reduces errors in agent orchestration updates.

**Independent Test**: Modify one increment entry in the catalog file, run validation, and confirm orchestration behavior updates without workflow syntax regressions.

**Acceptance Scenarios**:

1. **Given** an engineer updates the increment catalog, **When** orchestration runs, **Then** the updated increment definition is applied.
2. **Given** no custom input is supplied at dispatch, **When** the workflow starts, **Then** it uses the default repository-managed increment catalog.

---

### Edge Cases

- What happens when a default increment has an invalid or missing `agent:*` ownership marker?
- How does the system handle manual dispatch input that overrides the default plan with a minimal or empty increment set?
- How are management-facing activity records handled when multiple runs happen close together?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The system MUST define a repository-managed default autonomous increment catalog separate from inline workflow YAML literals.
- **FR-002**: Every default increment definition MUST carry a single explicit `agent:*` ownership marker.
- **FR-003**: The autonomous workflow MUST support manual `incremental_plan_json` override while preserving default-catalog fallback behavior.
- **FR-004**: The default autonomous plan MUST include increments attributed to multiple distinct agents, not a single-agent-only path.
- **FR-005**: Each default increment execution MUST produce management-readable activity output with agent identity, run metadata, and intent summary.
- **FR-006**: The repository MUST expose an aggregated management-friendly index that maps agents to their autonomous activity roots and snapshot conventions.
- **FR-007**: Autonomous PRs generated from increments MUST retain provenance labels required by governance, including `speckit-driven` and agent ownership.
- **FR-008**: Existing not-banana feedback and training automation increments MUST remain represented in the default catalog.
- **FR-009**: The management visibility mechanism MUST avoid requiring direct workflow YAML inspection to answer "which agents did what".
- **FR-010**: The default autonomous plan rendering path MUST be guided by a custom native C deterministic model when no manual increment override JSON is supplied.

### Key Entities *(include if feature involves data)*

- **Agent Increment Definition**: A catalog entry that declares increment ID, change command, commit metadata, and ownership labels.
- **Agent Activity Record**: A per-run record for one agent increment that includes run ID, attempt, timestamp, intent summary, and status details.
- **Agent Activity Index**: A management-facing summary that links each agent to deep-dive roots and snapshot conventions.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: A default autonomous cycle run attempts increments for at least 6 distinct agents.
- **SC-002**: 100% of default increment definitions include exactly one `agent:*` ownership marker.
- **SC-003**: Management can identify the latest activity for any participating agent in under 2 minutes using repository documentation plus run artifacts.
- **SC-004**: Updating increment orchestration defaults requires editing one catalog file instead of editing a large inline JSON literal in workflow YAML.
- **SC-005**: The native deterministic model renders at least 15 autonomous increments per default cycle while preserving explicit agent ownership labels for each increment.

## Assumptions

- The current governance model (required human reviewer and provenance labels) remains unchanged.
- Maintaining existing feedback/training increments is mandatory while extending visibility to additional agents.
- Activity records stored in repository documentation are acceptable for management auditability.
- Manual dispatch users may still provide custom increment plans for experiments, but default scheduled runs should rely on the repository-managed catalog.

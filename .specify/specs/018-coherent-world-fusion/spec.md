> **Superseded-by-031**: This spec is folded into [`031-unified-coherent-world`](../031-unified-coherent-world/spec.md) as of 2026-05-27. Do not extend this slice; extend 031 instead.

# Feature Specification: Coherent World Fusion Continuation

**Feature Branch**: `018-coherent-world-fusion`

**Created**: 2026-05-26

**Status**: Complete

**Input**: User description: "we have alot of slices, id like to combine them into one coherent game world"

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
  - Demonstrated to users independently
-->

### User Story 1 - Single World Identity Surface (Priority: P1)

As a player, all slice entry points resolve to one coherent world identity model with deterministic launch behavior.

**Why this priority**: World fragmentation is most visible at launch and transition boundaries.

**Independent Test**: Execute launch and first-transition paths from each active slice entry and assert a single canonical world identity contract.

**Acceptance Scenarios**:

1. **Given** any supported slice launch entry, **When** world bootstrap completes, **Then** a canonical coherent identity is emitted.
2. **Given** two entry paths to the same area, **When** continuity replay is requested, **Then** both paths resolve to the same authoritative area identity lineage.

---

### User Story 2 - Cross-Slice Contract Fusion (Priority: P1)

As a developer, native and API continuity contracts reject legacy drift patterns and only accept converged identity semantics.

**Why this priority**: Legacy acceptance paths are the primary risk for silent regressions.

**Independent Test**: Run focused accept/reject suites for canonical, malformed, unsupported, and legacy payload families and verify deterministic diagnostics.

**Acceptance Scenarios**:

1. **Given** canonical converged payloads, **When** commit/replay operations run, **Then** persistence and rehydration stay deterministic.
2. **Given** unsupported legacy identifiers or route semantics, **When** validation runs, **Then** requests fail with deterministic contract diagnostics.

---

### User Story 3 - Safe Evolution Lane (Priority: P2)

As a developer, adding one additional continuity attribute uses one owning path and preserves deterministic signature behavior.

**Why this priority**: Continuous expansion is required, but only safe if mutation paths stay narrow and testable.

**Independent Test**: Add a single attribute in the owning continuity schema/signature path and validate deterministic replay and reject-path coverage.

**Acceptance Scenarios**:

1. **Given** an additive continuity field, **When** signatures are recomputed, **Then** mutation is deterministic and replay remains stable.
2. **Given** disabled or unsupported values for the additive field, **When** commits are attempted, **Then** diagnostics remain deterministic across retries.

---

### Edge Cases

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right edge cases.
-->

- Legacy variant IDs with mixed case or whitespace.
- Route keys using obsolete separators or non-canonical ordering.
- Replay requests for areas whose last delta is non-continuity payload.
- Idempotency key reuse with payload-level drift.

## Requirements *(mandatory)*

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right functional requirements.
-->

### Functional Requirements

- **FR-001**: System MUST enforce one canonical coherent world identity surface across all active slice launch paths.
- **FR-002**: System MUST enforce strict converged route and variant identity semantics in continuity payload validation.
- **FR-003**: System MUST reject unsupported/legacy continuity shapes with deterministic diagnostics.
- **FR-004**: System MUST preserve deterministic commit/rehydrate behavior for canonical continuity payloads.
- **FR-005**: System MUST support additive continuity evolution via a single owning contract/signature path.
- **FR-006**: System MUST keep native scene-catalog coherence checks aligned with API continuity identity semantics.

### Key Entities *(include if feature involves data)*

- **Canonical Slice Identity**: Converged identifier set spanning variant IDs, route semantics, and area lineage.
- **Continuity Checkpoint Payload**: Owning API/native contract object for deterministic transition persistence and replay.
- **Deterministic Diagnostics Record**: Stable machine-readable reject reason bundle for unsupported or drifted payloads.

## Success Criteria *(mandatory)*

<!--
  ACTION REQUIRED: Define measurable success criteria.
  These must be technology-agnostic and measurable.
-->

### Measurable Outcomes

- **SC-001**: Focused native coherence suite passes at 100% for canonical identity and drift paths.
- **SC-002**: Continuity API contract and integration suites pass at 100% for accept/reject scenarios.
- **SC-003**: Repeated reject-path requests return stable deterministic diagnostics for unsupported legacy inputs.
- **SC-004**: One additive continuity field is introduced with changes constrained to owning contract/signature/tests.

## Assumptions

- 017 coherent-world unification outcomes remain the baseline and are not reverted.
- Existing native scene-catalog and API continuity entry points remain the primary integration seams.
- No storefront/disclosure updates are required for this slice.
- Windows native validation continues to require serial focused target builds.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: Internal runtime/API convergence only; no public claim changes.
- **Cross-Domain Contracts**: Native coherent profile/transition identity, API continuity payload service, replay merge persistence.
- **Quality Gates**: Native coherence/drift suites plus API continuity contract/integration suites, including deterministic reject paths.
- **Delivery Evidence**: Evidence captured under `artifacts/native/018-coherent-world-fusion/` and `artifacts/api/018-coherent-world-fusion/`.

## Storefront & Release Claims *(required when public Steam copy is affected)*

- Not applicable in this slice.

## Implementation Outcome

- US1 complete: canonical identity enforcement tightened for native bootstrap signature integrity and API lane identity canonicalization.
- US2 complete: legacy route delimiter drift now rejected deterministically; mixed-entry path replay lineage validated against one authoritative area identity.
- US3 complete: additive continuity evolution lane delivered via `checkpointFusionLaneTag` in owning schema/signature/test surfaces.
- Evidence:
  - `artifacts/native/018-coherent-world-fusion/continuity-runtime-evidence.md`
  - `artifacts/api/018-coherent-world-fusion/continuity-contract-evidence.md`

> **Superseded-by-031**: This spec is folded into [`031-unified-coherent-world`](../031-unified-coherent-world/spec.md) as of 2026-05-27. Do not extend this slice; extend 031 instead.

# Feature Specification: Coherent World Slice Convergence

**Feature Branch**: `017-coherent-world-unification`

**Created**: 2026-05-26

**Status**: Complete

**Input**: User description: "Combine remaining gameplay/runtime/API slices into one coherent game world"

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

### User Story 1 - Unified Slice Launch Flow (Priority: P1)

As a player, launching into the coherent world produces one connected slice flow with stable progression state and no conflicting slice identity.

**Why this priority**: Players must experience one world, not disconnected slice variants.

**Independent Test**: Run a deterministic launch + transition path across converged slices and verify one authoritative slice identity and continuity payload lineage.

**Acceptance Scenarios**:

1. **Given** converged slice metadata, **When** the world launches, **Then** one authoritative coherent profile identity is emitted.
2. **Given** a cross-slice transition sequence, **When** continuity checkpoints are committed and rehydrated, **Then** the same area identity and canonical signature lineage are preserved.

---

### User Story 2 - Runtime/API Slice Contract Convergence (Priority: P1)

As a developer, runtime coherent slice signals and API continuity contracts stay aligned even when legacy slice definitions are present.

**Why this priority**: Contract divergence is the primary source of slice fragmentation regressions.

**Independent Test**: Execute focused drift and reject-path suites with legacy and converged payload shapes to prove deterministic diagnostics and safe rejection.

**Acceptance Scenarios**:

1. **Given** converged continuity payloads, **When** API validation runs, **Then** payloads are accepted and persisted deterministically.
2. **Given** legacy or unsupported slice context payloads, **When** commit is attempted, **Then** explicit diagnostics are emitted and no silent partial persistence occurs.

---

### User Story 3 - Sustainable Slice Expansion Path (Priority: P2)

As a developer, adding a new slice-bound progression attribute requires one owning contract edit path plus targeted tests, without touching unrelated slice launch logic.

**Why this priority**: World expansion should not reintroduce brittle cross-slice rewrites.

**Independent Test**: Add one new continuity checkpoint field in the owning schema path and validate deterministic signature behavior plus diagnostics path coverage.

**Acceptance Scenarios**:

1. **Given** a new converged checkpoint field, **When** continuity signatures are recomputed, **Then** signature mutation is deterministic and replay remains stable.
2. **Given** disabled/unsupported field values, **When** validation runs, **Then** diagnostics are stable and deterministic across retries.

---

### Edge Cases

- Legacy slice route keys that do not map to converged route adjacency.
- Continuity payloads with unsupported context tags that appear valid structurally.
- Replay calls for areas with state but no valid continuity checkpoint payload in latest delta sequence.
- Concurrent idempotent writes where one write uses stale base area version.

## Requirements *(mandatory)*

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right functional requirements.
-->

### Functional Requirements

- **FR-001**: System MUST expose one authoritative coherent slice launch identity for converged world slices.
- **FR-002**: System MUST persist and rehydrate continuity checkpoints across connected slice transitions.
- **FR-003**: System MUST enforce strict runtime/API continuity schema parity and reject drift payloads.
- **FR-004**: System MUST emit deterministic diagnostics for route drift, signature drift, and unsupported context tags.
- **FR-005**: System MUST keep idempotency-safe persistence behavior for repeated continuity checkpoint commits.
- **FR-006**: System MUST support adding one new continuity field through a single owning schema/signature path.

### Key Entities *(include if feature involves data)*

- **Converged Continuity Payload**: Canonical API/runtime payload carrying route identity, transition signature, and checkpoint state.
- **Checkpoint Context Tag**: Slice-context checkpoint discriminator used for progression continuity and deterministic signature derivation.
- **Authoritative Area State**: Replayable continuity state record with canonical signature lineage.

## Success Criteria *(mandatory)*

<!--
  ACTION REQUIRED: Define measurable success criteria.
  These must be technology-agnostic and measurable.
-->

### Measurable Outcomes

- **SC-001**: Deterministic continuity round-trip suite passes across at least 10 repeated commits/rehydrations with no canonical signature drift.
- **SC-002**: Runtime/API drift and failure-path suites pass at 100% for accept and reject coverage.
- **SC-003**: Unsupported slice-context diagnostics remain deterministic across repeated retries.
- **SC-004**: Adding one new continuity field requires changes only in owning contract/signature/test surfaces.

## Assumptions

- Spec 015 coherent-world runtime profile and Spec 016 continuity contract baselines remain valid.
- Existing in-memory persistence/replay services are sufficient for this convergence slice.
- No storefront or player-facing copy updates are required.
- Native focused builds should use serial target builds to avoid Windows MSBuild lock contention.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: No public store claim changes; this slice remains runtime/API internal.
- **Cross-Domain Contracts**: Native coherent transition signals + API continuity schemas + replay persistence alignment.
- **Quality Gates**: Native focused continuity/drift suites; API contract, integration, and failure-path suites.
- **Delivery Evidence**: Runtime and API convergence evidence captured under `artifacts/native/017-coherent-world-unification/` and `artifacts/api/017-coherent-world-unification/`.

## Storefront & Release Claims *(required when public Steam copy is affected)*

- Not applicable in this slice.

## Implementation Outcome

- US1 complete: Native coherence validation now enforces catalog-authoritative profile identity and rejects variant/scene tampering.
- US2 complete: API continuity validation now enforces canonical + supported variant identities with deterministic diagnostics for unsupported/non-canonical inputs.
- US3 complete: Additive field `checkpointRouteSignatureTag` added via owning contract/signature path with deterministic mutation safety validation.
- Evidence:
  - `artifacts/native/017-coherent-world-unification/continuity-runtime-evidence.md`
  - `artifacts/api/017-coherent-world-unification/continuity-contract-evidence.md`

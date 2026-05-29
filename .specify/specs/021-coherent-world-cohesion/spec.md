> **Superseded-by-031**: This spec is folded into [`031-unified-coherent-world`](../031-unified-coherent-world/spec.md) as of 2026-05-27. Do not extend this slice; extend 031 instead.

# Feature Specification: Coherent World Cohesion Continuation

**Feature Branch**: `021-coherent-world-cohesion`

**Created**: 2026-05-26

**Status**: Complete

**Implementation Outcomes**:

- US1: Canonical profile fingerprint enforcement (`persistent_world_continuity_profile_fingerprint_mismatch`) wired through service, route contract, and drift integration tests.
- US2: Deterministic fingerprint-drift retry diagnostics validated with authoritative replay lineage stability in the continuity roundtrip integration suite.
- US3: Additive `checkpointConvergenceLaneTag` introduced through owning contract path with deterministic signature mutation and growth-path persistence/rehydration coverage.
- Validation: API contract suite 34/34 pass, API integration suite 15/15 pass, native focused coherent/transition/continuity suite 4/4 pass.

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

### User Story 1 - Canonical Profile Fingerprint Inputs (Priority: P1)

As a player, continuity writes enforce a canonical (trimmed, lowercased) profile fingerprint identity before persistence.

**Why this priority**: Mixed-case or padded fingerprints across converged slices are a primary source of replay divergence and ambiguous baseline identity.

**Independent Test**: Submit canonical and non-canonical profile fingerprint payloads and verify deterministic accept/reject behavior.

**Acceptance Scenarios**:

1. **Given** a trimmed lowercase profile fingerprint, **When** continuity is committed, **Then** persistence succeeds with deterministic transition signatures.
2. **Given** a profile fingerprint containing uppercase characters or surrounding whitespace, **When** commit is attempted, **Then** validation fails with deterministic diagnostics.

---

### User Story 2 - Replay Determinism Under Fingerprint Drift (Priority: P1)

As a developer, replay lineage remains authoritative when non-canonical fingerprint retries are interleaved with canonical commits.

**Why this priority**: Converged world lanes depend on identity-stable replay semantics to keep authoritative state versions stable.

**Independent Test**: Interleave fingerprint-drift rejects with valid commits and verify deterministic diagnostics plus stable authoritative lineage.

**Acceptance Scenarios**:

1. **Given** repeated non-canonical fingerprint retries, **When** a canonical payload is committed and replayed, **Then** one authoritative area lineage is returned.
2. **Given** repeated fingerprint drifts, **When** requests are retried, **Then** diagnostics remain deterministic across retries.

---

### User Story 3 - Additive Cohesion Attribute (Priority: P2)

As a developer, one additional checkpoint attribute can be introduced through the owning contract path without broad runtime mutation.

**Why this priority**: Safe additive evolution is required to continue coherent-world synthesis without destabilizing slices.

**Independent Test**: Add one additive field and validate deterministic signature mutation plus persistence/rehydration.

**Acceptance Scenarios**:

1. **Given** an additive cohesion field, **When** signatures are recomputed, **Then** mutation remains deterministic.
2. **Given** additive-field commits, **When** replay is requested, **Then** latest additive value rehydrates correctly.

---

### Edge Cases

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right edge cases.
-->

- Uppercase or padded profile fingerprints rejected by canonicalization.
- Repeated non-canonical fingerprint retries do not corrupt authoritative replay lineage.
- Additive cohesion field omitted by older clients (default applied).

## Requirements *(mandatory)*

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right functional requirements.
-->

### Functional Requirements

- **FR-001**: System MUST enforce canonical (trimmed, lowercased) profile fingerprint inputs.
- **FR-002**: System MUST reject non-canonical profile fingerprints with deterministic diagnostics.
- **FR-003**: System MUST preserve authoritative replay lineage when valid commits follow fingerprint-drift retries.
- **FR-004**: System MUST keep deterministic retry diagnostics for fingerprint-drift retries.
- **FR-005**: System MUST support one additive continuity checkpoint attribute through owning schema/signature paths.
- **FR-006**: System MUST keep native and API continuity invariants aligned for fingerprint cohesion.

### Key Entities *(include if feature involves data)*

- **Canonical Continuity Sequence**: Normalized checkpoint sequence identity used for deterministic continuity writes.
- **Replay Lineage State**: Authoritative continuity checkpoint history used to rehydrate area state.
- **Cohesion Checkpoint Payload**: Extensible checkpoint payload contract for cross-slice world cohesion.

## Success Criteria *(mandatory)*

<!--
  ACTION REQUIRED: Define measurable success criteria.
  These must be technology-agnostic and measurable.
-->

### Measurable Outcomes

- **SC-001**: Focused native coherence/transition continuity suites pass 100%.
- **SC-002**: API continuity contract and integration suites pass 100% across canonical sequence accept/reject paths.
- **SC-003**: Deterministic diagnostics for sequence drift remain stable across repeated retries.
- **SC-004**: One additive checkpoint field is introduced with changes confined to owning contract/signature/test surfaces.

## Assumptions

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right assumptions based on reasonable defaults
  chosen when the feature description did not specify certain details.
-->

- 020 coherent-world convergence outcomes are the baseline for this continuation.
- Native focused suites continue using serial builds on Windows.
- API continuity service remains the owning contract entry point.
- Storefront/public disclosure surfaces remain out of scope.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: Internal runtime/API convergence only; no public claim changes.
- **Cross-Domain Contracts**: Native coherent scene continuity checks and API continuity validation/replay services.
- **Quality Gates**: Focused native coherence/transition tests and API continuity contract/integration suites.
- **Delivery Evidence**: Capture deterministic outputs in `artifacts/native/021-coherent-world-cohesion/` and `artifacts/api/021-coherent-world-cohesion/`.

## Storefront & Release Claims *(required when public Steam copy is affected)*

- Not applicable in this slice.

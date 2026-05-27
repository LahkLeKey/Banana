# Feature Specification: Gameplay/API Persistence Continuity

**Feature Branch**: `016-gameplay-api-continuity`

**Created**: 2026-05-26

**Status**: Complete (2026-05-26)

**Input**: User description: "Unify coherent world gameplay progression with API persistence continuity"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Cross-Slice Progress Persists (Priority: P1)

As a player, when I move between connected coherent-world slices and return, my progression state (objective completion, checkpoint signature, and core profile state) remains intact.

**Why this priority**: It is the first end-to-end proof that the world is one persistent game instead of disconnected demos.

**Independent Test**: Run a controlled transition sequence across connected variants and verify persisted state survives the round trip.

**Acceptance Scenarios**:

1. **Given** a player completes a checkpoint in slice A, **When** they transition to slice B and back, **Then** the checkpoint is persisted and rehydrated.
2. **Given** repeated runs of the same sequence, **When** continuity validation executes, **Then** persisted signatures remain deterministic.

---

### User Story 2 - API/Runtime Contracts Stay Aligned (Priority: P1)

As a developer, I can validate that runtime coherent-world state and API persistence payloads use matching identifiers, signatures, and failure semantics.

**Why this priority**: Contract drift between native and API is the largest risk to coherent-world behavior.

**Independent Test**: Execute focused contract tests that compare runtime-generated continuity payloads against API contract validators.

**Acceptance Scenarios**:

1. **Given** a coherent transition payload, **When** API validation runs, **Then** it accepts matching schema/identifiers and rejects drift.
2. **Given** a missing or invalid continuity payload, **When** persistence is attempted, **Then** explicit diagnostics are emitted and state is not silently corrupted.

---

### User Story 3 - Persistence Growth Path Is Maintainable (Priority: P2)

As a developer, I can add one new persisted progression field through a single owning contract path without editing unrelated runtime transitions.

**Why this priority**: This ensures ongoing world growth without fragile cross-layer rewrites.

**Independent Test**: Introduce one new persisted field, run deterministic continuity tests, and verify unrelated transitions remain unchanged.

**Acceptance Scenarios**:

1. **Given** a new persistence field is added in the owning contract path, **When** transitions run, **Then** old fields remain stable and new field is persisted.
2. **Given** disabled/unsupported fields appear, **When** validation runs, **Then** diagnostics are explicit and fallback behavior is controlled.

---

### Edge Cases

- Runtime emits a continuity payload for a transition that API does not consider connected.
- API accepts stale continuity signatures from a prior route topology.
- Partial persistence succeeds (some fields committed, others rejected).
- Transition returns to a disabled coherent-world variant.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST persist coherent-world progression checkpoints across connected transitions.
- **FR-002**: System MUST rehydrate persisted progression state on return transitions.
- **FR-003**: System MUST enforce one canonical continuity payload schema shared by runtime and API layers.
- **FR-004**: System MUST validate route/variant connectivity before committing continuity payloads.
- **FR-005**: System MUST emit explicit diagnostics for rejected persistence payloads.
- **FR-006**: System MUST provide deterministic tests for continuity signature stability across repeated runs.
- **FR-007**: System MUST support adding one new persisted field via a single owning contract path without unrelated transition rewrites.

### Key Entities *(include if feature involves data)*

- **Continuity Payload**: Canonical transition persistence object (from-variant, to-variant, signature, checkpoint state).
- **Persistence Checkpoint**: Runtime progression marker committed and rehydrated across transitions.
- **Contract Version Marker**: Shared compatibility identifier for runtime/API continuity schema.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Connected transition round-trip rehydrates persisted checkpoint state with no drift in at least 10 repeated runs.
- **SC-002**: Runtime/API continuity contract tests pass 100% for both accept and reject paths.
- **SC-003**: Invalid continuity payloads produce explicit diagnostics in all covered failure-path tests.
- **SC-004**: Adding one new persisted field requires changes only in the owning continuity contract path plus targeted tests.

## Assumptions

- Coherent-world runtime baseline from spec 015 is stable and available.
- Existing API orchestration surfaces from specs 007/009 can be reused rather than redesigned.
- Initial validation can remain focused to native/API test surfaces without full frontend UX changes.
- No public storefront copy changes are required in this slice.

## Constitution Alignment *(mandatory)*

- **Disclosure Integrity**: No new public store claims are changed; behavior is internal runtime/API continuity.
- **Cross-Domain Contracts**: Native coherent-world contracts and API persistence contracts are jointly versioned and validated.
- **Quality Gates**: Deterministic continuity tests, contract drift tests, and failure-path diagnostics tests are required.
- **Delivery Evidence**: Continuity diagnostics and contract-validation evidence captured under `artifacts/api/016-gameplay-api-continuity/` and `artifacts/native/016-gameplay-api-continuity/`.

## Storefront & Release Claims *(required when public Steam copy is affected)*

- Not applicable for this slice: no public Steam copy updates are in scope.

## Implementation Outcome

- Canonical continuity contract delivered with strict schema validation and deterministic signature derivation.
- Continuity checkpoint write and rehydration endpoints delivered and validated.
- Runtime/API drift guard and failure-path diagnostics suites delivered and passing.
- Growth-path field evolution delivered via one owning contract path (`checkpointContextTag`) with mutation safety + unsupported-field diagnostics.
- Evidence captured in:
	- `artifacts/api/016-gameplay-api-continuity/continuity-contract-evidence.md`
	- `artifacts/native/016-gameplay-api-continuity/continuity-runtime-evidence.md`

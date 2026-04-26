# Feature Specification: Baseline Rehydration - Native Core Stability

**Feature Branch**: `000-native-rehydration`  
**Created**: 2026-04-25  
**Status**: Draft  
**Type**: Rehydration Specification (pre-sprint baseline)

## Summary

Define a pre-sprint native realignment baseline with intentionally pragmatic, arbitrary stability checks that reduce the risk of unreviewed contract drift. This rehydration spec verifies native ABI continuity, deterministic behavior, and safety envelopes before sprint implementation expands scope.

## In Scope

- Stability checks for native exports and status-code continuity.
- Determinism checks for core banana/not-banana native behavior.
- Memory ownership and error-handling guard checks.
- Build and native test readiness checks used as pre-sprint gating signals.

## Out of Scope

- Net-new native feature expansion.
- Redesigning native domain architecture.
- Raising strict performance targets beyond baseline stability confidence.

## User Scenarios & Testing

### User Story 1 - Verify native ABI continuity (Priority: P1)

As a maintainer, I can run a baseline native rehydration pass and confirm exported contracts remain stable.

**Why this priority**: Contract instability creates cascading failures across managed interop layers.

**Independent Test**: Validate baseline export and status mappings against previous accepted references.

**Acceptance Scenarios**:

1. **Given** a candidate code state, **When** ABI baseline checks run, **Then** unexpected export or status-code drift is flagged.
2. **Given** approved intentional ABI changes, **When** checks run, **Then** changes are explicitly reported as expected deltas.

---

### User Story 2 - Confirm deterministic native behavior (Priority: P1)

As a maintainer, I can verify repeated native inference calls remain stable for the same input.

**Why this priority**: Determinism protects regression triage quality and release confidence.

**Independent Test**: Execute repeated native calls with identical inputs and compare outputs.

**Acceptance Scenarios**:

1. **Given** identical input payloads, **When** native inference is executed repeatedly, **Then** outputs remain consistent.
2. **Given** malformed input payloads, **When** native calls execute, **Then** failures remain typed and non-crashing.

## Edge Cases

- Unintended enum/status reorder during refactors.
- Native output changes due to hidden state or non-deterministic logic.
- Memory ownership mismatches that only appear under error paths.

## Requirements

### Functional Requirements

- **FR-001**: Baseline rehydration MUST detect unexpected native export-surface drift.
- **FR-002**: Baseline rehydration MUST detect unexpected native status-code mapping drift.
- **FR-003**: Baseline rehydration MUST check deterministic output behavior for repeated identical inputs.
- **FR-004**: Baseline rehydration MUST require typed error behavior for malformed input scenarios.
- **FR-005**: Baseline rehydration MUST surface ownership or lifecycle instability signals before sprint implementation work.

### Key Entities

- **NativeContractSnapshot**: Expected export and status mapping reference used for rehydration comparison.
- **NativeDeterminismProbe**: Repeated-call check record for output consistency.
- **NativeStabilityReport**: Consolidated pass/fail report for baseline realignment checks.

## Success Criteria

- **SC-001**: Unexpected native contract drift is detected before sprint feature work begins.
- **SC-002**: Determinism probes show stable repeated outputs for baseline test inputs.
- **SC-003**: Baseline report provides clear pass/fail evidence for native rehydration readiness.

## Assumptions

- Native baseline checks are guardrails, not full product acceptance gates.
- Intentional contract changes are rare and must be explicitly annotated.
- This rehydration pass is run before or at sprint kickoff for realignment.

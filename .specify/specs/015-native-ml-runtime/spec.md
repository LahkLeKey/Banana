# Feature Specification: Native Deterministic ML Runtime

**Feature Branch**: `015-native-ml-runtime`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`006-native-core`](../006-native-core/spec.md)

## Summary

Replace native ML stubs with deterministic runtime models in C for banana vs not-banana prediction. The runtime must provide a real regression score, a binary classifier with Jaccard and confusion-matrix metrics, and a transformer-style deterministic classifier that keeps output stable across repeated runs and environments.

## User Scenarios & Testing

### User Story 1 — Deterministic regression score (Priority: P1)

As an API/service caller, I can request a banana regression score from native code and receive a stable numeric value for the same input.

**Independent Test**: Call regression export repeatedly with identical payload; value is identical and inside range [0, 1].

**Acceptance Scenarios**:

1. **Given** valid scoring input JSON, **When** regression export is called, **Then** native returns `BANANA_OK` and a numeric score in [0, 1].
2. **Given** invalid or empty input, **When** regression export is called, **Then** native returns typed validation status without crashing.

### User Story 2 — Binary classifier with explainable metrics (Priority: P1)

As an API/service caller, I can classify banana vs not-banana and receive confidence, Jaccard score, and confusion-matrix counts from native output JSON.

**Independent Test**: Call binary export with banana-heavy and non-banana-heavy examples; output includes label, confidence, jaccard, and confusion matrix fields.

**Acceptance Scenarios**:

1. **Given** banana-heavy text, **When** binary export is called, **Then** output label is `banana` with metrics payload.
2. **Given** non-banana-heavy text, **When** binary export is called, **Then** output label is `not_banana` with metrics payload.

### User Story 3 — Deterministic transformer-style classifier (Priority: P1)

As an API/service caller, I can run a transformer-style banana/not-banana classifier and receive deterministic outputs with confidence and metrics.

**Independent Test**: Execute transformer export 100 times with same input; output payload remains byte-identical.

**Acceptance Scenarios**:

1. **Given** valid input text, **When** transformer export is called, **Then** native returns deterministic label and confidence.
2. **Given** sparse or unknown-token input, **When** transformer export is called, **Then** native returns a valid fallback classification with explicit confidence.

### User Story 4 — ABI-preserving rollout (Priority: P2)

As managed interop and API consumers, existing native wrapper symbol names and status-code numerics remain stable while runtime behavior improves.

**Independent Test**: Build existing wrapper consumers without signature changes and run native unit tests.

## Edge Cases

- Empty text payload and malformed JSON payload.
- Inputs with mixed banana and non-banana tokens near decision threshold.
- Extremely long input strings where tokenization still must be bounded and deterministic.
- Inputs containing duplicate and casing-variant tokens.

## Requirements

### Functional Requirements

- **FR-001**: Native runtime MUST implement a deterministic regression model callable through `banana_predict_banana_regression_score`.
- **FR-002**: Regression score MUST be normalized to [0, 1] and deterministic for identical input.
- **FR-003**: Native runtime MUST implement a deterministic binary banana classifier callable through `banana_classify_banana_binary`.
- **FR-004**: Binary classifier output JSON MUST include `label`, `confidence`, `jaccard`, and `confusion_matrix` (`tp`, `fp`, `fn`, `tn`).
- **FR-005**: Native runtime MUST implement a deterministic transformer-style banana classifier callable through `banana_classify_banana_transformer`.
- **FR-006**: Transformer classifier MUST use fixed model parameters (no runtime randomness) so repeated runs are stable.
- **FR-007**: Runtime MUST preserve existing wrapper ABI symbols and `BananaStatusCode` numeric mappings.
- **FR-008**: ML core implementation MUST live in `src/native/core/domain/ml/{shared,regression,binary,transformer}` and wrapper bridging in `src/native/wrapper/domain/ml/{shared,regression,binary,transformer}` unless explicitly approved otherwise.
- **FR-009**: Native tests MUST verify regression bounds, binary metrics fields, transformer determinism, and status handling.
- **FR-010**: Banana signal vocabulary used in runtime scoring MUST remain compatible with `src/native/core/domain/banana_not_banana.c` drift checks.

### Hard contracts to preserve

- Public wrapper exports in `src/native/wrapper/banana_wrapper.h`.
- `BananaStatusCode` numeric values.
- Existing native build target names and CMake entry points.
- Not-banana training contract alignment (`data/not-banana/corpus.json`, `scripts/train-not-banana-model.py`, and native token references).

### Key Entities

- **Regression inference result**: scalar score in [0, 1].
- **Binary classification result**: label + confidence + Jaccard + confusion matrix.
- **Transformer classification result**: deterministic attention-weighted banana/not-banana decision payload.
- **Token feature vector**: normalized deterministic representation extracted from request input.

## Success Criteria

- **SC-001**: Native regression export returns deterministic scores with zero value drift across repeated runs for identical payloads.
- **SC-002**: Binary export output includes required metric fields in 100% of successful responses.
- **SC-003**: Transformer export produces byte-identical output for identical input across repeated invocations in a single build.
- **SC-004**: Native test suite includes explicit coverage for regression, binary metrics, and transformer determinism.
- **SC-005**: Managed/API consumers compile against unchanged native wrapper signatures.

## Assumptions

- In-scope work targets deterministic inference behavior, not full online retraining in native runtime.
- Input payloads are UTF-8 strings, with optional JSON envelopes containing text-like fields.
- Existing APIs and portals consume classification JSON payloads without requiring removal of current fields.

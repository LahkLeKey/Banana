# Feature Specification: Right Brain Binary Runnable C Code

**Feature Branch**: `[011-right-brain-binary-runnable]`
**Created**: 2026-04-26
**Status**: Draft
**Source**: 009-ml-brain-domain-spike, [readiness-packet-right-brain.md](../009-ml-brain-domain-spike/analysis/readiness-packet-right-brain.md)
**Input**: User description: "drive Right Brain binary classifier scaffold into runnable C code with operating-point evidence and negative-sample coverage"

## In Scope *(mandatory)*

- Define and lock the binary classification operating point (precision/recall target) for `is-banana` decisions and document it in `banana_ml_binary.h`.
- Expose a configurable threshold parameter (default = conservative operating point) on the binary classify entry point or a sibling entry point under `src/native/core/domain/ml/binary/`.
- Add native lane tests covering: positive-sample classification, negative-sample classification (non-banana correctly rejected), class probability `[0.0, 1.0]`, threshold sweep behavior, null-pointer rejection.
- Add a calibration step (Platt-scale-style affine remap or equivalent simple monotone rescaling) so the raw model score becomes a documented probability.
- Register a dedicated `banana_test_ml_binary` ctest target in `tests/native/CMakeLists.txt`.
- Document the operating point in both `banana_ml_binary.h` and `banana_wrapper_ml_binary.h` (precision target, recall target, threshold default).
- Lock the binary classification result JSON contract surfaced by `banana_classify_banana_binary` (fields: `model`, `label`, `confidence`, `confusion_matrix`).

## Out of Scope *(mandatory)*

- Left Brain regression work (covered by 010).
- Full Brain transformer work (covered by 012).
- Multi-class classification — Right Brain remains binary in this slice.
- Re-training of underlying weights — calibration is a post-hoc affine remap only.
- ASP.NET API contract changes (downstream slice once binary JSON contract is pinned).
- React or mobile UI display of classification labels.

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Pinned Operating Point and Calibration (Priority: P1) MVP

As a Banana native author, I need a documented operating point (precision/recall target) and a calibrated probability so the binary decision is reproducible across deployments.

**Why this priority**: RB-R02 (precision/recall balance unclear) is the SPIKE's top open uncertainty for Right Brain. Without a pinned operating point, no consumer can build on this output.

**Independent Test**: Inspect headers for documented operating point; run `banana_test_ml_binary` and confirm calibration anchor inputs map to expected probability buckets.

**Acceptance Scenarios**:

1. **Given** the binary classifier header, **When** read, **Then** it documents the chosen precision target, recall target, and threshold default.
2. **Given** a known-banana input, **When** classified, **Then** `confidence >= operating_point_threshold` and `label == "banana"`.
3. **Given** a known-not-banana input, **When** classified, **Then** `confidence < operating_point_threshold` and `label == "not_banana"`.

---

### User Story 2 - Negative-Sample Coverage (Priority: P1)

As a Banana operator, I need negative samples (non-banana inputs) to be reliably rejected so false-positive cost (RB-R04) stays bounded.

**Why this priority**: RB-R04 (false negatives on `is-banana` have downstream cost) was flagged as a required, not optional, acceptance scenario in the readiness packet.

**Independent Test**: Run `banana_test_ml_binary` and confirm at least 3 distinct negative-sample inputs all produce `label == "not_banana"`.

**Acceptance Scenarios**:

1. **Given** an obviously-not-banana payload (engine oil), **When** classified, **Then** `label == "not_banana"`.
2. **Given** an empty-text payload, **When** classified, **Then** result is deterministic and documented (rejected or `not_banana`).
3. **Given** an input with banana-adjacent but not-banana terms (banana-flavored, banana-yellow plastic), **When** classified, **Then** the documented behavior holds (this slice records the actual output as the locked contract).

---

### User Story 3 - Configurable Threshold (Priority: P2)

As a Banana operator, I need to be able to override the threshold without recompiling so per-deployment tuning (RB-R01) is possible.

**Why this priority**: RB-R01 (binary threshold may need per-deployment tuning) was identified as future-proofing risk; the existing `banana_ml_binary_classify_with_not_banana_bias` entry point partially addresses this.

**Independent Test**: Run a threshold-sweep test that calls the classifier across 3+ thresholds and confirms label monotonicity.

**Acceptance Scenarios**:

1. **Given** the same input and increasing thresholds, **When** classified, **Then** the label transitions from `banana` to `not_banana` exactly once (monotone).
2. **Given** a threshold of 0.0, **When** any banana-leaning input is classified, **Then** `label == "banana"`.
3. **Given** a threshold of 1.0, **When** any input is classified, **Then** `label == "not_banana"` unless confidence is exactly 1.0.

---

### User Story 4 - Locked JSON Contract (Priority: P2)

As a .NET interop consumer, I need the JSON shape returned by `banana_classify_banana_binary` to be locked so deserialization does not silently break.

**Why this priority**: The existing test uses substring asserts (`"model":"binary"`, `"label":"banana"`, `"confusion_matrix":`) — promoting these to an explicit locked contract prevents accidental shape drift.

**Independent Test**: Inspect `banana_test_ml_binary` for explicit field assertions on the JSON output.

**Acceptance Scenarios**:

1. **Given** any successful classification, **When** the JSON is parsed, **Then** it contains exactly: `model`, `label`, `confidence`, `confusion_matrix`, `jaccard` (current existing fields).
2. **Given** the JSON, **When** consumed by .NET interop, **Then** field names and types remain stable across releases.

## Validation Lanes

| Lane | Required | Key Assertion |
|---|---|---|
| Native (`ctest --preset default`) | Yes | New `banana_test_ml_binary` target passes (positive, negative, threshold sweep, null guard, JSON contract) |
| .NET Unit | Recommended | `INativeBananaClient` binary signature stable |
| .NET E2E Contracts | Recommended | HTTP response shape stable when binary result is surfaced |
| Coverage | Yes | Binary path contributes to 80% native threshold |

## Success Criteria

- `banana_test_ml_binary` is registered, runs in `<1s`, and passes 100%.
- Binary path coverage `>= 90%` line coverage.
- Operating point documented in header (precision target, recall target, threshold default).
- At least 3 negative-sample inputs locked in test.
- JSON output schema explicitly asserted (not substring-matched).

## Risks Inherited from SPIKE

| Risk | Mitigation in this slice |
|---|---|
| RB-R01 per-deployment threshold tuning | Configurable threshold via existing bias entry point |
| RB-R02 precision/recall unclear | Locked operating point in header |
| RB-R03 multi-class conflated with binary | Slice explicitly limits to binary; out-of-scope note |
| RB-R04 false-negative downstream cost | Negative-sample required acceptance scenario |
| U-002 runtime calibration unmeasured | Calibration anchor test |

# Feature Specification: Left Brain Regression Runnable C Code

**Feature Branch**: `[010-left-brain-regression-runnable]`
**Created**: 2026-04-26
**Status**: Draft
**Source**: 009-ml-brain-domain-spike, [readiness-packet-left-brain.md](../009-ml-brain-domain-spike/analysis/readiness-packet-left-brain.md)
**Input**: User description: "drive Left Brain regression scaffold into runnable C code with deterministic native lane evidence"
## Problem Statement

Feature Specification: Left Brain Regression Runnable C Code aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope *(mandatory)*

- Define and lock the Left Brain regression input feature contract (named feature vector slots, expected ranges, units) under `src/native/core/domain/ml/regression/`.
- Define and lock the Left Brain regression output contract: scalar score in `[0.0, 1.0]`, status code semantics, NaN/Inf rejection, clamping behavior at boundaries.
- Add deterministic native lane tests for: in-range output, monotonic ordering across reference vectors, boundary clamping (min/max/out-of-range), null-pointer rejection, NaN/Inf input rejection.
- Add wrapper ABI calibration spot-check test that pins at least 3 reference inputs to expected score buckets (low/mid/high) so regressions are caught.
- Document the consumer usage pattern (threshold guidance, ordinal-only interpretation) inline in `banana_ml_regression.h` and `banana_wrapper_ml_regression.h`.
- Register a dedicated `banana_test_ml_regression` ctest target in `tests/native/CMakeLists.txt` separate from the shared `test_ml_models.c` smoke.

## Out of Scope *(mandatory)*

- Right Brain binary classifier work (covered by 011).
- Full Brain transformer work (covered by 012).
- ASP.NET API or HTTP response shape changes — those are a separate downstream slice once contracts here are pinned.
- Frontend or UI surfacing of regression scores.
- Model retraining or weight updates — coefficient tuning beyond what the existing deterministic linear blend already provides.
- Cross-domain integration (regression score consumed as input to Right Brain or Full Brain).

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Locked Input/Output Contract (Priority: P1) MVP

As a Banana native author, I need the Left Brain regression input feature vector and output range to be locked and documented so consumers can rely on stable behavior.

**Why this priority**: Every downstream consumer (wrapper, .NET interop, future Right/Full Brain inputs) depends on a fixed contract. Without it, every other slice is blocked.

**Independent Test**: Inspect `banana_ml_regression.h` for documented input slots and output range; run a contract test that exercises every named input slot.

**Acceptance Scenarios**:

1. **Given** a fully populated `BananaMlFeatureVector`, **When** `banana_ml_regression_predict` is invoked, **Then** `*out_score` is in `[0.0, 1.0]` and the call returns `BANANA_OK`.
2. **Given** a NULL features pointer or NULL out_score pointer, **When** invoked, **Then** the call returns `BANANA_INVALID_ARGUMENT` and does not write to `*out_score`.
3. **Given** input slots containing NaN or +Inf/-Inf, **When** invoked, **Then** the call rejects with `BANANA_INVALID_ARGUMENT` (or clamps and documents the chosen behavior).

---

### User Story 2 - Deterministic Boundary and Clamping Tests (Priority: P1)

As a Banana native author, I need deterministic boundary tests so out-of-range inputs cannot leak into out-of-range scores.

**Why this priority**: LB-R02 (out-of-range scores under distribution shift) is the highest-impact regression risk identified in the SPIKE.

**Independent Test**: Run `banana_test_ml_regression` and confirm boundary assertions pass on a clean checkout.

**Acceptance Scenarios**:

1. **Given** all-zero feature vector, **When** scored, **Then** result is in `[0.0, 1.0]` (clamped) and status `BANANA_OK`.
2. **Given** all-one feature vector, **When** scored, **Then** result is in `[0.0, 1.0]` (clamped) and status `BANANA_OK`.
3. **Given** features with values intentionally outside `[0.0, 1.0]` (e.g., 5.0 or -5.0), **When** scored, **Then** the wrapper either rejects with documented status or clamps the output and stays in `[0.0, 1.0]` per the documented contract.

---

### User Story 3 - Calibration Spot-Check (Priority: P2)

As a Banana native author, I need at least 3 pinned reference inputs (clearly-banana, ambiguous, clearly-not-banana JSON payloads at the wrapper ABI) so a coefficient drift surfaces immediately in CI.

**Why this priority**: LB-R01 (calibration not pinned) is the highest day-2 maintenance risk. Without pinned references, silent drift in the linear blend is undetectable.

**Independent Test**: Run `banana_test_ml_regression` calibration block and confirm each reference falls in its expected score bucket.

**Acceptance Scenarios**:

1. **Given** a clearly-banana JSON payload, **When** scored via `banana_predict_banana_regression_score`, **Then** score > 0.65.
2. **Given** an ambiguous JSON payload, **When** scored, **Then** score is in `(0.30, 0.70)`.
3. **Given** a clearly-not-banana JSON payload (e.g., engine oil text), **When** scored, **Then** score < 0.35.

---

### User Story 4 - Documented Consumer Contract (Priority: P3)

As a downstream consumer (Right Brain, .NET interop, API), I need explicit guidance on how to interpret the scalar score so I do not invent a threshold and silently disagree with another consumer.

**Why this priority**: LB-R04 (consumer threshold misuse) is a guardrail risk, not a correctness risk, but it prevents fragmentation.

**Independent Test**: Inspect `banana_ml_regression.h` and `banana_wrapper_ml_regression.h` for documented threshold guidance and ordinal-only interpretation note.

**Acceptance Scenarios**:

1. **Given** the wrapper header, **When** read, **Then** it states the output is ordinal-only and that any threshold should be configured by the consumer with reference to the calibration anchors.

## Validation Lanes

| Lane | Required | Key Assertion |
|---|---|---|
| Native (`ctest --preset default`) | Yes | New `banana_test_ml_regression` target passes (range, boundary, calibration, null/NaN rejection) |
| .NET Unit | Recommended | Existing wrapper interop signature for regression remains stable |
| Coverage | Yes | Regression path contributes to the 80% native threshold |

## Success Criteria

- `banana_test_ml_regression` is registered, runs in `<1s`, and passes 100%.
- Regression path coverage is `>= 90%` line coverage.
- Headers document input/output contract and consumer usage pattern.
- All 3 calibration anchor inputs are pinned and named in test source.

## Risks Inherited from SPIKE

| Risk | Mitigation in this slice |
|---|---|
| LB-R01 calibration drift | Pinned anchor calibration test |
| LB-R02 out-of-range scores | Boundary + clamping tests |
| LB-R03 brittle feature engineering | Documented input contract in header |
| LB-R04 consumer threshold misuse | Documented consumer contract in header |

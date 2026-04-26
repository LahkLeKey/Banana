# Implementation Plan: Left Brain Regression Runnable C Code

**Feature**: 010-left-brain-regression-runnable
**Date**: 2026-04-26
**Source SPIKE**: 009-ml-brain-domain-spike

## Touched Files

| File | Change |
|---|---|
| `src/native/core/domain/ml/regression/banana_ml_regression.h` | Add documented input/output contract comments; add NaN/Inf guard declaration if exposed |
| `src/native/core/domain/ml/regression/banana_ml_regression.c` | Add NaN/Inf rejection guard before linear blend |
| `src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.h` | Document consumer usage pattern (ordinal-only, threshold guidance) |
| `tests/native/test_ml_regression.c` | New deterministic test: range, boundary, NaN/Inf, null, calibration anchors |
| `tests/native/CMakeLists.txt` | Register `banana_test_ml_regression` target + ctest entry |

## Phased Execution

### Phase 1: Contract Lock
1. Annotate `banana_ml_regression.h` with input slot definitions, expected ranges, and `[0.0, 1.0]` output range.
2. Annotate `banana_wrapper_ml_regression.h` with consumer usage guidance.

### Phase 2: Defensive Guards
3. Add NaN/Inf check in `banana_ml_regression.c` returning `BANANA_INVALID_ARGUMENT`.
4. Confirm clamp behavior using existing `banana_ml_clamp01`.

### Phase 3: Dedicated Test Target
5. Create `tests/native/test_ml_regression.c` with cases: null guard, NaN guard, range, all-zero, all-one, 3 calibration anchors via wrapper ABI.
6. Register in `tests/native/CMakeLists.txt`; verify discovery via `ctest -N`.

### Phase 4: Validation
7. `cmake --preset default && cmake --build --preset default && ctest --preset default --output-on-failure`.
8. Capture evidence under `analysis/`.

## Constitution / Style Notes

- Native code: ANSI C compatible, no platform headers, no dynamic allocation in hot path.
- Tests use `assert.h` and follow existing `test_ml_models.c` style.
- No new dependencies.

## Acceptance Gate

- 5/5 native tests pass (4 existing + new `banana_test_ml_regression`).
- Headers updated.
- Coverage line ratio for regression unchanged or improved.

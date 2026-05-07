# US1 — Locked input/output contract evidence

Slice: 010-left-brain-regression-runnable
Date: 2026-04 (validated against build/cmake-tools)

## Contract surfaces locked

- `src/native/core/domain/ml/regression/banana_ml_regression.h` — declares
  `banana_ml_regression_predict` with documented input ranges (all `*_ratio`
  finite, `[0.0, 1.0]`), output range (clamped `[0.0, 1.0]`), ordinal-only
  semantics, and explicit status codes.
- `src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.h` —
  declares the wrapper ABI entry with consumer threshold guidance and the
  three calibration anchors (clearly-banana > 0.65, ambiguous in (0.30, 0.70),
  clearly-not-banana < 0.35).
- `src/native/core/domain/ml/regression/banana_ml_regression.c` — implements
  the contract guard via `banana_ml_regression_is_valid_ratio` (rejects NaN,
  `±Inf`, and any value outside `[0.0, 1.0]`) before the linear blend runs.

## Test cases (banana_test_ml_regression — all green)

- `test_us1_null_input_rejected` — NULL `input_json` returns
  `BANANA_INVALID_ARGUMENT` and leaves `*out_score` untouched.
- `test_us1_null_output_rejected` — NULL `out_score` returns
  `BANANA_INVALID_ARGUMENT`.
- `test_us1_output_is_finite_and_in_range` — finite-text payload yields
  `BANANA_OK` with `score` finite and within `[0.0, 1.0]`.

## Defensive guard (not exercisable through public ABI)

The NaN/Inf rejection inside `banana_ml_regression_predict` is a defensive
contract for callers that bypass the wrapper. The JSON wrapper always emits
finite ratios, so this branch is verified by code review of
`banana_ml_regression.c` and is intentionally not reachable through
`banana_predict_banana_regression_score`.

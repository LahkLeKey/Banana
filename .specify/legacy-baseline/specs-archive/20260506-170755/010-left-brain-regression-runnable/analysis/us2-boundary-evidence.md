# US2 — Boundary and clamping evidence

Slice: 010-left-brain-regression-runnable
Date: 2026-04

## Boundary cases verified via wrapper ABI (`banana_predict_banana_regression_score`)

| Case                          | Payload (text field)                                  | Status     | Score range guarantee |
|-------------------------------|-------------------------------------------------------|------------|------------------------|
| Empty text (lower-rail)       | `""`                                                  | BANANA_OK  | `[0.0, 1.0]`          |
| Saturated banana (upper-rail) | 15× `"banana"` repeated                               | BANANA_OK  | `[0.0, 1.0]`          |
| Saturated not-banana          | 10× `"engine oil"` repeated                           | BANANA_OK  | `[0.0, 1.0]`          |
| Non-JSON freeform text        | `"not json"`                                          | BANANA_OK  | `[0.0, 1.0]`          |

All four cases are asserted in `tests/native/test_ml_regression.c` under the
`test_us2_*` set and pass under `ctest --preset default`.

## Clamp contract source

`banana_ml_regression_predict` calls `banana_ml_clamp01` on the sigmoid
output before assigning to `*out_score`, so the upper and lower rails are
hard guarantees regardless of the linear-blend magnitude.

## Out-of-range input

Out-of-range / non-finite ratio inputs at the core entrypoint return
`BANANA_INVALID_ARGUMENT` (defensive guard documented in US1 evidence).
This slice deliberately rejects rather than silently clamps so upstream
callers learn about distribution drift.

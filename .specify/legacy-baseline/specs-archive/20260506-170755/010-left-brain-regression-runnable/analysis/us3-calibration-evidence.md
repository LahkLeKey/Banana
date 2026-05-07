# US3 — Calibration anchor evidence

Slice: 010-left-brain-regression-runnable
Date: 2026-04

Anchors empirically derived against the deterministic Left Brain regression
coefficients in `src/native/core/domain/ml/regression/banana_ml_regression.c`
(linear blend → sigmoid_approx → clamp01) using a probe binary linked
against `build/cmake-tools/libbanana_native`.

## Anchor table

| Anchor                       | Payload (text field)                                                | Observed score | Pinned bound                 |
|------------------------------|---------------------------------------------------------------------|----------------|------------------------------|
| Clearly banana               | `ripe banana peel smoothie banana bunch banana bread`               | ~0.9027        | `score > 0.65`               |
| Ambiguous (empty text)       | `""` (empty string)                                                 | ~0.5890        | `0.30 < score < 0.70`        |
| Clearly not banana           | `plastic engine oil junk waste motor oil`                           | ~0.1932        | `score < 0.35`               |

## Test enforcement

Each anchor is asserted in `tests/native/test_ml_regression.c`:

- `test_us3_calibration_clearly_banana` — `score > 0.65`
- `test_us3_calibration_ambiguous` — `0.30 < score < 0.70`
- `test_us3_calibration_clearly_not_banana` — `score < 0.35`

All three pass under `ctest --preset default --output-on-failure`.

## Consumer guidance

These anchors are also referenced from
`src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.h` so
downstream consumers calibrate their own thresholds against the same
anchors rather than inventing a per-consumer threshold.

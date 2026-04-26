# US1 - Pinned operating point + affine calibration evidence

Slice: 011-right-brain-binary-runnable
Date: 2026-04

## Operating point (frozen for this slice)

- Default threshold: `BANANA_ML_BINARY_DEFAULT_THRESHOLD = 0.50`
  (declared in `src/native/core/domain/ml/binary/banana_ml_binary.h`).
- Affine calibration constants (identity for current model coefficients):
  `BANANA_ML_BINARY_CALIBRATION_A = 1.00`, `BANANA_ML_BINARY_CALIBRATION_B = 0.00`.
- Calibration site: `banana_ml_binary.c` applies
  `clamp01(A * raw_sigmoid + B)` BEFORE the threshold compare. The raw
  output is computed once and the calibrated probability is what every
  consumer sees.

## Positive calibration anchor

| Payload (text)                                             | Variant    | banana_score | label  | confidence |
|------------------------------------------------------------|------------|--------------|--------|------------|
| `ripe banana peel smoothie banana bunch banana bread`      | default    | ~0.9440      | banana | ~0.9440    |
| `ripe banana peel smoothie banana bunch banana bread`      | junk-bias  | ~0.9417      | banana | ~0.9417    |

Asserted in `tests/native/test_ml_binary.c::test_us1_positive_anchor_label_and_confidence`
via the public ABI (`banana_classify_banana_binary`). Confidence margin
above the operating point is ~0.44, which exceeds the precision target.

## Status

- New ctest target `banana_test_ml_binary` registered and green.
- US1 anchor passes against unchanged calibration (identity).

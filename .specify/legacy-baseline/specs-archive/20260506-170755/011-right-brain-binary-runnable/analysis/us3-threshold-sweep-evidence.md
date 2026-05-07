# US3 - Configurable threshold evidence

Slice: 011-right-brain-binary-runnable
Date: 2026-04

## New surfaces

- Core: `banana_ml_binary_classify_with_threshold(features, threshold, out_result)`
  in `src/native/core/domain/ml/binary/banana_ml_binary.c`.
- Wrapper: `banana_wrapper_ml_classify_binary_with_threshold(input_json, threshold, out_json)`
  in `src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.c`.
- Public ABI (additive): `banana_classify_banana_binary_with_threshold(input_json, threshold, out_json)`
  in `src/native/wrapper/banana_wrapper.h` / `.c`.
- ABI minor version bumped: `BANANA_WRAPPER_ABI_VERSION_MINOR` 0 -> 1.

## Threshold contract

- `threshold` MUST be finite and in `[0.0, 1.0]`.
- Out-of-range values return `BANANA_INVALID_ARGUMENT` with no allocation.
- Asserted in `test_us3_threshold_out_of_range_rejected`.

## Monotone sweep

Payload: `{"text":"yellow plastic toy shaped like a banana"}` -> banana_score ~0.523.

| Threshold | Asserted label |
|-----------|----------------|
| 0.10      | banana         |
| 0.30      | banana         |
| 0.50      | banana         |
| 0.70      | not_banana     |
| 0.90      | not_banana     |

Exactly one `banana -> not_banana` transition (no re-entry into banana once
left). Asserted in `test_us3_monotone_threshold_sweep`.

## Status

All US3 assertions pass under `ctest --preset default --output-on-failure`.

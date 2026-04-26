# Implementation Plan: Right Brain Binary Runnable C Code

**Feature**: 011-right-brain-binary-runnable
**Date**: 2026-04-26
**Source SPIKE**: 009-ml-brain-domain-spike

## Touched Files

| File | Change |
|---|---|
| `src/native/core/domain/ml/binary/banana_ml_binary.h` | Document operating point (precision target, recall target, threshold default); add `_with_threshold` entry point declaration if missing |
| `src/native/core/domain/ml/binary/banana_ml_binary.c` | Add affine calibration of raw score before threshold; expose threshold parameter path |
| `src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.h` | Document JSON shape contract: `model`, `label`, `confidence`, `confusion_matrix`, `jaccard` |
| `src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.c` | Ensure JSON emitter preserves field ordering and names |
| `tests/native/test_ml_binary.c` | New deterministic test: positive, 3 negative samples, threshold sweep, null guard, JSON field presence |
| `tests/native/CMakeLists.txt` | Register `banana_test_ml_binary` target + ctest entry |

## Phased Execution

### Phase 1: Operating Point Lock
1. Choose operating point values (e.g., precision >= 0.90, recall >= 0.85, default threshold = 0.55) and document in `banana_ml_binary.h`.
2. Document JSON contract in `banana_wrapper_ml_binary.h`.

### Phase 2: Calibration
3. Add affine remap (`p = a * raw + b`, clamped to `[0,1]`) before threshold comparison; expose `a`, `b` as named constants.
4. Validate that calibration anchor inputs map to expected buckets.

### Phase 3: Configurable Threshold
5. Confirm `banana_ml_binary_classify_with_not_banana_bias` (or add `_with_threshold` sibling) is callable from wrapper layer.
6. Add threshold-sweep test path.

### Phase 4: Dedicated Test Target
7. Create `tests/native/test_ml_binary.c`: positive sample, 3 negative samples, threshold sweep across [0.0, 0.5, 1.0], null guard, JSON contract assertions.
8. Register in `tests/native/CMakeLists.txt`; verify discovery via `ctest -N`.

### Phase 5: Validation
9. `cmake --preset default && cmake --build --preset default && ctest --preset default --output-on-failure`.
10. Capture evidence under `analysis/`.

## Constitution / Style Notes

- ANSI C; no dynamic allocation in classify hot path beyond existing JSON emit allocator.
- JSON emitter must remain deterministic (no map ordering surprises).
- Tests use `assert.h` plus a small JSON field-presence check (substring + colon prefix is acceptable for native lane).

## Acceptance Gate

- 6/6 native tests pass (4 existing + `banana_test_ml_regression` from 010 + new `banana_test_ml_binary`).
- Operating point and JSON contract documented.
- Coverage line ratio for binary path unchanged or improved.

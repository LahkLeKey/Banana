# Implementation Plan: Full Brain Transformer Runnable C Code

**Feature**: 012-full-brain-transformer-runnable
**Date**: 2026-04-26
**Source SPIKE**: 009-ml-brain-domain-spike

## Touched Files

| File | Change |
|---|---|
| `src/native/core/domain/ml/transformer/banana_ml_transformer.h` | Document context token schema; declare attention-map logging toggle |
| `src/native/core/domain/ml/transformer/banana_ml_transformer.c` | Enforce deterministic seed; add attention-map buffer fill path behind compile/runtime toggle |
| `src/native/wrapper/domain/ml/transformer/banana_wrapper_ml_transformer.h` | Document JSON output shape |
| `src/native/wrapper/domain/ml/transformer/banana_wrapper_ml_transformer.c` | Pass-through attention buffer when toggle is on |
| `tests/native/test_ml_transformer.c` | New deterministic test: idempotence, embedding helper, null guard, attention toggle |
| `tests/native/CMakeLists.txt` | Register `banana_test_ml_transformer` target + ctest entry |

## Phased Execution

### Phase 1: Input Contract Lock
1. Document context token schema in `banana_ml_transformer.h` (positions for Left score, Right label, raw feature vector slots).
2. Document JSON output shape in `banana_wrapper_ml_transformer.h`.

### Phase 2: Deterministic Seed
3. Audit `banana_ml_transformer.c` for any nondeterminism (time, PRNG, uninitialized memory).
4. Lock seed to a documented constant; verify byte-identical output across repeated calls.

### Phase 3: Embedding Assertion Pattern
5. Define a static helper `assert_embedding_valid(const double* vec, size_t dim, double max_norm)` inside `tests/native/test_ml_transformer.c` (or in a small shared header if reused).
6. Apply to current transformer output (even stub fingerprint vector) so the helper is exercised.

### Phase 4: Attention-Map Logging
7. Add toggle: `BANANA_TRANSFORMER_LOG_ATTENTION` env var or compile flag.
8. When enabled, populate a small attention buffer (per-row weights summing to 1.0); when disabled, no allocation.
9. Test path covers both toggle states.

### Phase 5: Dedicated Test Target
10. Create `tests/native/test_ml_transformer.c` with cases: idempotence (5 calls), embedding helper, null guard, attention on, attention off.
11. Register in `tests/native/CMakeLists.txt`; verify discovery via `ctest -N`.

### Phase 6: Validation
12. `cmake --preset default && cmake --build --preset default && ctest --preset default --output-on-failure`.
13. Capture evidence under `analysis/`.

## Constitution / Style Notes

- ANSI C; transformer body remains stub-quality but the contract surface is production-grade.
- Attention buffer allocation only when toggle is on; default off path matches existing behavior bit-for-bit.
- Tests use `assert.h` plus the new embedding helper.

## Acceptance Gate

- 7/7 native tests pass (4 existing + `banana_test_ml_regression` + `banana_test_ml_binary` + new `banana_test_ml_transformer`).
- Context token schema documented.
- Idempotence pinned across 5 calls.
- Attention toggle exercised in both states.
- Coverage line ratio for transformer path unchanged or improved.

## Cross-Spec Dependencies

- Sequential delivery suggested per SPIKE: 010 -> 011 -> 012.
- 012 may proceed in parallel with 010/011 because input format contract does not require live Left/Right outputs — only the schema slot definitions.

## Out-of-Scope but Tracked

- Quantized inference cost (U-001).
- End-to-end brain composition smoke (separate spec).
- ASP.NET HTTP surface for Full Brain output (separate spec).

# Tasks: Transformer Quantized Embedding (U-001) -- GATED

**Branch**: `016-transformer-quantized-embedding` | **Created**: 2026-04-26
**Status**: GATED. DO NOT EXECUTE until the trigger in
[../013-ml-brain-composition-spike/analysis/deferred-registry.md](../013-ml-brain-composition-spike/analysis/deferred-registry.md) fires.

## Phase 1 -- Setup

- [ ] T001 Update `.specify/feature.json` ->
  `016-transformer-quantized-embedding`.
- [ ] T002 Author `README.md` execution tracker.

## Phase 2 -- Foundational header

- [ ] T003 Add constants `BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_DIM`,
  `BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_SCALE`,
  `BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_ZERO_POINT` to
  `src/native/core/domain/ml/transformer/banana_ml_transformer.h`.
- [ ] T004 Declare `banana_ml_transformer_embedding_quantize(const double* in,
  size_t dim, int8_t* out, double* out_scale, int8_t* out_zero)` in the
  same header.
- [ ] T005 Declare wrapper export
  `banana_classify_banana_transformer_quant_embedding(const char* input_json,
  int8_t* out_quant, double* out_scale, int8_t* out_zero, char** out_json)`
  in `src/native/wrapper/banana_wrapper.h`. Bump
  `BANANA_WRAPPER_ABI_VERSION_MINOR` 2 -> 3.

## Phase 3 -- US1 Implementation

- [ ] T006 Implement `banana_ml_transformer_embedding_quantize` in
  `src/native/core/domain/ml/transformer/banana_ml_transformer.c`.
- [ ] T007 Implement
  `banana_wrapper_ml_classify_transformer_quant_embedding` in the
  transformer wrapper file.
- [ ] T008 Wire the public export in `src/native/wrapper/banana_wrapper.c`.
- [ ] T009 Build clean (`cmake --build --preset default`).

## Phase 4 -- US2 Reconstruction bound test

- [ ] T010 Create `tests/native/test_ml_transformer_quant.c` exercising
  the new export on all 6 SPIKE anchor payloads.
- [ ] T011 For each anchor, assert
  `max(abs(reconstruct(quant) - original)) < scale/2 + 1e-9`.
- [ ] T012 Register `banana_test_ml_transformer_quant` ctest target +
  Windows POST_BUILD DLL copy in `tests/native/CMakeLists.txt`.
- [ ] T013 Reconfigure (`cmake --preset default`) and build.

## Phase 5 -- US3 Default-off equivalence

- [ ] T014 Run pre-slice transformer JSON snapshot for each of the 6
  anchors (capture in evidence file).
- [ ] T015 Run post-slice transformer JSON for the same anchors and
  assert byte-equivalence (existing `_ex` ABI must be untouched).

## Phase 6 -- Validation + close-out

- [ ] T016 `ctest --preset default --output-on-failure` -- expect 8/8.
  Capture in `analysis/native-lane-evidence.md`.
- [ ] T017 Author `analysis/abi-additive-evidence.md` documenting the
  2.2 -> 2.3 bump as additive-only.
- [ ] T018 Close `tasks.md` (mark all `[x]`); update `README.md`.

## Out of scope

- Replacing existing double-precision ABI.
- ASP.NET / React consumption.
- Re-tuning transformer weights.

# Plan: Transformer Quantized Embedding (U-001) -- GATED

**Branch**: `016-transformer-quantized-embedding` | **Date**: 2026-04-26
**Status**: GATED. Trigger documented in
[../013-ml-brain-composition-spike/analysis/deferred-registry.md](../013-ml-brain-composition-spike/analysis/deferred-registry.md).

## Phases (DRAFT, do not execute until trigger fires)

- **Phase 1 (T001-T002)** Setup -- repoint `.specify/feature.json`,
  `README.md` tracker.
- **Phase 2 (T003-T005)** Header constants + helper signature
  (`BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_DIM`,
  `BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_SCALE`).
- **Phase 3 US1 (T006-T009)** Implement quantization helper in core +
  wrapper export + bump `BANANA_WRAPPER_ABI_VERSION_MINOR` 2 -> 3.
- **Phase 4 US2 (T010-T013)** Add ctest target
  `banana_test_ml_transformer_quant`; assert reconstruction error bound
  on all 6 SPIKE anchor payloads.
- **Phase 5 US3 (T014-T015)** Verify default-off equivalence: existing
  `_ex` ABI byte-equivalent before/after this slice.
- **Phase 6 (T016-T018)** Validation (8/8 ctest) + evidence + close-out.

## Out of scope

- Replacing the existing double-precision embedding ABI.
- ASP.NET or React consumption (downstream).
- Re-tuning transformer weights.

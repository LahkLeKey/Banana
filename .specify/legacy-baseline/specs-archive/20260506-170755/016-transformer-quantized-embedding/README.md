# 016 Transformer Quantized Embedding -- Execution Tracker

**Status**: COMPLETE (2026-04-27). Trigger U-001 fired by slice 017.

## Outcome

- New core helper `banana_ml_transformer_embedding_quantize` (symmetric int8, zero-point pinned to 0).
- New wrapper helper `banana_wrapper_ml_classify_transformer_quant_embedding`.
- New public ABI export `banana_classify_banana_transformer_quant_embedding`.
- ABI minor bump 2 → 3 (additive only; legacy `_ex` JSON is byte-identical).
- New ctest `banana_test_ml_transformer_quant` taking native lane 7/7 → 8/8.

See [analysis/native-lane-evidence.md](./analysis/native-lane-evidence.md) and
[analysis/abi-additive-evidence.md](./analysis/abi-additive-evidence.md).

## Trigger

Slice 014 merged AND `EnsembleVerdictResult` (or any downstream payload)
is serializing the 4-component transformer embedding to a cross-process
consumer. See
[../013-ml-brain-composition-spike/analysis/deferred-registry.md](../013-ml-brain-composition-spike/analysis/deferred-registry.md).

## Source

Built from
[../013-ml-brain-composition-spike/analysis/followup-C-quantized-embedding.md](../013-ml-brain-composition-spike/analysis/followup-C-quantized-embedding.md).

## Phases (mapped to tasks.md)

1. Setup (T001-T002)
2. Foundational header + ABI bump (T003-T005)
3. US1 Implementation (T006-T009)
4. US2 Reconstruction bound test (T010-T013)
5. US3 Default-off equivalence (T014-T015)
6. Validation + close-out (T016-T018) -- expect ctest 8/8.

## Constraints

- Additive-only ABI: 2.2 -> 2.3, no existing export changes.
- Existing `_ex` ABI MUST remain byte-identical for all SPIKE anchors.
- No JSON shape changes for any `model` value.

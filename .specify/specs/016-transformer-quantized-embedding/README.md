# 016 Transformer Quantized Embedding -- Execution Tracker

**Status**: GATED. DO NOT START until trigger fires.

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

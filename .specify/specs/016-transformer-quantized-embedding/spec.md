# Feature Specification: Transformer Quantized Embedding (U-001)

**Feature Branch**: `[016-transformer-quantized-embedding]`
**Created**: 2026-04-26
**Status**: GATED -- DO NOT START
**Trigger**: Slice 014 merged AND `EnsembleVerdictResult` (or downstream
payload) is serializing the 4-component transformer embedding to a
cross-process consumer.
**SPIKE source**: [../013-ml-brain-composition-spike/analysis/followup-C-quantized-embedding.md](../013-ml-brain-composition-spike/analysis/followup-C-quantized-embedding.md)

## In Scope

- Additive int8-quantized variant of the Full Brain transformer's 4-dim
  embedding output.
- New native export
  `banana_classify_banana_transformer_quant_embedding` returning the
  quantized vector + scale + zero-point.
- New ctest target validating reconstruction error bound.
- Minor ABI bump 2.2 -> 2.3 (additive only).

## Out of Scope

- Replacing existing double-precision embedding ABI.
- Changing JSON output shape of any model string.
- Re-tuning transformer weights.
- ASP.NET / React consumption (separate downstream slice).

## User Scenarios

### US1 -- Quantized embedding emitted (P1)
Native export returns a 4-element int8 vector + scale + zero-point that
reconstructs to the original double vector within bounded error.

### US2 -- Reconstruction error bounded (P1)
For all 6 SPIKE anchor payloads,
`max(abs(reconstruct(quant) - original)) < scale/2 + 1e-9`.

### US3 -- Default-off cost preserved (P2)
The existing `_ex` ABI is unchanged; new export is opt-in.

## Requirements

- **Q-R01**: New export MUST be additive; existing exports byte-equivalent.
- **Q-R02**: ABI bump MINOR 2 -> 3 only.
- **Q-R03**: New ctest target `banana_test_ml_transformer_quant` registered;
  native lane goes 7/7 -> 8/8.
- **Q-R04**: Reconstruction error bound asserted in test for all SPIKE
  anchor payloads.

## Validation lane

```
cmake --preset default && cmake --build --preset default
ctest --preset default --output-on-failure
# Expected: 8/8.
```

## In-scope files

See
[../013-ml-brain-composition-spike/analysis/followup-C-quantized-embedding.md](../013-ml-brain-composition-spike/analysis/followup-C-quantized-embedding.md).

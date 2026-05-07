# Follow-up C -- Quantized transformer embedding (U-001) -- GATED

## Status

**`defer-with-trigger`** -- this packet exists so the slice is
plan-ready when the trigger fires. Do NOT start work until the trigger
condition below is satisfied.

## Trigger condition

Slice 014 (ASP.NET ensemble) has been merged AND
`EnsembleVerdictResult` (or any downstream payload) is serializing the
4-component `BANANA_ML_TRANSFORMER_EMBEDDING_DIM` doubles to a
cross-process consumer.

When that lands, open this packet as the next-slice candidate.

## Objective

Add an additive int8-quantized variant of the Full Brain transformer's
embedding output to reduce the cross-process payload size while keeping
the existing double-precision ABI intact.

## In-scope files

- `src/native/core/domain/ml/transformer/banana_ml_transformer.h`
  (additive constants `BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_DIM`,
  `BANANA_ML_TRANSFORMER_EMBEDDING_QUANT_SCALE`)
- `src/native/core/domain/ml/transformer/banana_ml_transformer.c`
  (helper to derive int8 vector from the existing double vector)
- `src/native/wrapper/banana_wrapper.h`
  (additive export `banana_classify_banana_transformer_quant_embedding`)
- `src/native/wrapper/banana_wrapper.c` + corresponding wrapper domain file
- `tests/native/test_ml_transformer_quant.c` (NEW ctest target)
- `tests/native/CMakeLists.txt` (register 8th ctest target)

## Out of scope

- Replacing the existing double-precision embedding ABI.
- Changing JSON output shape of any `model` value.
- Re-tuning transformer weights or attention magnitudes.
- ASP.NET / React consumption (separate downstream slice).

## Validation lane

```
cmake --preset default
cmake --build --preset default
ctest --preset default --output-on-failure
# Expected: 8/8 (was 7/7 after 012).
```

## Contract risk

- **Additive ABI bump** -- `BANANA_WRAPPER_ABI_VERSION_MINOR` 2 -> 3.
  Pure additive: no existing export changes, no JSON shape changes.
- **Quantization correctness** -- int8 reconstruction error MUST be
  bounded; test ASSERTS `max(abs(reconstruct(quant) - original)) <
  scale / 2 + epsilon` for all anchor payloads.
- **Default-off cost** -- the quantization helper is opt-in via the
  new export only. The legacy `_ex` ABI is unchanged.

## Anchor references

- [012-full-brain-transformer-runnable/analysis/us3-embedding-helper-evidence.md](../../012-full-brain-transformer-runnable/analysis/us3-embedding-helper-evidence.md)
  (embedding shape + L2 bound contract this slice must respect)
- [deferred-registry.md](./deferred-registry.md) (trigger condition)

## Owner agent

`native-wrapper-agent` (primary), `native-core-agent` (quantization helper),
`api-interop-agent` (consumes the new export downstream after this slice).

## Spec branch (when promoted)

`016-transformer-quantized-embedding`

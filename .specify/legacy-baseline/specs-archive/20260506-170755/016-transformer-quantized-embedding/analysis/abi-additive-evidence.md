# ABI additive evidence — slice 016 (2.2 → 2.3)

## Macro bump

`src/native/wrapper/banana_wrapper.h`:

```c
#define BANANA_WRAPPER_ABI_VERSION_MAJOR 2
#define BANANA_WRAPPER_ABI_VERSION_MINOR 3   /* was 2 */
```

Comment trail records the symbol responsible:

```
Minor 1: additive `banana_classify_banana_binary_with_threshold` (slice 011).
Minor 2: additive `banana_classify_banana_transformer_ex` (slice 012).
Minor 3: additive `banana_classify_banana_transformer_quant_embedding` (slice 016).
```

## New symbol (additive)

```c
BANANA_API int banana_classify_banana_transformer_quant_embedding(
    const char* input_json,
    signed char* out_quant,
    double* out_scale,
    signed char* out_zero,
    char** out_json);
```

## No existing symbol modified

- `banana_classify_banana_transformer` — signature unchanged.
- `banana_classify_banana_transformer_ex` — signature unchanged.
- All NativeStatusCode enum values preserved (0..7).

## Byte-equivalence proof of legacy entry

`tests/native/test_ml_transformer_quant.c` `test_q_r01_legacy_ex_unchanged`
calls `banana_classify_banana_transformer_ex` and the new
`_quant_embedding` entry on the same input and asserts
`strcmp(legacy_json, quant_json) == 0`. PASSES under the 8/8 ctest run.

## Q-R01..Q-R04 mapping

| Requirement | Evidence |
|-------------|----------|
| Q-R01 (additive ABI; legacy unchanged) | strcmp byte-equivalence test passes |
| Q-R02 (symmetric int8, zero=0) | `banana_ml_transformer_embedding_quantize` pins zero-point to 0; ctest asserts `zero == 0` |
| Q-R03 (operates on all 6 SPIKE anchors) | Per-anchor loop in the new ctest |
| Q-R04 (reconstruction error < scale/2 + 1e-9) | Per-anchor, per-component assertion in the new ctest |

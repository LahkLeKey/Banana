# Quickstart -- 012 Full Brain Transformer

This slice locks the contracts and adds a default-off diagnostics surface
for the Full Brain transformer.

## Build + test (one-liner from repo root)

```
cmake --preset default
cmake --build --preset default
ctest --preset default --output-on-failure
```

Expected result: 7/7 passing tests; the new target is
`banana_test_ml_transformer`.

## Public ABI surface (post-slice)

- `banana_classify_banana_transformer(input_json, out_json)` -- unchanged.
- `banana_classify_banana_transformer_ex(input_json, log_attention,
   out_embedding, out_attention_weights, out_json)` -- NEW, additive.

`BANANA_WRAPPER_ABI_VERSION_{MAJOR,MINOR}` is now `2.2`.

## Diagnostics constants (banana_ml_transformer.h)

- `BANANA_ML_TRANSFORMER_EMBEDDING_DIM = 4`
- `BANANA_ML_TRANSFORMER_EMBEDDING_MAX_L2 = 8.0`
- `BANANA_ML_TRANSFORMER_ATTENTION_DIM = 8`
- `BANANA_ML_TRANSFORMER_BIAS = 0.05`

## Calling examples

Default (no diagnostics, hot path):
```
char* json = NULL;
int rc = banana_classify_banana_transformer("{\"text\":\"banana\"}", &json);
/* ... use json ... */
banana_free(json);
```

With embedding fingerprint capture only:
```
double embedding[BANANA_ML_TRANSFORMER_EMBEDDING_DIM];
char* json = NULL;
int rc = banana_classify_banana_transformer_ex(
    "{\"text\":\"banana\"}", /*log_attention=*/0,
    embedding, /*out_attention_weights=*/NULL, &json);
banana_free(json);
```

With attention logging on:
```
double attention[BANANA_ML_TRANSFORMER_ATTENTION_DIM];
char* json = NULL;
int rc = banana_classify_banana_transformer_ex(
    "{\"text\":\"banana\"}", /*log_attention=*/1,
    /*out_embedding=*/NULL, attention, &json);
/* sum(attention) ~= 1.0 ; weights are per-ratio-slot importance */
banana_free(json);
```

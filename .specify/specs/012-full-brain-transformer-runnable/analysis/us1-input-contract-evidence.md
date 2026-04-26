# US1 -- Input contract evidence (transformer)

## ABI under test
- `banana_classify_banana_transformer(const char* input_json, char** out_json)`
- `banana_classify_banana_transformer_ex(const char* input_json, int log_attention,
                                            double* out_embedding, double* out_attention_weights,
                                            char** out_json)`

## Cases (tests/native/test_ml_transformer.c -> test_us1_input_contract)
- NULL `input_json` is rejected with non-zero status; `*out_json` left NULL.
- NULL `out_json` is rejected with non-zero status.
- Same guards apply to the diagnostics variant.
- Well-formed `{"text":"..."}` produces a JSON block containing every locked
  field: `model`, `label`, `banana_score`, `not_banana_score`, `jaccard`,
  `confusion_matrix`.

## Result
PASS via `ctest --preset default` -> `banana_test_ml_transformer`.

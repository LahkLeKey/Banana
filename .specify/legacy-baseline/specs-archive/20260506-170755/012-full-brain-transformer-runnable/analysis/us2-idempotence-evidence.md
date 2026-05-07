# US2 -- Idempotence evidence (transformer)

## Determinism audit
- `banana_ml_transformer_classify_impl` uses only the input feature vector
  and the static, file-scoped weight constants (no PRNG, no clock,
  no thread-local state, no malloc-of-uninitialized memory).
- `BANANA_ML_TRANSFORMER_BIAS = 0.05` is the documented seed-analog bias.
- The wrapper layer wraps the same call with deterministic JSON formatting
  (`banana_wrapper_ml_result_to_json`, fixed `%.6f` precision).

## Test (tests/native/test_ml_transformer.c -> test_us2_idempotence)
For each of three payloads (clearly-banana, clearly-not-banana, empty),
`banana_classify_banana_transformer` is invoked 5 times and the resulting
JSON outputs are compared byte-for-byte with `strcmp`.

## Result
PASS -- 5/5 outputs are byte-identical for all three payloads.

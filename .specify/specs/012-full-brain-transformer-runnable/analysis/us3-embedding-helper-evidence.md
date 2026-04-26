# US3 -- Embedding helper evidence (transformer)

## Helper
`assert_embedding_valid(const double* vec, size_t dim, double max_norm)` in
`tests/native/test_ml_transformer.c`. Returns 1 only when all of the
following hold: `vec != NULL`, `dim > 0`, every component finite,
L2 norm `<= max_norm`.

## Application
The diagnostics ABI `banana_classify_banana_transformer_ex` writes the
4-component fingerprint vector
`[banana_context, not_banana_context, attention_delta, banana_probability]`
into the caller-provided buffer. The test invokes the helper with
`dim = BANANA_ML_TRANSFORMER_EMBEDDING_DIM (4)` and
`max_norm = BANANA_ML_TRANSFORMER_EMBEDDING_MAX_L2 (8.0)`.

## Failure-mode coverage
- NULL `vec` -> rejected.
- `dim == 0` -> rejected.
- NaN component -> rejected.

## Result
PASS -- helper accepts the live transformer fingerprint and rejects each
failure mode.

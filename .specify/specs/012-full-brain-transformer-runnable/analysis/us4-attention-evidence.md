# US4 -- Attention-map logging evidence (transformer)

## Toggle surface
- New ABI: `banana_classify_banana_transformer_ex(input_json, log_attention,
  out_embedding, out_attention_weights, out_json)`.
- When `log_attention == 0` (default) the attention buffer is NEVER written
  to and the JSON output is byte-for-byte equal to the legacy
  `banana_classify_banana_transformer` output (FB-R03: zero cost when off).
- When `log_attention != 0` AND `out_attention_weights` is non-NULL, the
  function writes 8 normalized weights summing to ~1.0 (one per ratio slot).

## Cases (tests/native/test_ml_transformer.c -> test_us4_attention_toggle)
- Baseline call -> JSON_BASELINE.
- Diagnostics-off + sentinel-filled buffer -> buffer untouched
  (sentinel preserved); JSON byte-identical to JSON_BASELINE.
- Diagnostics-on -> 8 weights, all finite, each in [0,1],
  `|sum - 1.0| < 1e-6`; JSON still byte-identical to JSON_BASELINE.

## Result
PASS for all three cases.

## ABI bump
`BANANA_WRAPPER_ABI_VERSION_MINOR` 1 -> 2 (additive: new export only,
existing exports unchanged).

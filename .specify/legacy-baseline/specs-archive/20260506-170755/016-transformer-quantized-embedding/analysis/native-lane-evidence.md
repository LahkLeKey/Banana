# Native lane evidence — slice 016

```
$ ctest --preset default --output-on-failure
1/8 banana_test_calc ............. Passed
2/8 banana_test_ml_models ........ Passed
3/8 banana_test_operational_domains  Passed
4/8 banana_test_dal_contracts .... Passed
5/8 banana_test_ml_regression .... Passed
6/8 banana_test_ml_binary ........ Passed
7/8 banana_test_ml_transformer ... Passed
8/8 banana_test_ml_transformer_quant Passed

100% tests passed, 0 tests failed out of 8
```

Lane size: 7/7 → 8/8. New target: `banana_test_ml_transformer_quant`.

## Per-anchor reconstruction bound

The new test exercises all 6 SPIKE anchor payloads
(see `.specify/specs/013-ml-brain-composition-spike/analysis/composition-strategy.md`):

1. ripe banana smoothie / banana bunch (cheap-path positive)
2. banana repeated 7x (cheap-path positive)
3. plastic engine oil junk (cheap-path negative)
4. yellow plastic toy shaped like a banana (escalation band)
5. yellow fruit on the counter maybe (escalation band)
6. empty payload (escalation band)

For each anchor:

- Original embedding pulled via `banana_classify_banana_transformer_ex`.
- Quantized embedding pulled via the new
  `banana_classify_banana_transformer_quant_embedding`.
- Per-component assertion: `abs((q[i] - zero) * scale - original[i]) < scale/2 + 1e-9`.

All 6 anchors pass.

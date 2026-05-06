# US2 - Negative-sample coverage evidence

Slice: 011-right-brain-binary-runnable
Date: 2026-04

Negative samples exercised through the public ABI and asserted in
`tests/native/test_ml_binary.c`:

| Test case                                          | Payload (text)                                       | Variant     | Observed banana_score | Asserted label |
|----------------------------------------------------|------------------------------------------------------|-------------|------------------------|----------------|
| `test_us2_oil_payload_rejected`                    | `plastic engine oil junk waste motor oil`            | default     | ~0.0926                | not_banana     |
| `test_us2_empty_text_documented` (default)         | `""`                                                 | default     | ~0.5833                | banana         |
| `test_us2_empty_text_documented` (junk-strict)     | `""`                                                 | junk-bias   | ~0.4310                | not_banana     |
| `test_us2_banana_adjacent_rejected_by_junk_variant`| `yellow plastic toy shaped like a banana`            | junk-bias   | ~0.3812                | not_banana     |

## Variant guidance

- `banana_classify_banana_binary` is permissive at the operating point
  (defaults to `banana` on ambiguous input).
- `banana_classify_not_banana_junk` is strict (RB-R04: bound false-positive
  cost). It correctly rejects empty-text and banana-adjacent decoys without
  any threshold tuning.

## Status

All four negative-sample assertions pass under
`ctest --preset default --output-on-failure`.

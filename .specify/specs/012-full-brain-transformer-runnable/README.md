# 012 Full Brain Transformer Runnable C Code -- Execution Tracker

Active feature pointer: `.specify/feature.json` -> `.specify/specs/012-full-brain-transformer-runnable`.

## Goal

Lock the Full Brain transformer's input/output contract (FB-R05), make
inference deterministic and idempotent (FB-R02), expose a reusable
embedding-validation pattern (FB-R04), and add an opt-in attention-map
logging hook (FB-R03) without paying any runtime cost when disabled.

## Phases (mapped to tasks.md)

- Phase 1 (T001-T003) -- Setup scaffolding (this tracker + analysis files).
- Phase 2 (T004-T006) -- Foundational headers: context token schema, JSON
  output shape, embedding dim/L2 norm bound.
- Phase 3 US1 (T007-T011) -- Input contract enforcement + ctest target.
- Phase 4 US2 (T012-T015) -- Deterministic seed + 5-call idempotence test.
- Phase 5 US3 (T016-T019) -- `assert_embedding_valid` helper + fingerprint
  vector application + helper failure-mode test.
- Phase 6 US4 (T020-T025) -- Attention-map logging toggle (default off,
  bit-for-bit equivalence) + on/off tests.
- Phase 7 (T026-T030) -- Native lane validation (7/7 ctest), evidence,
  residual U-001 quantization follow-up note, closure.

## Calibration anchors (empirically derived)

| Payload (text)                                             | banana_score | label       |
|------------------------------------------------------------|--------------|-------------|
| `ripe banana peel smoothie banana bunch banana bread`      | ~0.9507      | banana      |
| `banana banana banana banana banana banana banana banana`  | ~0.9441      | banana      |
| `yellow fruit on the counter maybe`                        | ~0.8287      | banana      |
| `""` (empty)                                               | ~0.6454      | banana      |
| `yellow plastic toy shaped like a banana` (decoy)          | ~0.4404      | not_banana  |
| `plastic engine oil junk waste motor oil`                  | ~0.0782      | not_banana  |

The transformer correctly rejects the "banana-yellow" decoy that the
default binary classifier accepts permissively -- evidence the attention
blend is providing additional signal beyond the bag-of-features path.

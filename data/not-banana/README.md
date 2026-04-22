# Not-Banana Training Corpus

This folder holds labeled samples used to train the polymorphic not-banana
signal vocabulary consumed by:

- `src/native/core/domain/banana_not_banana.c` (`k_banana_signal_tokens`)
- `src/typescript/api/src/domains/not-banana/routes.ts` (`BANANA_SIGNAL_TOKENS`)

## Format

`corpus.json` is a JSON document with a top-level `samples` array. Each entry
has:

- `label`: `BANANA` or `NOT_BANANA`
- `payload`: a polymorphic `/not-banana/junk` request body

## Training

Run the trainer from the repository root:

```bash
python3 scripts/train-not-banana-model.py \
  --corpus data/not-banana/corpus.json \
  --output artifacts/not-banana-model
```

Outputs:

- `artifacts/not-banana-model/vocabulary.json` — learned signal tokens, scores,
  and evaluation metrics.
- `artifacts/not-banana-model/banana_signal_tokens.h` — generated C header that
  can replace the inline vocabulary in
  `src/native/core/domain/banana_not_banana.c`.
- `artifacts/not-banana-model/metrics.json` — precision, recall, F1 on a
  hold-out split.

The trainer is deterministic (fixed split seed) so CI runs produce stable
artifacts unless the corpus or hyperparameters change.

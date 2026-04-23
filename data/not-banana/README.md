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

## Feedback Inbox

Use `feedback/inbox.json` to queue classifier observations for ingestion into
`corpus.json`.

- Keep entries in `pending` until reviewed.
- Mark reviewed entries as `approved` (or `rejected`) before orchestration.
- Approved entries must include human oversight metadata:
  - `reviewed_by` (string or array of GitHub usernames)
  - `reviewed_at_utc` (RFC3339 timestamp)
- Run apply command directly when needed:

```bash
python3 scripts/apply-not-banana-feedback.py \
  --feedback data/not-banana/feedback/inbox.json \
  --corpus data/not-banana/corpus.json \
  --status-filter approved \
  --require-human-review \
  --minimum-human-reviewers 1
```

Automation workflow `Orchestrate Not-Banana Feedback Loop` uses the same
script and opens a triaged PR for human approval.

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
  can be staged to
  `src/native/core/domain/generated/banana_signal_tokens.h` for native
  inference consumption.
- `artifacts/not-banana-model/metrics.json` — precision, recall, F1 on a
  hold-out split.

The trainer is deterministic (fixed split seed) so CI runs produce stable
artifacts unless the corpus or hyperparameters change.

Token quality defaults improve generalization by excluding short and
numeric-only tokens. Override with `--min-token-length` and
`--allow-numeric-tokens` when needed.

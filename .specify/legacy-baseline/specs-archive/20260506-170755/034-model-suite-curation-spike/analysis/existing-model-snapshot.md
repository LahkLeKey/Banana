# Existing Model Snapshot (G1)

## Corpus shape (`data/not-banana/corpus.json`)

```jsonc
{
    "schema_version": 1,
    "description": "Seed corpus for banana vs not-banana training.",
    "samples": [
        { "id": "banana-001", "label": "banana", "text": "...", "source": "seed" },
        { "id": "non-banana-001", "label": "not-banana", "text": "...", "source": "seed" }
    ]
}
```

- Top-level keys: `schema_version` (int), `description` (string),
  `samples` (array).
- Sample keys: `id` (string), `label` (one of `banana` /
  `not-banana`; underscores normalized via
  `train-not-banana-model.py::normalize_label`), `text`,
  `source`.

## Trainer input contract

`scripts/train-not-banana-model.py`:

- Required: `--corpus`, `--output`.
- Profiles: `ci` / `local` / `overnight` / `auto` -> session +
  vocab defaults (see `PROFILE_DEFAULTS`).
- Quality gates: `--min-signal-score` (default 0.7),
  `--min-f1` (default 0.7).
- Minimum 6 samples enforced.

## Model image format

`scripts/manage-not-banana-model-image.py`:

- Immutable image (sha256-named) + channel pointer JSON.
- Optional HMAC signature (`require_signed_images` workflow input).
- Promotion is the only path that mutates committed model state.

## API consumer

`src/c-sharp/asp.net/Pipeline/**` consumes the not-banana model
via `BANANA_NOT_BANANA_MODEL_PATH` (env-driven path resolution
with ancestor search fallback).

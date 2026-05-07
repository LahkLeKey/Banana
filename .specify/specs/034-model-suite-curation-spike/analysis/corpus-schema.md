# Corpus Schema (G3)

## Convention

Every model under `data/<model>/corpus.json` MUST be a JSON
object with these top-level keys:

- `schema_version` (int, currently `1`).
- `description` (string).
- `samples` (array of sample objects).
- (optional) `labels` (array of allowed label strings; if absent,
  derived from `samples[].label`).
- (optional) `splits` (object with `train` / `eval` arrays of
  sample `id`s; if absent, the trainer derives a deterministic
  hold-out split).

Each sample MUST have `id`, `label`, `text`. May have `source`,
`tags`, free-form metadata.

## Per-model conventions

### `data/banana/corpus.json` (binary positive class)

- Labels: `banana` / `not-banana` (mirrors not-banana for back-compat).
- Difference from `data/not-banana/`: weighted toward banana-positive
  text from the production verdict log; not-banana samples are a
  minimum balanced set drawn from the existing corpus.
- Trainer: `scripts/train-banana-model.py` (mirrors
  not-banana; sees corpus path, emits artifact in same shape).

### `data/ripeness/corpus.json` (multi-class)

- Labels: `unripe`, `ripe`, `overripe`, `spoiled`.
- `splits` recommended (eval set must include all four labels).
- Trainer: `scripts/train-ripeness-model.py`.

## Back-compat

`data/not-banana/corpus.json` already conforms to the convention
(`schema_version=1`, `samples[]` with `id` + `label` + `text`).
No migration required; existing trainer keeps working unchanged.

## Schema version evolution

Bump `schema_version` only when a breaking field rename or removal
happens. Adding optional fields stays at v1.

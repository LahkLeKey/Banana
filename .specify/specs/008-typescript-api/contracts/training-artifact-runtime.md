# Not-Banana Training Artifact Runtime Contract (008)

## Scope

This contract defines the files produced by `scripts/train-not-banana-model.py` and consumed by:

- `src/typescript/api/src/domains/not-banana/model.ts`
- `src/typescript/api/src/domains/not-banana/routes.ts`
- `src/typescript/api/src/domains/chat/routes.ts`
- `scripts/validate-not-banana-runtime-contract.py`

## Canonical Schema Version

- `schema_version` (all artifact files): `1`
- Runtime behavior on mismatch: TypeScript runtime must switch to `builtin-fallback` and expose `fallback_reason`.
- CI behavior on mismatch: contract validator fails the workflow.

## Required Artifact Set

For every model output directory (`artifacts/not-banana-model/<release-id>/`), the following files are required:

- `vocabulary.json`
- `metrics.json`
- `sessions.json`
- `banana_signal_tokens.h`

## `vocabulary.json` (schema `1`)

Required top-level fields:

- `schema_version: number`
- `generated_at_utc: string`
- `vocab_size: number`
- `vocabulary: Array<VocabularyEntry>`
- `metrics: object`

`VocabularyEntry` required fields:

- `token: string`
- `banana_count: number`
- `not_banana_count: number`
- `support: number`
- `weight: number`
- `signal: number`

Runtime invariants:

- `vocab_size === vocabulary.length`
- token values are non-empty and unique
- `weight` is finite numeric for every token

## `metrics.json` (schema `1`)

Required top-level fields:

- `schema_version: number`
- `generated_at_utc: string`
- `metrics: object`

Required `metrics` fields:

- `min_signal_score`
- `min_f1`
- `holdout_f1`
- `holdout_accuracy`
- `meets_thresholds`

Consistency requirements:

- Shared metrics fields must match `vocabulary.json.metrics` for the same artifact release.

## `sessions.json` (schema `1`)

Required top-level fields:

- `schema_version: number`
- `generated_at_utc: string`
- `sessions: Array<object>`

Session invariants:

- `sessions` must be non-empty
- each session entry must include `session_index` and `metrics`

## `banana_signal_tokens.h`

Required export:

- `k_banana_signal_tokens_generated[]`

Consistency requirement:

- token set in `k_banana_signal_tokens_generated[]` must exactly match the token set in `vocabulary.json`.

## API Surface Requirements

`/not-banana/model` and `/not-banana/score` responses must include:

- `source`
- `recommended_threshold`
- `artifact_path` (when trained artifact is loaded)
- `fallback_reason` (when `source = builtin-fallback`)

Typed rejection behavior:

- `/not-banana/score` rejects zero-signal input with `error_type = invalid_argument`.

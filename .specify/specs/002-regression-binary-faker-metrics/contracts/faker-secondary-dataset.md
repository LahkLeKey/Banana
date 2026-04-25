# Contract: Faker Secondary Dataset

## Purpose

Define deterministic synthetic dataset generation used as a secondary comparison set for binary evaluation.

## Generation Config Schema

- `run_id` (string, required): Generation run identifier.
- `seed` (integer, required): Deterministic random seed.
- `schema_version` (string, required): Shared schema version to target.
- `locale_set` (array[string], optional): Faker locales.
- `class_balance` (object, required): Target distribution for labels.
- `sample_count` (integer, required): Number of records to generate.
- `generation_profile` (string, required): Named profile for reproducibility.

## Output Schema

Output records MUST conform to `regression-binary-shared-dataset.md` plus:

- `synthetic` (boolean, required): Always `true`.
- `seed` (integer, required): Echoed seed used for generation.
- `generation_profile` (string, required): Echoed profile name.

## Validation Rules

- Fixed seed + fixed config MUST produce deterministic equivalent output.
- Output records MUST satisfy shared schema required fields and enums.
- Label distribution MUST be within configured tolerance bounds.

## Behavioral Guarantees

- Generation emits a manifest containing config digest, seed, and output counts.
- Synthetic data is explicitly tagged and never mixed as unlabeled primary data.
- Runs with missing/invalid config fail before record emission.

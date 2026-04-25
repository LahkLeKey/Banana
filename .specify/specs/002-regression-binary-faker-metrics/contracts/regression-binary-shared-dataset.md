# Contract: Regression-to-Binary Shared Dataset

## Purpose

Define the shared projection schema used to transfer regression-derived records into binary classification workflows.

## Input Requirements

- Source dataset MUST conform to `scraper-regression-corpus.md`.
- Input dataset MUST include a source dataset version manifest.

## Projection Schema

- `sample_id` (string, required): Unique ID for projected sample.
- `source_record_id` (string, required): Foreign key to regression corpus `record_id`.
- `schema_version` (string, required): Semantic version for shared schema.
- `text` (string, required): Text payload used by binary model.
- `binary_label` (string, required): Allowed values `banana`, `not-banana`, `unknown`.
- `label_origin` (string, required): Allowed values `rule`, `human`, `model-proxy`.
- `shared_features` (object, required): Feature payload agreed by both model paths.
- `source_dataset_version` (string, required): Version ID of originating regression dataset.

## Compatibility Rules

- MAJOR schema mismatch MUST fail projection.
- MINOR schema changes MUST remain backward-compatible.
- `unknown` labels MUST be excluded from binary training split unless explicitly overridden.

## Validation Rules

- Every projected record MUST resolve `source_record_id` in source corpus.
- `schema_version` MUST match gate policy compatibility matrix.
- `binary_label` and `label_origin` MUST be from allowed enums.
- Missing required fields MUST fail projection.

## Behavioral Guarantees

- Projection emits dataset manifest with record counts by label.
- Coverage statistics (mapped, dropped, rejected) are generated per run.
- Projection output is deterministic for fixed input dataset and ruleset version.

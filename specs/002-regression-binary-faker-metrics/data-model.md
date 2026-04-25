# Data Model: Regression Corpus Sharing and Evaluation Gates

## Entity: ScraperSourcePolicy

- Purpose: Defines approved source boundaries and crawl governance for corpus ingestion.
- Fields:
  - `policy_id`
  - `allowed_domains`
  - `path_allow_rules`
  - `path_block_rules`
  - `content_license_requirements`
  - `effective_from_utc`
  - `effective_to_utc` (optional)

## Entity: RegressionCorpusRecord

- Purpose: Canonical normalized record from scraper ingestion for regression training.
- Fields:
  - `record_id`
  - `source_url`
  - `source_domain`
  - `fetched_at_utc`
  - `raw_text`
  - `normalized_text`
  - `regression_target` (optional)
  - `feature_tokens`
  - `content_hash_sha256`
  - `provenance`

## Entity: SharedProjectionRecord

- Purpose: Shared versioned projection consumed by binary classification workflows.
- Fields:
  - `sample_id`
  - `source_record_id`
  - `schema_version`
  - `binary_label`
  - `label_origin`
  - `shared_features`
  - `source_dataset_version`

## Entity: FakerSecondaryRecord

- Purpose: Deterministic synthetic sample aligned with shared projection schema.
- Fields:
  - `sample_id`
  - `synthetic` (always `true`)
  - `seed`
  - `generation_profile`
  - `binary_label`
  - `shared_features`
  - `schema_version`

## Entity: EvaluationMetricReport

- Purpose: Captures run-scoped quality metrics and gate decisions.
- Fields:
  - `report_id`
  - `run_id`
  - `evaluated_at_utc`
  - `dataset_primary_version`
  - `dataset_secondary_version`
  - `jaccard_similarity`
  - `confusion_matrix` (`tp`, `fp`, `tn`, `fn`)
  - `derived_metrics` (`precision`, `recall`, `f1`, `accuracy`)
  - `gate_policy_version`
  - `gate_passed`
  - `gate_failures`
  - `human_summary_ref`
  - `ai_audit_ref`

## Entity: GatePolicy

- Purpose: Versioned threshold configuration for evaluation pass/fail behavior.
- Fields:
  - `policy_version`
  - `jaccard_min`
  - `precision_min`
  - `recall_min`
  - `f1_min`
  - `accuracy_min`
  - `enforcement_mode`

## State Transitions

1. Policy loaded -> `ScraperSourcePolicy` validates crawl boundaries.
2. Scraping run completes -> `RegressionCorpusRecord` dataset version published.
3. Projection runs -> `SharedProjectionRecord` dataset version emitted.
4. Faker generation runs -> `FakerSecondaryRecord` dataset emitted with seed/config manifest.
5. Evaluation runs -> `EvaluationMetricReport` computed against `GatePolicy`.
6. Gate decision emitted -> workflow succeeds or fails based on threshold policy.

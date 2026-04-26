# Contract: Jaccard and Confusion Matrix Gates

## Purpose

Define metric computation outputs and gate thresholds for comparing primary scraped data and secondary Faker data in binary evaluation.

## Inputs

- Primary dataset: shared projection dataset version.
- Secondary dataset: Faker-generated dataset version.
- Prediction output: binary classifier predictions for evaluated records.
- Gate policy: threshold configuration document.

## Metric Output Schema

- `report_id` (string, required)
- `run_id` (string, required)
- `evaluated_at_utc` (string, required)
- `primary_dataset_version` (string, required)
- `secondary_dataset_version` (string, required)
- `jaccard_similarity` (number, required, range 0.0-1.0)
- `confusion_matrix` (object, required):
  - `tp` (integer)
  - `fp` (integer)
  - `tn` (integer)
  - `fn` (integer)
- `derived_metrics` (object, required):
  - `precision` (number)
  - `recall` (number)
  - `f1` (number)
  - `accuracy` (number)
- `gate_policy_version` (string, required)
- `gate_passed` (boolean, required)
- `gate_failures` (array[string], required)
- `human_summary_ref` (string, required)
- `ai_audit_ref` (string, required)

## Threshold Policy Schema

- `jaccard_min` (number, required)
- `precision_min` (number, required)
- `recall_min` (number, required)
- `f1_min` (number, required)
- `accuracy_min` (number, required)

## Gate Behavior

- If any threshold is unmet, `gate_passed` MUST be `false` and workflow MUST fail.
- Failures MUST include explicit metric deltas in `gate_failures`.
- If metrics cannot be computed (for example zero valid labels), evaluation MUST fail with explicit reason.

## Behavioral Guarantees

- Every evaluation run produces one report artifact in a stable schema.
- Metric computation logic is deterministic for fixed datasets and predictions.
- Gate decision output is machine-readable and management-readable.
- Published outputs preserve both human-readable and AI-audit lanes without losing gate trace detail.

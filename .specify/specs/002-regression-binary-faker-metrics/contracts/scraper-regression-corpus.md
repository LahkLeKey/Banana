# Contract: Scraper Regression Corpus

## Purpose

Define the canonical output schema and validation rules for web-scraped regression corpus records.

## Output Format

- File format: JSON Lines (`.jsonl`)
- One object per line
- UTF-8 encoding

## Record Schema

- `record_id` (string, required): Stable unique ID for the scraped record.
- `source_url` (string, required): Absolute source URL.
- `source_domain` (string, required): Source domain extracted from URL.
- `fetched_at_utc` (string, required): ISO 8601 UTC timestamp.
- `raw_text` (string, required): Raw extracted text payload.
- `normalized_text` (string, required): Normalized text used for feature extraction.
- `regression_target` (number, optional): Target value when available.
- `feature_tokens` (array[string], required): Canonicalized token list.
- `content_hash_sha256` (string, required): SHA-256 hash of normalized payload.
- `provenance` (object, required):
  - `policy_id` (string, required)
  - `license` (string, required)
  - `crawl_rule` (string, required)
  - `source_retrieval_method` (string, required)

## Validation Rules

- Source URL domain MUST be allowlisted by active source policy.
- `normalized_text` MUST be non-empty after sanitation.
- `content_hash_sha256` MUST be valid hex digest format.
- Duplicate `content_hash_sha256` values in one dataset version MUST be de-duplicated or rejected.
- Records missing any required field MUST be rejected.

## Behavioral Guarantees

- Dataset publication emits a deterministic manifest containing run ID, policy ID, and record count.
- Failed records are written to a reject report with explicit validation reasons.
- Corpus versions are immutable once published.

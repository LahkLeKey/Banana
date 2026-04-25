# Feature Specification: Wikidata + Faker Training Data

**Feature Branch**: `017-wikidata-faker-training-data`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`015-native-ml-runtime`](../015-native-ml-runtime/spec.md)

## Summary

Define a reproducible training-data pipeline that combines public structured knowledge from Wikidata with deterministic synthetic examples generated through Faker-style libraries. The pipeline must improve banana-vs-not-banana training coverage while preserving provenance, reproducibility, and CI-ready validation.

## User Scenarios & Testing

### User Story 1 - Curate external and synthetic corpus inputs (Priority: P1)

As a data curator, I can refresh training data from Wikidata and Faker-backed synthetic generation so model training has broader and more current examples.

**Why this priority**: Data quality and coverage directly determine model quality and release confidence.

**Independent Test**: Execute a corpus refresh flow and verify the resulting dataset contains both Wikidata-sourced and Faker-generated records with explicit provenance tags.

**Acceptance Scenarios**:

1. **Given** a valid Wikidata query profile, **When** corpus refresh runs, **Then** labeled examples from Wikidata are included with source metadata.
2. **Given** a configured Faker generation profile, **When** synthetic generation runs, **Then** synthetic examples are appended with deterministic metadata and labels.

---

### User Story 2 - Train with balanced and deterministic datasets (Priority: P1)

As an ML maintainer, I can train banana-vs-not-banana models using balanced, deterministic datasets so repeated CI runs are stable.

**Why this priority**: Deterministic training data prevents release drift and reduces debugging noise.

**Independent Test**: Run dataset generation twice with identical snapshot and seed settings and verify identical output checksums and class-distribution metrics.

**Acceptance Scenarios**:

1. **Given** identical input snapshots and seeds, **When** corpus generation executes twice, **Then** output artifacts are byte-identical.
2. **Given** generated corpus output, **When** distribution checks run, **Then** banana/not-banana class ratio remains within configured bounds.

---

### User Story 3 - Enforce CI trust and provenance controls (Priority: P2)

As a release owner, I can rely on CI gates that validate data provenance, reproducibility, and contract alignment before model artifacts are accepted.

**Why this priority**: Training-data regressions should be caught before they become model or runtime regressions.

**Independent Test**: Run CI data validation checks and verify failures occur for missing provenance, missing source snapshots, or reproducibility mismatches.

**Acceptance Scenarios**:

1. **Given** corpus records without source metadata, **When** validation executes, **Then** CI fails with explicit remediation guidance.
2. **Given** data refresh updates, **When** contract drift checks run, **Then** native/API token-alignment gates report pass or fail deterministically.

---

### User Story 4 - Preserve runtime contract compatibility (Priority: P2)

As a platform maintainer, I can evolve training data without silently breaking downstream native, API, and workflow contracts.

**Why this priority**: Data updates must not destabilize established model-training and inference pipelines.

**Independent Test**: Validate updated corpus against existing training workflow and token-drift contracts.

**Acceptance Scenarios**:

1. **Given** new Wikidata/Faker corpus content, **When** training workflow executes, **Then** contract artifacts remain compatible with downstream consumers.
2. **Given** vocabulary shifts beyond threshold, **When** drift checks run, **Then** validation fails with explicit delta reporting.

## Edge Cases

- Wikidata endpoint throttling or temporary outages during scheduled refresh.
- Duplicate or conflicting labels across Wikidata and synthetic records.
- Faker profile/version changes that alter generated text patterns.
- Over-representation of one class after combined-source ingestion.
- Non-ASCII aliases or multilingual labels that require normalization.
- Unsafe or low-quality synthetic phrases that must be filtered before training.

## Requirements

### Functional Requirements

- **FR-001**: System MUST support corpus ingestion from Wikidata-derived records for banana and not-banana classes.
- **FR-002**: System MUST support synthetic corpus generation via Faker-style libraries with deterministic seed control.
- **FR-003**: Each training record MUST include explicit provenance metadata (`source_type`, source identifier, and generation metadata).
- **FR-004**: Corpus generation MUST produce deterministic outputs when snapshot inputs and seeds are unchanged.
- **FR-005**: System MUST enforce configurable class-balance bounds for banana vs not-banana examples.
- **FR-006**: System MUST normalize, de-duplicate, and validate records before writing canonical corpus artifacts.
- **FR-007**: System MUST emit validation reports that include provenance completeness, class distribution, and reproducibility signals.
- **FR-008**: CI validation MUST fail when provenance is incomplete, reproducibility is violated, or drift checks exceed defined thresholds.
- **FR-009**: Corpus outputs MUST remain aligned with existing not-banana training contracts and downstream drift checks.
- **FR-010**: System MUST provide a fallback mode that uses last known-good source snapshots when live Wikidata refresh is unavailable.
- **FR-011**: System MUST document allowed Faker generation profiles (locale/category patterns) and disallowed unsafe synthetic patterns.

### Hard contracts to preserve

- `data/not-banana/corpus.json` as the canonical training corpus artifact location.
- `scripts/train-not-banana-model.py` and `.github/workflows/train-not-banana-model.yml` as the primary training/CI path.
- Native/API drift alignment expectations tied to `src/native/core/domain/banana_not_banana.c` and `src/typescript/api/src/domains/not-banana/routes.ts`.
- Existing deterministic model-training expectations for CI reproducibility.

### Key Entities

- **SourceSnapshot**: Versioned capture of Wikidata query outputs and related metadata.
- **SyntheticGenerationProfile**: Seeded Faker configuration that defines generation rules and category coverage.
- **TrainingRecord**: Canonical labeled example with normalized content and provenance fields.
- **CorpusManifest**: Validation artifact summarizing counts, class balance, reproducibility hash, and data-quality checks.
- **DriftReport**: Comparison artifact showing vocabulary or signal shifts against baseline thresholds.

## Success Criteria

- **SC-001**: 100% of training records in generated corpus include valid provenance metadata.
- **SC-002**: Two corpus generations with identical snapshots and seeds produce identical checksums.
- **SC-003**: Banana/not-banana class balance remains within configured bounds for all CI-validated runs.
- **SC-004**: CI reliably blocks corpus updates that violate provenance, reproducibility, or drift thresholds.
- **SC-005**: Training workflow completes successfully using combined Wikidata + Faker corpus without downstream contract breakage.

## Assumptions

- Wikidata access is permitted for the project and can be cached/snapshotted for deterministic CI use.
- Faker-style synthetic generation is used to augment, not replace, curated and external corpus data.
- Existing training and drift pipelines remain the canonical enforcement path for corpus quality.
- Corpus governance (review/approval) continues through existing repository workflows.

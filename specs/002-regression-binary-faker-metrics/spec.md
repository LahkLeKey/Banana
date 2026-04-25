# Feature Specification: Regression, Binary Sharing, Faker, and Metric Gates

**Feature Branch**: `002-regression-binary-faker-metrics`  
**Created**: 2026-04-24  
**Status**: Draft  
**Input**: User description: "we need to use a web scraper for the regression model and then share that data with the binary classification model using faker to act as a second data set for jaccard simularity and confusion matrix"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Scraper Regression Corpus (Priority: P1)

A model engineer can refresh a governed web-scraped corpus that is valid for regression training and traceable by provenance.

**Why this priority**: Without a deterministic and governed corpus, downstream sharing and evaluation steps have no reliable base.

**Independent Test**: Run one ingestion cycle and confirm regression-ready records are produced with required provenance fields and validation.

**Acceptance Scenarios**:

1. **Given** approved source policies exist, **When** scraping runs, **Then** a normalized regression corpus is produced with source metadata and stable record IDs.
2. **Given** invalid or blocked sources are encountered, **When** ingestion validates records, **Then** non-compliant records are rejected with actionable reasons.

---

### User Story 2 - Shared Data and Faker Secondary Dataset (Priority: P2)

A model engineer can share regression-derived data with the binary classifier and generate a deterministic Faker dataset as a secondary comparison set.

**Why this priority**: Cross-model reuse plus synthetic augmentation is required before quality comparison metrics can be trusted.

**Independent Test**: Generate a shared dataset projection and a Faker dataset run, then confirm both conform to one shared schema contract.

**Acceptance Scenarios**:

1. **Given** a refreshed regression corpus, **When** projection runs, **Then** binary-ready records are emitted with contract version metadata.
2. **Given** a fixed Faker seed and config, **When** synthetic generation runs twice, **Then** both runs produce equivalent outputs.

---

### User Story 3 - Jaccard and Confusion Matrix Gates (Priority: P3)

An integration operator can enforce Jaccard similarity and confusion matrix thresholds across scraped and Faker-backed evaluation runs.

**Why this priority**: Management and engineering need explicit quality gates, not ad hoc metric checks.

**Independent Test**: Execute one evaluation run against primary and secondary datasets and confirm thresholds are enforced with pass/fail outputs.

**Acceptance Scenarios**:

1. **Given** evaluation inputs are available, **When** metric computation runs, **Then** Jaccard similarity and confusion matrix outputs are emitted in a standard report schema.
2. **Given** any threshold is violated, **When** gating evaluates metrics, **Then** workflow status fails with threshold deltas and remediation context.
3. **Given** reports are published after evaluation, **When** stakeholders inspect outputs, **Then** human-facing summaries remain concise while AI-audit outputs retain full technical details.

---

### Edge Cases

- What happens when scraper output is empty for one or more approved sources?
- How does the system handle shared-schema version drift between regression and binary pipelines?
- What happens when Faker class balance cannot be satisfied under configured constraints?
- How are Jaccard and confusion outputs handled when label coverage is sparse or one class is missing?

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The system MUST define a governed scraper corpus contract for regression training records.
- **FR-002**: Scraper outputs MUST include provenance metadata including source, fetch timestamp, and content hash.
- **FR-003**: Scraper ingestion MUST enforce source allowlist and crawl policy checks.
- **FR-004**: The system MUST expose a shared dataset projection contract consumable by both regression and binary workflows.
- **FR-005**: Shared dataset records MUST include contract version metadata and source lineage references.
- **FR-006**: The system MUST support deterministic Faker-based secondary dataset generation using explicit seed/config.
- **FR-007**: Faker output MUST conform to the same shared schema consumed by binary evaluation.
- **FR-008**: The evaluation harness MUST compute Jaccard similarity between primary and secondary dataset feature/label sets.
- **FR-009**: The evaluation harness MUST compute confusion matrix outputs for binary classification results.
- **FR-010**: Quality gate thresholds for Jaccard and confusion-matrix-derived metrics MUST be explicit and versioned.
- **FR-011**: Workflow execution MUST fail when any configured quality threshold is not met.
- **FR-012**: Generated metric reports MUST be exportable for management-facing review.
- **FR-013**: The implementation MUST preserve native ML public contracts unless a breaking change is explicitly approved.
- **FR-014**: Published metric reporting artifacts MUST preserve an audience split with a concise human-readable path and a verbose AI-audit path.

### Key Entities *(include if feature involves data)*

- **Scraper Corpus Record**: A normalized web-derived record for regression training with provenance and feature fields.
- **Shared Projection Record**: A contract-versioned record mapped from regression corpus into binary-consumable shape.
- **Faker Secondary Record**: A deterministic synthetic record conforming to shared projection schema.
- **Evaluation Metric Report**: A run-scoped report containing Jaccard, confusion matrix values, and gate decisions.
- **Gate Policy**: A versioned threshold configuration for evaluation pass/fail enforcement.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: 100% of accepted scraped records include required provenance fields and valid content hash.
- **SC-002**: Shared regression-to-binary projection succeeds without schema violations for at least 99% of candidate records in a baseline run.
- **SC-003**: Faker generation with identical seed/config produces deterministic equivalent outputs across repeated runs.
- **SC-004**: Every evaluation run emits Jaccard and confusion matrix metrics in one standardized report artifact.
- **SC-005**: Quality gate enforcement blocks promotion whenever threshold policy is violated.
- **SC-006**: Management and operators can locate both human-readable summary output and AI-audit detail output from one shared runbook entry point in under 2 minutes.

## Assumptions

- Existing repository governance labels and approval requirements remain unchanged.
- Scraper ingestion policy and legal constraints are provided by existing repo governance inputs.
- The binary classification workflow can consume shared records when schema version checks pass.
- Faker is used only for synthetic training/evaluation augmentation and not as a replacement for governed primary corpus data.
- Documentation publishing follows a dual-track convention so casual stakeholder summaries do not remove technical audit detail needed for automation and triage.
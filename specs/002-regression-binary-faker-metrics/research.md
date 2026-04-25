# Research: Regression Corpus Sharing and Evaluation Gates

## Decision 1: Treat web scraping as a governed primary corpus source for regression

- Decision: Use an allowlisted scraper ingestion contract with strict provenance and normalization requirements.
- Rationale: Regression training requires traceable and consistent source quality.
- Alternatives considered:
  - Manual one-off data pulls. Rejected due to poor reproducibility.
  - Unrestricted crawling. Rejected due to governance and quality risk.

## Decision 2: Use one shared schema contract for regression-to-binary projection

- Decision: Define one versioned shared dataset projection schema consumed by binary classification paths.
- Rationale: Prevents duplicated parsers and drift between model pipelines.
- Alternatives considered:
  - Separate per-model export formats. Rejected due to contract drift risk.

## Decision 3: Require deterministic Faker generation for secondary dataset

- Decision: Synthetic dataset generation is seed/config driven and produces reproducible manifests.
- Rationale: Secondary dataset must be replayable for comparisons and auditability.
- Alternatives considered:
  - Non-deterministic synthetic data. Rejected due to unreproducible evaluation deltas.

## Decision 4: Gate promotion on Jaccard and confusion matrix thresholds

- Decision: Enforce explicit threshold policy with fail-fast workflow behavior.
- Rationale: Codifies quality criteria for management and engineering confidence.
- Alternatives considered:
  - Informational-only metrics. Rejected because failures could silently pass.

## Decision 5: Keep native ML contracts stable by default

- Decision: Any public contract change in native ML headers requires explicit breaking-change approval.
- Rationale: Protects upstream/downstream consumers from accidental interface churn.
- Alternatives considered:
  - Ad hoc contract changes during implementation. Rejected as too risky.

## Decision 6: Publish evaluation reporting in dual tracks for human and AI audiences

- Decision: Keep one shared reporting payload while publishing two audience-specific views: concise human-readable summaries and verbose AI-audit detail.
- Rationale: Management needs quick situational understanding while operators and automation require complete technical traceability.
- Alternatives considered:
  - Single mixed audience report. Rejected because it either overwhelms human readers or under-specifies audit detail.

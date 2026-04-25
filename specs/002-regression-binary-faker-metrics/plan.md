# Implementation Plan: Regression, Binary Sharing, Faker, and Metric Gates

**Branch**: `002-regression-binary-faker-metrics` | **Date**: 2026-04-24 | **Spec**: `specs/002-regression-binary-faker-metrics/spec.md`
**Input**: Feature specification from `specs/002-regression-binary-faker-metrics/spec.md`

## Summary

Introduce a governed scraper corpus for regression training, project shared records into the binary classification pipeline, add deterministic Faker secondary dataset generation, enforce Jaccard/confusion-matrix quality gates in evaluation workflows, and preserve dual-track reporting outputs for human-readable summaries and AI-audit detail.

## Technical Context

**Language/Version**: C11 (native ML interop), Python 3.11 (data tooling), Bash (orchestration), JSON/Markdown (contracts and reports)  
**Primary Dependencies**: Existing workflow orchestration scripts, native ML domain contracts, deterministic Faker generation tooling, metrics computation utilities  
**Storage**: Repository-managed dataset manifests and artifacts under `data/` and `artifacts/`  
**Testing**: Native tests (`tests/native`), Python/unit integration checks, `python scripts/validate-ai-contracts.py`, orchestration dry-runs  
**Target Platform**: GitHub-hosted Ubuntu runners and local Windows + Git Bash workflows  
**Project Type**: Multi-domain data + model automation enhancement  
**Performance Goals**: Deterministic dataset generation and bounded evaluation runtime within existing CI windows  
**Constraints**: Preserve native ML public contract stability unless explicitly approved; keep governance labels and approval paths intact; ensure reproducible synthetic generation; keep documentation/reporting outputs split between concise human-readable and verbose AI-audit paths  
**Scale/Scope**: Scraper ingestion policy, shared data schema, synthetic data generation, and quality gate enforcement across regression and binary model paths

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- **Domain Core First**: Pass with explicit native ML scope in `src/native/core/domain/ml/*` and `src/native/wrapper/domain/ml/*`.
- **Layered Interop Contract**: Pass if shared schema and evaluation contracts remain explicit and versioned.
- **Spec First Delivery**: Pass. This feature is captured in a dedicated spec set before implementation changes.
- **Verifiable Quality Gates**: Pass. Jaccard and confusion matrix thresholds become explicit gate contracts.
- **Documentation and Wiki Parity**: Pass with required runbook updates in docs/wiki during implementation.

## Project Structure

### Documentation (this feature)

```text
specs/002-regression-binary-faker-metrics/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── scraper-regression-corpus.md
│   ├── regression-binary-shared-dataset.md
│   ├── faker-secondary-dataset.md
│   └── jaccard-confusion-gates.md
└── tasks.md
```

### Source Code (repository root)

```text
scripts/
├── scrape-regression-corpus.py
├── project-shared-regression-binary-dataset.py
├── generate-faker-secondary-dataset.py
└── evaluate-regression-binary-quality-gates.py

src/native/core/domain/ml/
└── [shared and model-specific gate/metric updates]

src/native/wrapper/domain/ml/
└── [interop surface updates if required]

tests/native/
└── [metric and gate contract tests]

docs/
└── [runbook and contract maintenance docs]
```

**Structure Decision**: Keep contract definitions in Spec-Kit docs and execute implementation across scripts + native ML layers with explicit shared-schema boundaries.

## Complexity Tracking

No constitution violations expected.

# Tasks: Regression, Binary Sharing, Faker, and Metric Gates

**Input**: Design documents from `.specify/specs/002-regression-binary-faker-metrics/`
**Prerequisites**: `plan.md`, `spec.md`, `research.md`, `data-model.md`, `contracts/`, `quickstart.md`

**Tests**: Add contract, integration, and deterministic replay checks as part of implementation.

## Phase 1: Setup

- [ ] T001 Create dataset artifact roots under `data/regression-corpus/` and `artifacts/model-data/`
- [ ] T002 Create script scaffolds:
  - `scripts/scrape-regression-corpus.py`
  - `scripts/project-shared-regression-binary-dataset.py`
  - `scripts/generate-faker-secondary-dataset.py`
  - `scripts/evaluate-regression-binary-quality-gates.py`

## Phase 2: Foundational Contracts

- [ ] T003 Implement scraper output schema validation against `contracts/scraper-regression-corpus.md`
- [ ] T004 Implement shared projection schema validation against `contracts/regression-binary-shared-dataset.md`
- [ ] T005 Implement Faker generation config/output validation against `contracts/faker-secondary-dataset.md`
- [ ] T006 Implement evaluation report and threshold policy validation against `contracts/jaccard-confusion-gates.md`

## Phase 3: User Story 1 (P1)

- [ ] T007 [US1] Implement governed source policy loading and validation
- [ ] T008 [US1] Implement normalized scraper ingestion and manifest emission
- [ ] T009 [US1] Add reject-report path for invalid source/payload records

## Phase 4: User Story 2 (P2)

- [ ] T010 [US2] Implement regression-to-binary projection pipeline
- [ ] T011 [US2] Implement deterministic Faker secondary dataset generator
- [ ] T012 [US2] Add compatibility checks for shared schema versioning

## Phase 5: User Story 3 (P3)

- [ ] T013 [US3] Implement Jaccard similarity computation on primary/secondary datasets
- [ ] T014 [US3] Implement confusion matrix and derived metric computation
- [ ] T015 [US3] Implement threshold gate enforcement and fail-fast behavior

## Phase 6: Polish

- [ ] T016 Add docs and runbook updates for contract maintenance, including separate human-readable and AI-audit reporting paths
- [ ] T017 Run validation and deterministic replay checks in CI
- [ ] T018 Verify native ML public contract stability in touched headers

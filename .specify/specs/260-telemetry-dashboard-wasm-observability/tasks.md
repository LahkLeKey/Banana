# Tasks: Telemetry Dashboard + Production WASM Worker Assurance (260)

**Input**: Spec at `.specify/specs/260-telemetry-dashboard-wasm-observability/spec.md`

## Phase 1: Discovery

- [ ] T001 Inventory the current Review Spikes tab entry points, routes, and component dependencies that will be replaced by Telemetry dashboard surfaces. [FR-001]
- [ ] T002 Inventory existing observability signals available to the website (runtime, API, frontend error, and worker lifecycle) and document dashboard data-source fit/gaps. [FR-002]
- [ ] T003 Define baseline production WASM worker health dimensions and measurement windows (init, execution, timeout, cancel, fallback, variant mix). [FR-003] [FR-004] [FR-005]

## Phase 2: Design

- [ ] T004 Define Telemetry dashboard information architecture that replaces Review Spikes and supports drill-down navigation between observability panels. [FR-001] [FR-002]
- [ ] T005 Define WASM worker health panel schema, including required metrics, segmentation dimensions, and event sample fields. [FR-003] [FR-004] [FR-005]
- [ ] T006 Define readiness-threshold policy and pass/fail indicator model for production rollout decisions. [FR-006] [FR-008]
- [ ] T007 Define degraded/stale-data UX states and fallback reason presentation contract for operator triage. [FR-007]

## Phase 3: Validation Planning

- [ ] T008 Define production verification workflow to confirm WASM workers are performing as expected (evidence capture, window selection, and acceptance checklist). [FR-008]
- [ ] T009 Define browser-based UAT scenarios that validate dashboard replacement behavior and drill-down usability in the website itself. [FR-001] [FR-002]
- [ ] T010 Define telemetry quality checks for freshness, missing-data handling, and scalar-versus-SIMD interpretation correctness. [FR-004] [FR-005] [FR-007]

## Phase 4: Spec Validation

- [ ] T011 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/260-telemetry-dashboard-wasm-observability` and confirm `OK`. [FR-001]
- [ ] T012 Run `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/260-telemetry-dashboard-wasm-observability/spec.md` and confirm pass. [FR-002]
- [ ] T013 Run `bash .specify/scripts/bash/validate-task-traceability.sh --spec .specify/specs/260-telemetry-dashboard-wasm-observability/spec.md --tasks .specify/specs/260-telemetry-dashboard-wasm-observability/tasks.md` and confirm pass. [FR-003]

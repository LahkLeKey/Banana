# Tasks: Telemetry Dashboard + Production WASM Worker Assurance (260)

**Input**: Spec at `.specify/specs/260-telemetry-dashboard-wasm-observability/spec.md`

## Phase 1: Implementation

- [x] T001 Replace Review Spikes navigation and route with Telemetry dashboard entry in the React workspace shell/router. [FR-001]
- [x] T002 Implement Telemetry dashboard page layout with drill-down sections for runtime, API, frontend, WASM worker, and native-level health views. [FR-002] [FR-009]
- [x] T003 Add WASM worker telemetry aggregation layer for init/call latency, timeout/cancel/fallback rates, error-code distribution, and SIMD/scalar mix. [FR-003] [FR-004] [FR-005]
- [x] T004 Add native-level aggregation and correlation model to surface wrapper/core/DAL status and latency distributions from website-accessible telemetry inputs. [FR-009] [FR-010]
- [x] T005 Implement readiness threshold indicators and degraded/stale-data UX states with explicit pass/fail and fallback reason visibility. [FR-006] [FR-007] [FR-008]

## Phase 2: QA (Automated)

- [x] T006 Add/extend React unit tests for route replacement, dashboard rendering, drill-down transitions, and stale-data/degraded state handling. [FR-001] [FR-002] [FR-007]
- [x] T007 Add/extend telemetry-layer tests validating worker metric rollups, native metric rollups, and cross-layer correlation behavior. [FR-003] [FR-004] [FR-009] [FR-010]
- [x] T008 Add/extend tests validating readiness-threshold evaluation logic and pass/fail surface behavior for production windows. [FR-006] [FR-008]

## Phase 3: UAT (Browser + Production-Facing)

- [x] T009 Add/extend Playwright UAT scenario that verifies Review Spikes is replaced by Telemetry dashboard and that WASM drill-down is reachable and populated. [FR-001] [FR-002] [FR-003]
- [x] T010 Add/extend Playwright UAT scenario that verifies native observability drill-down exposes wrapper/core/DAL status/latency fields (or explicit unavailable state) without UI failure. [FR-009] [FR-010]
- [ ] T011 Capture production-verification evidence that WASM workers are performing as expected and thresholds are evaluable from the website dashboard. [FR-008]

## Phase 4: Spec Validation

- [x] T012 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/260-telemetry-dashboard-wasm-observability` and confirm `OK`. [FR-001]
- [x] T013 Run `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/260-telemetry-dashboard-wasm-observability/spec.md` and confirm pass. [FR-002]
- [x] T014 Run `bash .specify/scripts/bash/validate-task-traceability.sh --spec .specify/specs/260-telemetry-dashboard-wasm-observability/spec.md --tasks .specify/specs/260-telemetry-dashboard-wasm-observability/tasks.md` and confirm pass. [FR-003]

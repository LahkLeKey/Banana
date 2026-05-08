# Tasks: 261-wasm-viewport-react-overlays

**Input**: Spec at `.specify/specs/261-wasm-viewport-react-overlays/spec.md`

## Phase 1: Discovery and Contracts

- [x] T001 Define canonical viewport lifecycle state machine (`booting`, `ready`, `running`, `degraded`, `recovering`, `stopped`) in `.specify/specs/261-wasm-viewport-react-overlays/spec.md`. [FR-003]
- [x] T002 Define overlay layer priority and focus-lock contract in `.specify/specs/261-wasm-viewport-react-overlays/spec.md`. [FR-004, FR-005]
- [x] T003 Define runtime telemetry event schema for viewport and overlay transitions in `.specify/specs/261-wasm-viewport-react-overlays/spec.md`. [FR-006]

## Phase 2: React Runtime Shell

- [x] T004 Implement viewport-first root layout so WASM surface is the primary background in `src/typescript/react/src/**` app shell files. [FR-001, FR-002]
- [x] T005 Implement overlay layer manager (HUD, modal, critical alert) in `src/typescript/react/src/**` overlay components/hooks. [FR-004]
- [x] T006 Implement deterministic input-routing transitions between viewport mode and overlay mode in `src/typescript/react/src/**` input/focus hooks. [FR-005]

## Phase 3: Failure and Recovery Path

- [x] T007 Implement degraded-mode entry on WASM timeout/failure with non-crashing UI shell behavior in `src/typescript/react/src/**`. [FR-007, FR-009]
- [x] T008 Implement retry and API fallback actions in degraded overlay state in `src/typescript/react/src/**`. [FR-008]
- [x] T009 Preserve game-state persistence and telemetry endpoint compatibility for migrated runtime flow in `src/typescript/react/src/**` API integration modules. [FR-010]

## Phase 4: Validation

- [x] T010 Add lifecycle transition contract tests for healthy and degraded transitions in React test suites under `src/typescript/react/**` and/or `tests/**`. [FR-011]
- [x] T011 Add input-routing transition contract tests for overlay open/close handoff in React test suites under `src/typescript/react/**` and/or `tests/**`. [FR-011]
- [x] T012 Capture healthy-path and degraded-recovery CI evidence (run IDs + artifacts) in `.specify/specs/261-wasm-viewport-react-overlays/tasks.md`. [FR-012]

## Phase 5: Spec Validation

- [x] T013 Run `python scripts/validate-spec-tasks-parity.py .specify/specs/261-wasm-viewport-react-overlays` and confirm `OK`. [FR-012]
- [x] T014 Run `bash .specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/261-wasm-viewport-react-overlays/spec.md` and confirm pass. [FR-011]
- [x] T015 Run `bash .specify/scripts/bash/validate-task-traceability.sh --spec .specify/specs/261-wasm-viewport-react-overlays/spec.md --tasks .specify/specs/261-wasm-viewport-react-overlays/tasks.md` and confirm pass. [FR-012]

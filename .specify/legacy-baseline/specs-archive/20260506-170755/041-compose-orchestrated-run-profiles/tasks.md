# Tasks: Compose-Orchestrated Run Profiles

**Branch**: 041-compose-orchestrated-run-profiles | **Created**: 2026-04-26

## Phase 1 -- Setup

- [x] T001 Document compose-first profile objective and execution tracker in .specify/specs/041-compose-orchestrated-run-profiles/README.md.
- [x] T002 Confirm profile contract scope and required runtime variables in .specify/specs/041-compose-orchestrated-run-profiles/plan.md.

## Phase 2 -- Foundational

- [x] T003 Create canonical profile matrix source in .specify/specs/041-compose-orchestrated-run-profiles/contracts/compose-run-profile-contract.md.
- [x] T004 [P] Add a shared compose run-profile entrypoint script in scripts/compose-run-profile.sh.
- [x] T005 [P] Add deterministic profile readiness helper script in scripts/compose-profile-ready.sh.
- [x] T006 Align existing compose wrapper scripts to canonical profile entrypoint in scripts/compose-runtime.sh.
- [x] T007 [P] Align app/runtime wrappers to canonical profile entrypoint in scripts/compose-apps.sh.
- [x] T008 [P] Align test profile wrapper to canonical profile entrypoint in scripts/compose-tests.sh.
- [x] T009 Align profile teardown behavior to canonical orchestration flow in scripts/compose-apps-down.sh.

## Phase 3 -- US1 Deterministic profile bring-up (Priority: P1)

**Goal**: Developers launch each supported local profile through one compose-driven orchestration path with predictable readiness.

**Independent Test**: Launch primary profile through canonical script, verify readiness checks pass, stop and relaunch with consistent outcomes.

- [x] T010 [US1] Implement profile routing and compose profile mapping in scripts/compose-run-profile.sh.
- [x] T011 [US1] Implement per-profile readiness checks and timeout handling in scripts/compose-profile-ready.sh.
- [x] T012 [US1] Update Windows->WSL2 desktop launcher to use canonical profile entrypoint in scripts/launch-container-channels-with-wsl2-electron.sh.
- [x] T013 [US1] Update Ubuntu desktop launcher to use canonical profile entrypoint in scripts/compose-electron-desktop-wsl2.sh.
- [x] T014 [US1] Ensure mobile launcher profile orchestration uses canonical compose entrypoint in scripts/launch-container-channels-with-wsl2-mobile.sh.

## Phase 4 -- US2 Reproducible onboarding and troubleshooting (Priority: P2)

**Goal**: New contributors can start supported profiles with one documented flow and actionable remediation when prerequisites fail.

**Independent Test**: Follow onboarding/runtime docs on clean setup and complete profile startup without undocumented host-specific commands.

- [x] T015 [US2] Publish run-profile matrix and start/stop commands in README.md.
- [x] T016 [US2] Update WSL2 runtime contract troubleshooting and prerequisite failure guidance in .github/ubuntu-wsl2-runtime-contract.md.
- [x] T017 [US2] Update script-level prerequisite diagnostics and remediation messaging in scripts/compose-electron-desktop-wsl2.sh.
- [x] T018 [US2] Update mobile orchestration prerequisite diagnostics and remediation messaging in scripts/compose-mobile-emulators-wsl2.sh.
- [x] T019 [US2] Mirror runtime workflow changes into AI-consumable wiki source in .specify/wiki/human-reference/.

## Phase 5 -- US3 Predictable validation and repeatability checks (Priority: P3)

**Goal**: Maintainers can validate reproducibility through repeat-run profile checks with clear contract drift signals.

**Independent Test**: Run profile reproducibility validation for selected profiles across three consecutive runs and verify consistent pass/fail outcomes.

- [x] T020 [US3] Add repeat-run profile reproducibility validator in scripts/validate-compose-run-profiles.sh.
- [x] T021 [US3] Integrate profile readiness and reproducibility checks into compose CI/runtime validation flow in scripts/validate-compose-ci-lane-contract.sh.
- [x] T022 [US3] Add optional local orchestration validation lane command wrapper in scripts/compose-runtime.sh.
- [x] T023 [US3] Document reproducibility validation workflow in .specify/specs/041-compose-orchestrated-run-profiles/quickstart.md.

## Phase 6 -- Validation and close-out

- [x] T024 Run python scripts/validate-ai-contracts.py.
- [x] T025 Run bash scripts/validate-spec-tasks-parity.sh --all.
- [x] T026 Run profile bring-up/down and readiness validation for primary and secondary profiles using canonical entrypoints.
- [x] T027 Run repeat-run reproducibility validation for selected profiles and record outcomes in .specify/specs/041-compose-orchestrated-run-profiles/README.md.
- [x] T028 Mark spec 041 tasks complete and set README status for execution state in .specify/specs/041-compose-orchestrated-run-profiles/README.md.

## Dependencies & Execution Order

- Setup (Phase 1) must complete first.
- Foundational (Phase 2) blocks all user stories.
- US1 (Phase 3) should complete before US2 and US3 because onboarding and validation rely on canonical entrypoints.
- US2 (Phase 4) and US3 (Phase 5) can proceed in parallel once US1 is stable.
- Validation/close-out (Phase 6) runs after desired stories complete.

## Parallel Execution Examples

- Foundational parallel set: T004, T005, T007, T008
- US2 parallel set: T016, T017, T018
- US3 parallel set: T021, T023 (after T020)

## Implementation Strategy

### MVP First (US1 only)

1. Complete Phase 1 and Phase 2.
2. Complete US1 tasks (T010-T014).
3. Validate deterministic bring-up and restart behavior.

### Incremental Delivery

1. Deliver US1 canonical orchestration path.
2. Add US2 onboarding and troubleshooting parity updates.
3. Add US3 reproducibility validation automation and complete close-out.

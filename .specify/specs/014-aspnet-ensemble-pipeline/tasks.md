# Tasks: ASP.NET Ensemble Pipeline (Gated Cascade)

**Branch**: `014-aspnet-ensemble-pipeline` | **Created**: 2026-04-26

## Phase 1 -- Setup

- [ ] T001 Update `.specify/feature.json` -> `014-aspnet-ensemble-pipeline`.
- [ ] T002 Author `README.md` execution tracker (anchor table from SPIKE).
- [ ] T003 Author `analysis/in-scope-files.md` and
  `analysis/validation-commands.md`.

## Phase 2 -- Foundational

- [ ] T004 Add `src/c-sharp/asp.net/Pipeline/Results/EnsembleVerdictResult.cs`
  with documented field order: `label, score, did_escalate,
  calibration_magnitude, status`.
- [ ] T005 Extend `src/c-sharp/asp.net/Pipeline/PipelineContext.cs`
  with additive cascade flags (`VerdictLocked`, `DidEscalate`,
  `CalibrationMagnitude`).
- [ ] T006 Scaffold DI registration in `src/c-sharp/asp.net/Program.cs`
  in cascade order (gating -> escalation -> calibration). No behavior yet.

## Phase 3 -- US1 Cheap path

- [ ] T007 Implement `Pipeline/Steps/EnsembleGatingStep.cs` calling
  `ClassifyBananaBinary`; set `VerdictLocked = true` when out-of-band.
- [ ] T008 Unit test: clearly-banana payload -> cheap path, transformer
  invocation count = 0.
- [ ] T009 Unit test: clearly-not-banana payload -> cheap path, transformer
  invocation count = 0.
- [ ] T010 Unit test: in-band payload sets `VerdictLocked = false` and
  does NOT set the label yet.

## Phase 4 -- US2 Escalation

- [ ] T011 Implement `Pipeline/Steps/EnsembleEscalationStep.cs` calling
  `ClassifyBananaTransformer` when `VerdictLocked == false`; final verdict
  overrides binary.
- [ ] T012 Unit test: yellow-plastic-toy decoy escalates to transformer,
  final label = `not_banana`, `did_escalate=true`.
- [ ] T013 Unit test: ambiguous yellow-fruit escalates, final label =
  `banana`, `did_escalate=true`.
- [ ] T014 Unit test: empty-text escalates, final label = `banana`,
  `did_escalate=true`.

## Phase 5 -- US3 Calibration

- [ ] T015 Implement `Pipeline/Steps/EnsembleCalibrationStep.cs` calling
  `PredictRegressionScore`; attach `CalibrationMagnitude`; MUST NOT alter
  `label` or `score`.
- [ ] T016 Unit test: every payload from US1+US2 has `calibration_magnitude`
  in `[0,1]` after this step runs.
- [ ] T017 Unit test: calibration step does not mutate the locked label.

## Phase 6 -- US4 Degradation

- [ ] T018 Implement non-OK fallthrough in `EnsembleGatingStep` -> always
  escalate when binary status is non-OK.
- [ ] T019 Unit test: binary non-OK -> transformer wins, status=OK if
  transformer succeeds.
- [ ] T020 Unit test: both binary + transformer non-OK -> response carries
  regression score, `label="unknown"`, non-OK status code surfaced.

## Phase 7 -- Integration

- [ ] T021 Add ensemble route to `Controllers/BananaMlController.cs`
  (`POST /api/ml/ensemble`); reuse existing controller; no new file.
- [ ] T022 End-to-end pipeline test in
  `tests/integration/Banana.IntegrationTests/Pipeline/EnsembleEndToEndTests.cs`
  using fake `INativeBananaClient` returning anchor JSON.
- [ ] T023 Snapshot test of `EnsembleVerdictResult` JSON shape so 015
  contract drift fails this lane.

## Phase 8 -- Validation + close-out

- [ ] T024 Run native lane (`ctest --preset default`) -- expect 7/7,
  capture in `analysis/native-lane-evidence.md`.
- [ ] T025 Run `dotnet test` + `scripts/check-dotnet-coverage-threshold.sh`,
  capture in `analysis/dotnet-evidence.md`.
- [ ] T026 Close `tasks.md` (mark all `[x]`), update `README.md` with
  final status.

## Out of scope

- Native code changes.
- React / Electron wiring (slice 015 / later).

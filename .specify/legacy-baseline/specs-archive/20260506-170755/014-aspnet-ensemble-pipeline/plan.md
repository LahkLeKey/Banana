# Plan: ASP.NET Ensemble Pipeline (Gated Cascade)

**Branch**: `014-aspnet-ensemble-pipeline` | **Date**: 2026-04-26
**SPIKE source**: [../013-ml-brain-composition-spike/](../013-ml-brain-composition-spike/)

## Strategy

Three new ordered `IPipelineStep` implementations registered in DI in
cascade order, plus one new payload DTO and one new controller route.
No native changes; no existing step / route changes.

## In-scope files

See
[../013-ml-brain-composition-spike/analysis/followup-A-aspnet-ensemble.md](../013-ml-brain-composition-spike/analysis/followup-A-aspnet-ensemble.md).

## Phases

- **Phase 1 (Setup, T001-T003)**:
  - Update `.specify/feature.json` -> `014-aspnet-ensemble-pipeline`.
  - Author `README.md` execution tracker.
  - Author `analysis/in-scope-files.md`, `analysis/validation-commands.md`.
- **Phase 2 (Foundational, T004-T006)**:
  - Add `EnsembleVerdictResult.cs` DTO with documented field order.
  - Extend `PipelineContext.cs` with cascade flags (additive properties only).
  - Wire DI registration scaffolding in `Program.cs` (no behavior yet).
- **Phase 3 US1 -- Cheap path (T007-T010)**:
  - Implement `EnsembleGatingStep` (calls `ClassifyBananaBinary`,
    sets `verdict_locked` when out-of-band).
  - Unit tests: clear-banana + clear-not-banana cheap path; transformer
    invocation count = 0.
- **Phase 4 US2 -- Escalation (T011-T014)**:
  - Implement `EnsembleEscalationStep` (calls `ClassifyBananaTransformer`
    when escalate flag is set; final verdict overrides).
  - Unit tests: decoy + ambiguous-yellow + empty cases route to
    transformer and final label matches transformer.
- **Phase 5 US3 -- Calibration (T015-T017)**:
  - Implement `EnsembleCalibrationStep` (calls `PredictRegressionScore`,
    attaches magnitude; never alters label).
  - Unit tests: calibration always present and in `[0,1]`.
- **Phase 6 US4 -- Degradation (T018-T020)**:
  - Cascade fallthrough on non-OK status from binary / transformer.
  - Unit tests: degraded-status path emits `label="unknown"` + non-OK code.
- **Phase 7 -- Integration (T021-T023)**:
  - End-to-end pipeline test with fake `INativeBananaClient`.
  - Add controller route on `BananaMlController` (`POST /api/ml/ensemble`).
  - Integration test on the live ASP.NET host.
- **Phase 8 -- Validation + close-out (T024-T026)**:
  - Run native lane (7/7), `dotnet test`, coverage threshold.
  - Capture evidence under `analysis/`.
  - Close `tasks.md`.

## Risks

- **Test double drift**: `INativeBananaClient` already exposes all three
  methods, so no new interop seam is required. New test fakes must
  implement only the three methods used here; reuse existing fake from
  `tests/unit/**` if present.
- **Coverage threshold**: new pipeline code adds branches; ensure unit
  tests cover both in-band and out-of-band paths to keep
  `check-dotnet-coverage-threshold.sh` green.

## Out of scope

- Same as spec.md.

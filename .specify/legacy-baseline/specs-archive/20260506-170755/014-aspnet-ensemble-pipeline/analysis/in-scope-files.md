# In-scope files -- 014 ASP.NET Ensemble Pipeline

## Source under change

- `src/c-sharp/asp.net/Pipeline/Results/EnsembleVerdictResult.cs` (NEW, response DTO)
- `src/c-sharp/asp.net/Pipeline/PipelineContext.cs` (additive ensemble working state)
- `src/c-sharp/asp.net/Pipeline/Steps/EnsembleGatingStep.cs` (NEW)
- `src/c-sharp/asp.net/Pipeline/Steps/EnsembleEscalationStep.cs` (NEW)
- `src/c-sharp/asp.net/Pipeline/Steps/EnsembleCalibrationStep.cs` (NEW)
- `src/c-sharp/asp.net/Controllers/BananaMlController.cs` (extend with `[HttpPost("ensemble")]`)
- `src/c-sharp/asp.net/Program.cs` (DI registration of three new steps)

## Tests under change

- `tests/unit/EnsembleGatingStepTests.cs` (NEW)
- `tests/unit/EnsembleEscalationStepTests.cs` (NEW)
- `tests/unit/EnsembleCalibrationStepTests.cs` (NEW)
- `tests/unit/EnsembleEndToEndTests.cs` (NEW -- fold integration scope here
  because there is no `tests/integration/` project)

## Out of scope

- `src/native/**` (no exports, no ABI bump; native lane stays 7/7 + ABI 2.2)
- React/Electron consumption (slice 015 / Electron later)
- Cascade-band configurability (baked at `[0.35, 0.65]`)

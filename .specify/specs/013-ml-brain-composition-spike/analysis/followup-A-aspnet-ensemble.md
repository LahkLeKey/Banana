# Follow-up A -- ASP.NET ensemble pipeline (gated cascade)

## Objective

Implement the gated-cascade composition strategy (R-01) as three new
ordered ASP.NET pipeline steps, returning a single composite verdict that
combines the Right Brain (binary) decision, the Full Brain (transformer)
escalation, and the Left Brain (regression) calibration magnitude.

## In-scope files

- `src/c-sharp/asp.net/Pipeline/Steps/EnsembleGatingStep.cs` (NEW)
- `src/c-sharp/asp.net/Pipeline/Steps/EnsembleEscalationStep.cs` (NEW)
- `src/c-sharp/asp.net/Pipeline/Steps/EnsembleCalibrationStep.cs` (NEW)
- `src/c-sharp/asp.net/Pipeline/Results/EnsembleVerdictResult.cs` (NEW)
- `src/c-sharp/asp.net/Controllers/BananaMlController.cs` (extend with
  ensemble route only; no rename of existing routes)
- `src/c-sharp/asp.net/Program.cs` (DI registration of the three new steps)
- `tests/unit/Banana.UnitTests/Pipeline/EnsembleGatingStepTests.cs` (NEW)
- `tests/unit/Banana.UnitTests/Pipeline/EnsembleEscalationStepTests.cs` (NEW)
- `tests/unit/Banana.UnitTests/Pipeline/EnsembleCalibrationStepTests.cs` (NEW)
- `tests/integration/Banana.IntegrationTests/Pipeline/EnsembleEndToEndTests.cs` (NEW)

## Out of scope

- Any change to `src/native/**` (additive-ABI discipline preserved).
- Any change to existing pipeline steps (only additive registration).
- Configurable cascade band (baked in at `[0.35, 0.65]` for this slice).
- React or Electron consumer wiring (slice 015 / Electron deferred).
- New PostgreSQL queries or `BANANA_PG_CONNECTION` dependence.

## Validation lane

```
# Native (must stay 7/7 -- proves no native drift)
cmake --preset default
cmake --build --preset default
ctest --preset default --output-on-failure

# Managed
dotnet test src/c-sharp/asp.net/asp.net.sln \
  --collect:"XPlat Code Coverage"

# Coverage threshold (existing helper)
bash scripts/check-dotnet-coverage-threshold.sh
```

## Contract risk

- **JSON shape** -- introduces new `EnsembleVerdictResult` payload. MUST
  document field order in the new file's XML doc comment (mirror the
  012 wrapper-header pattern). No existing JSON shapes change.
- **DI registration order** -- the three new steps MUST be registered in
  cascade order (gating -> escalation -> calibration) to preserve cost
  guarantees from R-01.
- **No ABI bump** -- `BANANA_WRAPPER_ABI_VERSION_*` stays 2.2.

## Anchor references

- [011-right-brain-binary-runnable/analysis/us3-calibration-evidence.md](../../011-right-brain-binary-runnable/analysis/us3-calibration-evidence.md)
- [012-full-brain-transformer-runnable/README.md](../../012-full-brain-transformer-runnable/README.md)
- [composition-strategy.md](./composition-strategy.md) (worked anchor walks)

## Owner agent

`api-pipeline-agent` (primary), `api-interop-agent` (DTO + interop fakes).

## Acceptance summary

- 3 new pipeline steps registered; cascade preserves R-01 cost guarantee.
- Unit tests cover all 6 anchor walks from
  [composition-strategy.md](./composition-strategy.md).
- Integration test exercises the full pipeline end-to-end with a fake
  `INativeBananaClient` returning anchor JSON.
- `dotnet test` green; native lane still 7/7; coverage threshold preserved.

## Spec branch (when promoted)

`014-aspnet-ensemble-pipeline`

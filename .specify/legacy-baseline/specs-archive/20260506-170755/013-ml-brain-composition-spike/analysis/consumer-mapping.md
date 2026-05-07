# Consumer pull-through mapping (013 SPIKE, R-02 expansion)

## Brain ABI to consumer mapping

| Brain ABI export                                      | First consumer (file)                                                                                                | Owner agent           | New native export needed? |
|-------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------|-----------------------|---------------------------|
| `banana_classify_banana_regression`                   | `src/c-sharp/asp.net/Pipeline/Steps/EnsembleCalibrationStep.cs` (NEW, in followup-A)                                 | api-pipeline-agent    | no                        |
| `banana_classify_banana_binary_with_threshold`        | `src/c-sharp/asp.net/Pipeline/Steps/EnsembleGatingStep.cs` (NEW, in followup-A)                                      | api-pipeline-agent    | no                        |
| `banana_classify_banana_transformer` (legacy)         | `src/c-sharp/asp.net/Pipeline/Steps/EnsembleEscalationStep.cs` (NEW, in followup-A)                                  | api-pipeline-agent    | no                        |
| `banana_classify_banana_transformer_ex`               | Same as above, opt-in via controller flag for diagnostics surfacing                                                  | api-interop-agent     | no                        |
| Composite verdict JSON                                | `src/typescript/shared/ui/src/components/BananaBadge.tsx` ensemble variant + `src/typescript/react/src/App.tsx` wiring | react-ui-agent        | no                        |
| Optional attention weights                            | `src/typescript/electron/preload.js` bridge (deferred -- only after 015 lands)                                       | electron-agent        | no                        |
| Quantized embedding                                   | `src/c-sharp/asp.net/Pipeline/Steps/EnsembleEscalationStep.cs` payload reduction                                     | native-wrapper-agent  | YES (slice 016 trigger)   |

## Pipeline shape (proposed in followup-A)

`PipelineRunner` already orchestrates `IPipelineStep` instances. The
ensemble cascade is implemented as three new steps registered in order:

1. `EnsembleGatingStep` -- calls `ClassifyBananaBinary`. If
   `banana_score` is outside `[0.35, 0.65]`, sets a "verdict locked" flag
   on `PipelineContext` and returns. Otherwise sets an "escalate" flag.
2. `EnsembleEscalationStep` -- when "escalate" flag is set, calls
   `ClassifyBananaTransformer`. The transformer verdict overrides the
   binary verdict.
3. `EnsembleCalibrationStep` -- always calls `PredictRegressionScore`
   and attaches the regression magnitude to the response payload. Runs
   last so it never influences the label.

This ordering is intentional: **gating first, escalation second, calibration
last** -- so calibration cost is paid exactly once, and escalation cost is
paid only for in-band payloads.

## Test seam (each follow-up)

- **014** (ASP.NET ensemble): `tests/unit/Banana.UnitTests/Pipeline/Ensemble*Tests.cs`
  using a fake `INativeBananaClient` that returns the 011/012 anchor JSON.
- **015** (React badge): `src/typescript/shared/ui/src/components/BananaBadge.test.tsx`
  + `src/typescript/react/src/lib/api.test.ts` snapshot of ensemble shape.
- **016** (quantized embedding): `tests/native/test_ml_transformer_quant.c`
  ctest target + an integration test in 014's pipeline once the trigger fires.

## Out of scope for ALL three follow-ups

- New controllers (extend `BananaMlController` only if a new route is
  unavoidable; otherwise reuse).
- New Bun packages (consume existing `@banana/ui` exports).
- New PostgreSQL queries (`BANANA_PG_CONNECTION` not required).

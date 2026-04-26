# Checklist: Domain Value Delivery — Implementation Readiness

**Feature**: `004-domain-value-delivery`
**Generated**: 2026-04-26
**Source**: Value-risk scored backlog (Ranks 1–5)

---

## Pre-Implementation Gate

- [ ] Branch `004-domain-value-delivery` created and `feature.json` points to this spec
- [ ] `spec.md` reviewed — no `[NEEDS CLARIFICATION]` markers outstanding
- [ ] Existing `ctest` native tests pass before any native changes (`cmake --build` + `ctest`)
- [ ] Existing .NET unit tests pass before any ASP.NET changes (`dotnet test`)
- [ ] React `tsc --noEmit` passes before any TypeScript changes

---

## Backlog Rank 1 — ASP.NET Response Deserialization

**Files**: `Controllers/RipenessController.cs`, `Controllers/BananaMlController.cs`, `Controllers/NotBananaController.cs`, `Controllers/BatchController.cs`, `Pipeline/RipenessResult.cs` (new)

- [ ] `sealed record RipenessResult(string Label, double Confidence, string Model)` created in `Pipeline/`
- [ ] `RipenessController.PredictRipeness` calls `JsonSerializer.Deserialize<RipenessResult>(json)` and returns `Ok(result)` — no `Ok(new { json })`
- [ ] Same pattern applied to `BananaMlController` binary and transformer endpoints
- [ ] Same pattern applied to `NotBananaController`
- [ ] No controller calls `JsonSerializer.Deserialize` directly — deserialization lives in `NativeBananaClient` or Pipeline
- [ ] Integration test: `GET /ripeness/predict` response asserts `label` (string) and `confidence` (number) at top level
- [ ] `FR-001`, `FR-002`, `SC-001` verifiable via `curl` without secondary parse

---

## Backlog Rank 2 — Shared UI Ripeness Type Fix

**Files**: `src/typescript/shared/ui/src/types.ts`, `src/typescript/shared/ui/src/components/RipenessLabel.tsx`

- [ ] `Ripeness` type updated to `'ripe' | 'unripe' | 'overripe'` (removed: `'green' | 'yellow' | 'spotted' | 'brown'`)
- [ ] `COLORS` map in `RipenessLabel.tsx` rekeyed: `ripe → bg-yellow-200`, `unripe → bg-green-200`, `overripe → bg-amber-700`
- [ ] `RipenessLabel` renders a neutral/unknown state for any string value outside the three valid labels (edge case guard)
- [ ] `tsc --noEmit` passes in `src/typescript/shared/ui/`
- [ ] `FR-004`, `SC-003` verified

---

## Backlog Rank 3 — React API Client + RipenessLabel Render

**Files**: `src/typescript/react/src/lib/api.ts`, `src/typescript/react/src/App.tsx`

- [ ] `predictRipeness(baseUrl: string, inputJson: string): Promise<{ label: Ripeness; confidence: number }>` exported from `api.ts`
- [ ] `App.tsx` has a `<textarea>` input and submit button for ripeness input
- [ ] `App.tsx` renders `<RipenessLabel value={result.label} />` after a successful prediction
- [ ] `App.tsx` shows an error/loading state while the request is in-flight or has failed
- [ ] `RipenessLabel` import in `App.tsx` is no longer a dead import
- [ ] `tsc --noEmit` passes in `src/typescript/react/`
- [ ] `FR-005`, `FR-006`, `SC-004` verified in compose channel

---

## Backlog Rank 4 — Native ML Ripeness Upgrade

**Files**: `src/native/core/banana_ripeness.c`

- [ ] `count_term_ci` keyword-counting loop removed from `banana_core_predict_ripeness`
- [ ] `banana_ml_build_feature_vector(input_json, &features)` called and status checked
- [ ] `banana_ml_regression_predict(&features, &score)` called and status checked
- [ ] Score threshold mapping applied: `score < 0.35 → "unripe"`, `score > 0.72 → "overripe"`, else `"ripe"`
- [ ] Output JSON shape: `{"label":"ripe","confidence":0.8123,"model":"regression"}`
- [ ] Non-OK status from `banana_ml_build_feature_vector` propagated without crash
- [ ] `ctest` passes all existing native tests
- [ ] New test in `tests/native/` asserts label is always in `{"ripe","unripe","overripe"}`
- [ ] `FR-003`, `SC-002` verified

---

## Backlog Rank 5 — ASP.NET Pipeline Steps

**Files**: `src/c-sharp/asp.net/Pipeline/IPipelineStep.cs` (new), `src/c-sharp/asp.net/Pipeline/PipelineRunner.cs` (new), `Pipeline/Steps/InputValidationStep.cs` (new)

- [ ] `IPipelineStep<PipelineContext>` interface created with `int Order` and `Task<PipelineStepResult> ExecuteAsync(PipelineContext ctx, CancellationToken ct)`
- [ ] `PipelineStepResult` created with `bool IsSuccess` and `ProblemDetails? Problem`
- [ ] `PipelineRunner` executes steps ordered ascending by `Order`, halts on first failure
- [ ] `InputValidationStep` (Order=10) wired into `RipenessController` flow
- [ ] Unit test: step ordering and halt-on-failure behavior verified
- [ ] Unit test coverage ≥ 80% on `PipelineRunner`
- [ ] `FR-007`, `FR-008`, `SC-005`, `SC-006` verified

---

## Cross-Cutting Checks

- [ ] `BANANA_NATIVE_PATH` missing returns 503 + `ProblemDetails` (not 500 + stack trace) — `FR-009`
- [ ] Native empty-string input returns a defined status code, not a segfault
- [ ] All existing tests still pass after the full slice is implemented
- [ ] No new GitHub Actions workflows, compose profiles, or Spec Kit scripts created

---

## Freeze Enforcement

No work started or merged in these categories until at least Ranks 1, 2, and 4 are marked complete above:

- [ ] (Freeze) New GitHub Actions workflows
- [ ] (Freeze) New `.specify/scripts/` additions
- [ ] (Freeze) New compose profiles
- [ ] (Freeze) Coverage denominator/threshold script changes
- [ ] (Freeze) Additional P/Invoke declarations

> Mark each freeze item with `[x]` only if the freeze was intentionally lifted with written justification.

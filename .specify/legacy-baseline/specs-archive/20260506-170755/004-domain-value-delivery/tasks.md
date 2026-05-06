---
description: "Task list for Domain Value Delivery — End-to-End Ripeness MVP"
---

# Tasks: Domain Value Delivery — End-to-End Ripeness MVP

**Input**: Design documents from `.specify/specs/004-domain-value-delivery/`
**Prerequisites**: plan.md ✅, spec.md ✅, research.md ✅, data-model.md ✅, contracts/ ✅
**Branch**: `004-domain-value-delivery`
**Date**: 2026-04-26

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no blocking dependencies)
- **[US#]**: Which user story this task belongs to (see spec.md)
- Exact file paths included in all descriptions

## User Story Map

| ID | Priority | Title | Depends On |
|----|----------|-------|-----------|
| US1 | P1 | Ripeness Prediction End-to-End (React UI) | Phase 2 + US2 |
| US2 | P2 | Typed ASP.NET Pipeline Responses | Phase 2 |
| US3 | P3 | Native ML Ripeness Upgrade | — (independent) |
| US4 | P4 | ASP.NET Pipeline Steps | — (independent) |

---

## Phase 1: Setup

**Purpose**: No new project initialization is required — all four domain projects exist. This phase verifies green baselines before any changes.

- [x] T001 Verify native baseline: run `cmake --build build/native && ctest --test-dir build/native` — confirm all tests pass before modifying `src/native/core/banana_ripeness.c` (verified 2026-04-27: ctest 8/8 PASS via `ctest --preset default`)
- [x] T002 [P] Verify ASP.NET baseline: run `dotnet test tests/unit` and `dotnet test tests/integration` — confirm all tests pass before modifying any `src/c-sharp/asp.net/` files (verified 2026-04-27: 69/69 unit PASS)
- [x] T003 [P] Verify TypeScript baseline: run `bun run --cwd src/typescript/shared/ui tsc --noEmit` and `bun run --cwd src/typescript/react tsc --noEmit` — confirm no type errors before modifying any TypeScript files (verified 2026-04-27: clean exit on both)

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: The `Ripeness` type fix in `@banana/ui` and the new `RipenessResult` record in Pipeline/ are referenced by both US1 and US2. Both must land before those stories can be implemented. These tasks have no internal dependencies on each other and can run in parallel.

- [X] T004 [P] Fix `Ripeness` type in `src/typescript/shared/ui/src/types.ts`: change `export type Ripeness = 'green'|'yellow'|'spotted'|'brown'` to `export type Ripeness = 'ripe'|'unripe'|'overripe'`
- [X] T005 [P] Update `COLORS` map in `src/typescript/shared/ui/src/components/RipenessLabel.tsx`: remove keys `green`, `yellow`, `spotted`, `brown`; add `ripe: "bg-yellow-200 text-yellow-900"`, `unripe: "bg-green-200 text-green-900"`, `overripe: "bg-amber-700 text-amber-50"`
- [X] T006 [P] Add unknown-value guard to `src/typescript/shared/ui/src/components/RipenessLabel.tsx`: if `COLORS[value]` is undefined, fall back to a neutral style (e.g., `bg-gray-100 text-gray-600`) and render the raw string — prevents runtime crash on stale API responses
- [X] T007 [P] Create `src/c-sharp/asp.net/Pipeline/RipenessResult.cs`: `namespace Banana.Api.Pipeline; public sealed record RipenessResult(string Label, double Confidence, string Model);`
- [X] T008 Verify type fix: run `bun run --cwd src/typescript/shared/ui tsc --noEmit` — confirm zero errors after T004–T006

---

## Phase 3: US2 — Typed ASP.NET Pipeline Responses (P2)

**Story goal**: All native-backed controller responses return typed objects. No `Ok(new { json })` wrapper remains.
**Independent test**: `curl -s -X POST http://localhost:5000/ripeness/predict -H "Content-Type: application/json" -d '{"inputJson":"yellow banana"}' | jq '.label'` returns `"ripe"`, `"unripe"`, or `"overripe"` — not `null`.

- [X] T009 [US2] Add `InputJson` property to `src/c-sharp/asp.net/Pipeline/PipelineContext.cs`: `public string? InputJson { get; set; }` — preserves all existing properties (`Route`, `StartedAt`, `LastStatus`, `Diagnostics`)
- [X] T010 [US2] Update `src/c-sharp/asp.net/Controllers/RipenessController.cs`: add `using System.Text.Json;` import; replace `Ok(new { json })` with `Ok(JsonSerializer.Deserialize<RipenessResult>(json, new JsonSerializerOptions { PropertyNameCaseInsensitive = true }))` where `RipenessResult` is from `Banana.Api.Pipeline`; set `ctx.InputJson = req.InputJson` before the native call
- [X] T011 [P] [US2] Update `src/c-sharp/asp.net/Controllers/BananaMlController.cs`: apply same pattern as T010 — deserialize native JSON string to typed result record; define a `BinaryClassificationResult` and/or `TransformerClassificationResult` record in `src/c-sharp/asp.net/Pipeline/` appropriate to the endpoint's return shape
- [X] T012 [P] [US2] Update `src/c-sharp/asp.net/Controllers/NotBananaController.cs`: apply same deserialization pattern — remove `Ok(new { json })` if present; define appropriate result record in Pipeline/
- [X] T013 [US2] Run `dotnet build src/c-sharp/asp.net/Banana.Api.csproj` — confirm zero compile errors after T009–T012
- [X] T014 [US2] Run `dotnet test tests/unit` and `dotnet test tests/integration` — confirm all existing tests pass; if any test asserts on the old `{ "json": "..." }` shape, update it to assert on the new typed property shape

---

## Phase 4: US3 — Native ML Ripeness Upgrade (P3)

**Story goal**: `banana_core_predict_ripeness` uses `banana_ml_predict_regression_score` instead of keyword counting. Label is always one of `"ripe"`, `"unripe"`, `"overripe"`.
**Independent test**: `ctest --test-dir build/native -R ripeness` — all tests pass; output JSON has `"model":"regression"`.

*Can run in parallel with Phase 3 — different domain, different files.*

- [X] T015 [P] [US3] Rewrite `banana_core_predict_ripeness` in `src/native/core/banana_ripeness.c`:
  1. Remove `count_term_ci` calls and the heuristic score calculation
  2. Call `banana_ml_predict_regression_score(input_json, &score)` (declared in `src/native/core/domain/banana_ml_models.h`) and propagate non-OK return codes
  3. Apply threshold: `score < 0.35 → "unripe"`, `score > 0.72 → "overripe"`, else `"ripe"`
  4. Format output: `snprintf(payload, sizeof(payload), "{\"label\":\"%s\",\"confidence\":%.4f,\"model\":\"regression\"}", label, score)`
  5. Call existing `duplicate_json(payload, out_json)` helper to set `*out_json`
  6. Keep `count_term_ci` as a static helper only if referenced elsewhere in the file; otherwise remove it
- [X] T016 [US3] Add or update ripeness label-domain test in `tests/native/` (create `tests/native/test_ripeness.c` if it does not exist, or add cases to an existing ripeness test file): assert that `banana_core_predict_ripeness` returns `BANANA_OK` for a valid input, and that the returned `label` field in the output JSON is one of `"ripe"`, `"unripe"`, or `"overripe"`
- [X] T017 [US3] Rebuild and run native tests: `cmake --build build/native && ctest --test-dir build/native` — all tests pass

---

## Phase 5: US1 — Ripeness Prediction End-to-End, React UI (P1)

**Story goal**: A user submits a banana description in the React UI and sees `<RipenessLabel>` rendered with a valid ripeness label.
**Independent test**: Compose `docker compose up react api` → open browser → enter text → confirm `<RipenessLabel>` renders with one of `ripe`, `unripe`, `overripe` and a confidence score.

*Depends on Phase 2 (type fix) and Phase 3 (US2, ASP.NET response shape). Can start once T008 and T013 are green.*

- [X] T018 [US1] Add `predictRipeness` function to `src/typescript/react/src/lib/api.ts`:
  ```typescript
  export type RipenessResponse = { label: Ripeness; confidence: number };
  export async function predictRipeness(baseUrl: string, inputJson: string): Promise<RipenessResponse> {
    return requestJson<RipenessResponse>(baseUrl, '/ripeness/predict', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ inputJson }),
    });
  }
  ```
  Import `Ripeness` from `@banana/ui`. Use the existing `requestJson<T>` helper already in `api.ts`.
- [X] T019 [US1] Add ripeness state and handler to `src/typescript/react/src/App.tsx`:
  Add four state variables: `ripenessInput` (string), `ripenessResult` (`RipenessResponse | null`), `ripenessError` (`string | null`), `ripenessLoading` (boolean). Add handler `handleRipenessSubmit` that calls `predictRipeness(apiBaseUrl, ripenessInput)`, sets result or error, and resets loading state.
- [X] T020 [US1] Add ripeness panel JSX to `src/typescript/react/src/App.tsx`: render a `<section>` containing a `<textarea>` bound to `ripenessInput`, a submit button that calls `handleRipenessSubmit` (disabled during `ripenessLoading`), and — when `ripenessResult` is set — `<RipenessLabel value={ripenessResult.label} />` and a confidence display. Show `ripenessError` as error text when set. Place this section above the chat section.
- [X] T021 [US1] Import `predictRipeness` and `RipenessResponse` from `./lib/api` in `src/typescript/react/src/App.tsx` — the existing `RipenessLabel` import from `@banana/ui` is already present; verify it is not duplicated
- [X] T022 [US1] Run `bun run --cwd src/typescript/react tsc --noEmit` — confirm zero type errors after T018–T021

---

## Phase 6: US4 — ASP.NET Pipeline Steps (P4)

**Story goal**: `IPipelineStep<PipelineContext>`, `PipelineStepResult`, and `PipelineRunner<T>` exist and are wired into `RipenessController`. At least one real step (input validation) executes on every request.
**Independent test**: Unit test — `PipelineRunner` with a failing step halts before subsequent steps are called.

*Can run in parallel with Phase 5 — different layers, different files.*

- [X] T023 [P] [US4] Create `src/c-sharp/asp.net/Pipeline/PipelineStepResult.cs`:
  ```csharp
  namespace Banana.Api.Pipeline;
  public sealed record PipelineStepResult(bool IsSuccess, Microsoft.AspNetCore.Mvc.ProblemDetails? Problem = null)
  {
      public static PipelineStepResult Ok() => new(true);
      public static PipelineStepResult Fail(string title, int status = 400) =>
          new(false, new() { Title = title, Status = status });
  }
  ```
- [X] T024 [P] [US4] Create `src/c-sharp/asp.net/Pipeline/IPipelineStep.cs`:
  ```csharp
  namespace Banana.Api.Pipeline;
  public interface IPipelineStep<T>
  {
      int Order { get; }
      Task<PipelineStepResult> ExecuteAsync(T context, CancellationToken ct);
  }
  ```
- [X] T025 [P] [US4] Create `src/c-sharp/asp.net/Pipeline/PipelineRunner.cs`:
  ```csharp
  namespace Banana.Api.Pipeline;
  public sealed class PipelineRunner<T>
  {
      private readonly IReadOnlyList<IPipelineStep<T>> _steps;
      public PipelineRunner(IEnumerable<IPipelineStep<T>> steps) =>
          _steps = [.. steps.OrderBy(s => s.Order)];
      public async Task<PipelineStepResult> RunAsync(T context, CancellationToken ct = default)
      {
          foreach (var step in _steps)
          {
              var result = await step.ExecuteAsync(context, ct);
              if (!result.IsSuccess) return result;
          }
          return PipelineStepResult.Ok();
      }
  }
  ```
- [X] T026 [P] [US4] Create `src/c-sharp/asp.net/Pipeline/Steps/InputValidationStep.cs`:
  ```csharp
  namespace Banana.Api.Pipeline.Steps;
  public sealed class InputValidationStep : IPipelineStep<PipelineContext>
  {
      public int Order => 10;
      public Task<PipelineStepResult> ExecuteAsync(PipelineContext ctx, CancellationToken ct) =>
          Task.FromResult(string.IsNullOrWhiteSpace(ctx.InputJson)
              ? PipelineStepResult.Fail("input_json is required", 400)
              : PipelineStepResult.Ok());
  }
  ```
- [X] T027 [US4] Wire `PipelineRunner<PipelineContext>` and `InputValidationStep` into `src/c-sharp/asp.net/Controllers/RipenessController.cs`: inject `PipelineRunner<PipelineContext>` via constructor; set `ctx.InputJson = req.InputJson`; call `var pipelineResult = await runner.RunAsync(ctx, ct)` before the native call; if `!pipelineResult.IsSuccess` return `BadRequest(pipelineResult.Problem)`
- [X] T028 [US4] Register `InputValidationStep` and `PipelineRunner<PipelineContext>` in DI in `src/c-sharp/asp.net/Program.cs`: `builder.Services.AddScoped<IPipelineStep<PipelineContext>, InputValidationStep>(); builder.Services.AddScoped<PipelineRunner<PipelineContext>>();`
- [X] T029 [US4] Create `tests/unit/PipelineRunnerTests.cs` with at minimum: (a) test that steps execute in ascending `Order`; (b) test that a failing step (IsSuccess=false) halts execution and no subsequent step is called; (c) test that `PipelineRunner` with zero steps returns `IsSuccess=true`
- [X] T030 [US4] Run `dotnet test tests/unit` — all tests including T029 pass; `PipelineRunner` branch coverage ≥ 80%

---

## Final Phase: Polish & Cross-Cutting Validation

**Purpose**: Confirm all four layers compose correctly end-to-end and no regression was introduced.

- [X] T031 Run full test suite: `cmake --build build/native && ctest --test-dir build/native` (native), `dotnet test tests/unit && dotnet test tests/integration` (ASP.NET), `bun run --cwd src/typescript/shared/ui tsc --noEmit` and `bun run --cwd src/typescript/react tsc --noEmit` (TypeScript) — all pass
- [X] T032 [P] Verify 503 path: start the API with `BANANA_NATIVE_PATH` pointing to a non-existent path; confirm `POST /ripeness/predict` returns HTTP 503 (not 500 with a stack trace)
- [X] T033 [P] Verify empty-input path: call `POST /ripeness/predict` with `{"inputJson":""}` — confirm HTTP 400 (not a native crash)
- [X] T034 Update `src/typescript/shared/ui` package version (patch bump in `src/typescript/shared/ui/package.json`) so downstream React and Electron builds pick up the `Ripeness` type change cleanly

---

## Dependency Graph

```
T001, T002, T003           (Phase 1 — parallel baseline checks)
       │
T004–T007                  (Phase 2 — parallel type + record foundational)
       │
   T008 ─────────────────────────────────────────────────────────────┐
       │                                                             │
T009–T014 (US2, ASP.NET)           T015–T017 (US3, native)          │
       │                                   │                        │
   T013 ──────────────────────────────────────────────────────┐    │
                                                              │    │
                                                      T018–T022 (US1, React)
                                                              │
T023–T030 (US4, pipeline — parallel with US1)                │
       │                                                      │
       └───────────────────────┬───────────────────────────────┘
                               │
                         T031–T034 (Final validation)
```

## Parallel Execution Opportunities

| Parallel Group | Tasks | Notes |
|----------------|-------|-------|
| Baseline verification | T001, T002, T003 | Different test runners, all read-only |
| Type fix + record | T004, T005, T006, T007 | Different files, no dependencies |
| ASP.NET controller fixes | T011, T012 | Different controller files |
| US3 (native) alongside US2 (ASP.NET) | T015–T017 alongside T009–T014 | Completely different layers |
| US4 (pipeline) alongside US1 (React) | T023–T030 alongside T018–T022 | Different layers, no shared files |
| Final spot checks | T032, T033 | Different API paths |

## Implementation Strategy

**MVP scope** (minimum to produce visible end-to-end result):
1. Phase 2 (T004–T008) — type fix, 15 min
2. Phase 3/US2 (T009–T014) — ASP.NET response fix, 30 min
3. Phase 5/US1 (T018–T022) — React form + RipenessLabel, 30 min

MVP total: ~75 min. After MVP, React shows a functional ripeness form with real (heuristic) labels.

**Full slice** adds US3 (native ML, improves accuracy) and US4 (pipeline steps, improves architecture) — neither blocks the visible UI behavior.

**Suggested agent routing**:
| Phase | Tasks | Agent |
|-------|-------|-------|
| Phase 2 (TS type fix) | T004–T008 | `react-ui-agent` |
| Phase 3 (ASP.NET) | T009–T014 | `api-pipeline-agent` |
| Phase 4 (native C) | T015–T017 | `native-core-agent` |
| Phase 5 (React UI) | T018–T022 | `react-ui-agent` |
| Phase 6 (pipeline steps) | T023–T030 | `api-pipeline-agent` |
| Final | T031–T034 | `integration-agent` |

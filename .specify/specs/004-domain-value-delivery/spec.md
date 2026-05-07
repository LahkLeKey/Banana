# Feature Specification: Domain Value Delivery — End-to-End Ripeness MVP

**Feature Branch**: `004-domain-value-delivery`
**Created**: 2026-04-26
**Status**: Draft
**Scoring basis**: Value-risk prioritized backlog (April 2026) — ranks 1–5 of scored domain backlog

---

## In Scope *(mandatory)*

- Fix double-encoded JSON responses in ASP.NET controllers so `GET/POST /ripeness/predict`, `/ml/binary`, `/ml/transformer`, `/not-banana/predict` return typed objects instead of `{"json":"<raw string>"}`.
- Add `sealed record RipenessResult(string Label, double Confidence, string Model)` and equivalent ML response records to the ASP.NET Pipeline layer.
- Upgrade `banana_core_predict_ripeness` in native C from keyword counting to an ML-derived prediction using `banana_ml_build_feature_vector` → `banana_ml_regression_predict` → threshold-based label.
- Fix the broken `Ripeness` type contract in `@banana/ui` so the shared type matches what the native layer emits.
- Wire `RipenessLabel` component into the React UI with a form input and result display.
- Add `predictRipeness()` to the React API client library.
- Implement the `IPipelineStep` interface and `PipelineRunner` in the ASP.NET Pipeline layer so cross-cutting concerns (validation, error normalization) are not ad-hoc per controller.

## Out of Scope *(mandatory)*

- New GitHub Actions workflows, compose profiles, or `.specify/scripts/` automation.
- New Prisma migrations or `banana_profile` DAL wiring (tracked separately in Rank 8).
- `HarvestBatchController`, `TruckController`, or `BatchController.PredictBatchRipeness` HTTP surfaces (tracked in Rank 7).
- TypeScript Fastify API ripeness route (React will call ASP.NET directly via `VITE_BANANA_API_BASE_URL` for the MVP).
- Chat enrichment with ripeness context.
- Mobile or Electron runtime changes.
- Model retraining or corpus feedback triggers.

## User Scenarios & Testing *(mandatory)*
## Problem Statement

Feature Specification: Domain Value Delivery — End-to-End Ripeness MVP aims to improve system capability and user experience by implementing the feature described in the specification.


### User Story 1 — Ripeness Prediction End-to-End (Priority: P1)

A developer submits a banana description to the React UI, gets back a ripeness label (`ripe`, `unripe`, or `overripe`) and confidence score displayed using the shared `RipenessLabel` component.

**Why this priority**: This is the primary product demonstration. It exercises all four domain layers (native ML → ASP.NET pipeline → React UI → shared component) and produces visible output. Without it the product has no demonstrable behavior.

**Independent Test**: Can be validated end-to-end by running the compose stack, opening the React app, submitting any text input, and confirming `<RipenessLabel>` renders with a valid value from the `ripe | unripe | overripe` set and a confidence decimal.

**Acceptance Scenarios**:

1. **Given** the React app is running, **When** a user submits the text `"yellow soft banana"`, **Then** the response displays `RipenessLabel` with label `ripe` or `overripe` and a confidence between 0 and 1.
2. **Given** `GET /ripeness/predict` is called directly, **Then** the response body is `{ "label": "ripe", "confidence": 0.81, "model": "regression" }` — not `{ "json": "..." }`.
3. **Given** an unknown or empty input, **Then** the API returns a graceful result (not a 500) and the UI displays the label with `confidence < 0.5`.

---

### User Story 2 — Typed ASP.NET Pipeline Responses (Priority: P2)

Any controller response that currently wraps a raw native JSON string is replaced with a deserialized, strongly-typed response record. All affected controllers pass integration tests that assert property shape, not string content.

**Why this priority**: The double-encoded response bug is an undetected API contract break. Every consumer (React, tests, external callers) must do an extra `JSON.parse` to read results — this silently corrupts the API surface.

**Independent Test**: Integration test calling each affected endpoint and asserting `response.label` (string) and `response.confidence` (number) exist as first-class properties.

**Acceptance Scenarios**:

1. **Given** `POST /ml/binary` is called with valid input, **Then** the JSON response has top-level `label` and `confidence` fields, no `json` wrapper key.
2. **Given** `POST /not-banana/predict` is called, **Then** `response.isNotBanana` (bool) and `response.confidence` (double) exist at the top level.
3. **Given** `RipenessController.PredictRipeness` calls the native layer and it returns a status code indicating failure, **Then** the pipeline returns a `ProblemDetails` response (not a 200 with an error string inside `json`).

---

### User Story 3 — Native ML Ripeness Prediction (Priority: P3)

`banana_core_predict_ripeness` uses the ML feature pipeline (`banana_ml_build_feature_vector` → `banana_ml_regression_predict`) instead of keyword counting, and produces label values matching the API contract (`ripe`, `unripe`, `overripe`).

**Why this priority**: The native layer is the core differentiator. The keyword-counting implementation defeats the purpose of having an ML domain. This item unlocks accurate results for US1 and P2 and is independently testable via `ctest`.

**Independent Test**: `tests/native/` C test: feed known inputs and assert labels are drawn from `{"ripe","unripe","overripe"}` and confidence is in `(0,1]`. No keyword-counting code paths remain.

**Acceptance Scenarios**:

1. **Given** input `"very green firm banana"`, **When** `banana_core_predict_ripeness` runs, **Then** label is `"unripe"` and confidence is a float in `(0,1]`.
2. **Given** input `"brown mushy overripe banana"`, **Then** label is `"overripe"`.
3. **Given** `banana_ml_build_feature_vector` returns a non-OK status, **Then** `banana_core_predict_ripeness` propagates the error code without crashing.

---

### User Story 4 — ASP.NET Pipeline Steps (Priority: P4)

`IPipelineStep<PipelineContext>` interface and a `PipelineRunner` exist in `src/c-sharp/asp.net/Pipeline/`. At least one real step (input validation) is wired into the `RipenessController` flow so cross-cutting concerns are not ad-hoc.

**Why this priority**: Without a real pipeline, each controller duplicates error handling and validation ad-hoc. The pipeline is the documented architecture; building controllers without it creates technical debt that blocks clean expansion.

**Independent Test**: Unit test: `PipelineRunner` executes steps in `Order` sequence; a step returning a non-OK status halts execution and the controller returns the correct HTTP status code.

**Acceptance Scenarios**:

1. **Given** a pipeline is configured with an `InputValidationStep` (Order=10) and a `NativeCallStep` (Order=20), **When** the validation step returns failure, **Then** `NativeCallStep` is never called.
2. **Given** all steps pass, **Then** the controller returns the typed result record from US2.

---

### Edge Cases

- What happens when the native library is not loaded (`BANANA_NATIVE_PATH` missing)? ASP.NET must return 503 with `ProblemDetails`, not a 500 with a null-deref stack trace.
- What happens when `banana_ml_build_feature_vector` receives an empty string? Native function must return a defined status code, not segfault.
- What happens when `Ripeness` type values in the UI receive an unexpected string (e.g., `"green"` from old native code)? `RipenessLabel` must not crash — render a neutral/unknown state.
- What happens when the React API call times out? UI should show an error state, not a blank `RipenessLabel`.

---

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: The ASP.NET API MUST return deserialized typed objects from all native-backed endpoints, not raw JSON strings wrapped in an outer object.
- **FR-002**: `RipenessResult` MUST be a `sealed record` in the Pipeline layer with fields `string Label`, `double Confidence`, `string Model`.
- **FR-003**: `banana_core_predict_ripeness` MUST call `banana_ml_build_feature_vector` and `banana_ml_regression_predict` and map the score to one of exactly three labels: `"ripe"`, `"unripe"`, `"overripe"`.
- **FR-004**: The `Ripeness` type in `@banana/ui` MUST be `'ripe' | 'unripe' | 'overripe'` and the `RipenessLabel` `COLORS` map MUST key on those values.
- **FR-005**: The React app MUST expose a text input that calls `predictRipeness()` and renders `<RipenessLabel value={label} />` with the returned label.
- **FR-006**: `predictRipeness(baseUrl: string, inputJson: string): Promise<{ label: Ripeness; confidence: number }>` MUST be exported from `src/typescript/react/src/lib/api.ts`.
- **FR-007**: `IPipelineStep<PipelineContext>` MUST define `int Order` and `Task<PipelineStepResult> ExecuteAsync(PipelineContext ctx, CancellationToken ct)`.
- **FR-008**: `PipelineRunner` MUST execute steps ordered ascending by `Order`, halt on first non-OK result, and never call subsequent steps after a failure.
- **FR-009**: `BANANA_NATIVE_PATH` absence or library load failure MUST produce a 503 response with `ProblemDetails`, not an unhandled exception.

### Key Entities

- **`RipenessResult`**: Pipeline-layer record — `Label: string`, `Confidence: double`, `Model: string`. Replaces raw JSON wrapper in API responses.
- **`RipenessFeatureVector`**: Native C struct populated by `banana_ml_build_feature_vector` — input to regression and binary classifiers.
- **`PipelineContext`**: Existing ASP.NET data bag — extended with `RipenessResult?` output slot used by pipeline steps.
- **`IPipelineStep<T>`**: New interface — `Order` property + `ExecuteAsync` method.
- **`Ripeness`** (shared UI): Union type `'ripe' | 'unripe' | 'overripe'` — consumed by `RipenessLabel`.

---

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: `GET /ripeness/predict` response body has `label` and `confidence` as top-level JSON fields — verifiable with a `curl` one-liner without any client-side `JSON.parse`.
- **SC-002**: `banana_core_predict_ripeness` passes all existing `ctest` native tests and new label-domain tests (label is always in `{"ripe","unripe","overripe"}`).
- **SC-003**: `RipenessLabel` renders without TypeScript type errors in the React app — `tsc --noEmit` passes with the corrected type.
- **SC-004**: The React compose channel (`docker compose up react`) shows a functional ripeness form and label in the browser.
- **SC-005**: `PipelineRunner` unit test coverage ≥ 80% on the new step execution path.
- **SC-006**: No controller file calls `JsonSerializer.Deserialize` directly — deserialization is owned by the Pipeline layer or `NativeBananaClient`.

---

## Assumptions

- `banana_ml_build_feature_vector`, `banana_ml_regression_predict`, and their headers in `src/native/core/domain/banana_ml_models.h` are callable from `banana_ripeness.c` without ABI changes.
- The score-to-label thresholds (`< 0.35 = "unripe"`, `> 0.72 = "overripe"`, else `"ripe"`) are the agreed decision boundaries — any change requires a separate spec.
- `VITE_BANANA_API_BASE_URL` is the only API base URL contract for the React app; the ripeness form calls the ASP.NET API, not the TS Fastify API.
- Existing `NativeBananaClient` P/Invoke declarations for `PredictRipeness` are correct and do not need ABI changes.
- The `@banana/ui` package version bump required by the type fix is a non-breaking semver patch increment consumed by the React app automatically via file dependency.
- Electron and mobile channels are not exercised by this spec slice — they pick up the type fix passively via shared package resolution.

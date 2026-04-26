# Tasks: Ensemble Verdict Embedding Passthrough

**Branch**: `017-ensemble-verdict-embedding-passthrough` | **Created**: 2026-04-26

## Phase 1 -- Setup

- [x] T001 Repoint `.specify/feature.json` ->
  `017-ensemble-verdict-embedding-passthrough`.
- [x] T002 Author `analysis/in-scope-files.md` and
  `analysis/validation-commands.md`.

## Phase 2 -- Foundational interop

- [x] T003 Add
  `int ClassifyBananaTransformerWithEmbedding(string inputJson, double[] embedding, out string outJson)`
  to `src/c-sharp/asp.net/NativeInterop/INativeBananaClient.cs` and
  implement it in `NativeBananaClient.cs` by calling the existing native
  `banana_classify_banana_transformer_ex` (log_attention=0,
  attention buffers null, embedding buffer = caller-allocated 4 doubles).
- [x] T004 Extend `tests/unit/TestDoubles/FakeNativeBananaClient.cs`
  with public settable `ClassifyTransformerWithEmbeddingStatus`,
  `ClassifyTransformerWithEmbeddingJson`, and
  `ClassifyTransformerWithEmbeddingValues` (4 doubles).

## Phase 3 -- Result type + route

- [x] T005 Add `EnsembleVerdictWithEmbeddingResult` record in
  `src/c-sharp/asp.net/Pipeline/Results/` with documented field order
  `{ verdict, embedding }`.
- [x] T006 Add an internal helper to `BananaMlController` (or a fourth
  embedding-aware step gated on a sentinel `ctx.Route ==
  "/ml/ensemble/embedding"`) that re-runs the transformer with
  embedding when the cascade escalated.
- [x] T007 Add `[HttpPost("ensemble/embedding")]` route to
  `BananaMlController.cs` that returns
  `EnsembleVerdictWithEmbeddingResult`. On cheap-path verdicts,
  embedding=null. On escalated verdicts, embedding=double[4].
- [x] T008 DI -- register any new step. Re-use existing gating /
  escalation / calibration steps; do not duplicate logic.
- [x] T009 `dotnet build` clean.

## Phase 4 -- Tests

- [x] T010 Unit tests in `tests/unit/EnsembleEmbeddingPassthroughTests.cs`
  covering all 6 SPIKE anchor walks: 3 cheap-path -> embedding=null,
  3 escalated -> embedding length=4 with finite components.
- [x] T011 Snapshot test asserting the legacy `/ml/ensemble` payload
  shape is byte-identical to slice 014 (no field drift).
- [x] T012 Test asserting cheap path does NOT call
  `ClassifyBananaTransformerWithEmbedding`.
- [x] T013 `dotnet test src/c-sharp/asp.net/asp.net.sln` green.

## Phase 5 -- Validation + close-out

- [x] T014 `cmake --build --preset default && ctest --preset default
  --output-on-failure` -- expect 7/7 unchanged. Capture in
  `analysis/native-lane-evidence.md`.
- [x] T015 Flip slice 016's trigger to FIRED in
  `.specify/specs/013-ml-brain-composition-spike/analysis/deferred-registry.md`;
  mark all 017 tasks `[x]` and update `README.md` to COMPLETE.

## Out of scope

- Native changes; ABI bump.
- Quantization (slice 016).
- React/Electron consumption.

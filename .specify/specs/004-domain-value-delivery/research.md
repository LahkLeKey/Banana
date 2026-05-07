# Research: Domain Value Delivery

**Branch**: `004-domain-value-delivery` | **Phase**: 0 | **Date**: 2026-04-26

---

## R-01: Native ML entry point for ripeness prediction

**Question**: Does `banana_ml_build_feature_vector` / `banana_ml_regression_predict` exist in the native codebase?

**Finding**: **No.** Those function names were speculative. The actual header `src/native/core/domain/banana_ml_models.h` declares:

```c
int banana_ml_predict_regression_score(const char* input_json, double* out_score);
int banana_ml_classify_binary(const char* input_json, BananaMlClassificationResult* out_result);
int banana_ml_classify_transformer(const char* input_json, BananaMlClassificationResult* out_result);
int banana_ml_classify_not_banana_junk(const char* input_json, BananaMlClassificationResult* out_result);
```

**Decision**: `banana_core_predict_ripeness` will call `banana_ml_predict_regression_score(input_json, &score)` directly. No intermediate feature-vector step is needed.

**Rationale**: `banana_ml_predict_regression_score` takes `input_json` directly — same signature pattern as all other ML calls in the codebase.

**Score-to-label mapping** (confirmed by spec FR-003): `score < 0.35 → "unripe"`, `score > 0.72 → "overripe"`, else `"ripe"`.

---

## R-02: ASP.NET deserialization ownership

**Question**: Where should `JsonSerializer.Deserialize<T>` live — controller, `NativeBananaClient`, or a Pipeline step?

**Finding**: `NativeBananaClient` currently returns a raw `string` (`out var json`) for all native results. `PipelineContext` is a data bag. Controllers currently own the `Ok(new { json })` call site.

**Decision**: Deserialization lives in the **controller** for this slice, replacing `Ok(new { json })` with `Ok(JsonSerializer.Deserialize<RipenessResult>(json))`. A follow-on pipeline step refactor (Rank 5) can move this later. Keeping it in controllers for now minimizes blast radius and does not require `NativeBananaClient` to know about response types.

**Rationale**: `NativeBananaClient` is an interop boundary — it owns P/Invoke surface, not JSON shaping. Controllers are the first managed layer with full type information.

**Alternative rejected**: Moving deserialization into `NativeBananaClient` would require the interop layer to reference Pipeline types, creating a backwards dependency.

---

## R-03: `RipenessResult` record location

**Question**: Where does the new `sealed record RipenessResult` live?

**Decision**: `src/c-sharp/asp.net/Pipeline/RipenessResult.cs` (new file). The Pipeline namespace already hosts `PipelineContext` and `StatusMapping` — result records belong in the same layer.

**Pattern confirmed**: `RipenessController` already has a local `RipenessRequest` record — the pattern of defining request/response shapes adjacent to usage is established. Moving result shapes to Pipeline/ keeps them available across controllers without circular refs.

---

## R-04: React ripeness form placement in App.tsx

**Question**: Where in `App.tsx` does the ripeness form live without conflicting with the existing chat + banana badge layout?

**Finding**: `App.tsx` already has: `BananaBadge` (top), bootstrap/chat session flow (middle), chat message list + draft input (bottom). `RipenessLabel` is imported but unused.

**Decision**: Add a standalone `RipenessPanel` section above the chat section. It is self-contained with its own `useState` — no coupling to chat session state. Error/loading state co-located in the same section.

**Rationale**: Keeps concerns separate. The chat section and the ripeness section are independent product features; they should not share state.

---

## R-05: `Ripeness` type migration impact on downstream packages

**Question**: Does changing `Ripeness` from `'green'|'yellow'|'spotted'|'brown'` to `'ripe'|'unripe'|'overripe'` break other consumers of `@banana/ui`?

**Finding**: `@banana/ui` is consumed by:
- `src/typescript/react/` — imports `RipenessLabel` (currently dead, no usage of the old type)
- `src/typescript/electron/` — does not reference `RipenessLabel` or `Ripeness` directly
- `src/typescript/react-native/` — does not reference `RipenessLabel` or `Ripeness` directly

**Decision**: Safe to change. No existing code uses the current `Ripeness` union values outside `RipenessLabel.tsx` itself.

**Migration**: Update `types.ts` + `RipenessLabel.tsx` COLORS map in one commit. No consumers need changes other than `App.tsx` which will use the new type via `predictRipeness()`.

---

## R-06: `IPipelineStep` scope for this slice

**Question**: Does the pipeline step infrastructure need to be fully wired to all controllers for Rank 5, or is one controller sufficient?

**Decision**: Wire `InputValidationStep` + `PipelineRunner` through `RipenessController` only for this slice. Other controllers continue to use `StatusMapping.ToActionResult` directly. Rank 5 is scoped as "architecture proof" — a second controller can be wired in a follow-on slice.

**Rationale**: The goal is to have a real pipeline execution path that is tested, not a global rollout. Full controller migration is a separate slice.

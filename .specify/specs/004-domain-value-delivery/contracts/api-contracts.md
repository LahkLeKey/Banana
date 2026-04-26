# API Contracts: Domain Value Delivery

**Branch**: `004-domain-value-delivery` | **Phase**: 1 | **Date**: 2026-04-26

These contracts define the HTTP and TypeScript surfaces that this feature slice establishes or repairs.

---

## ASP.NET HTTP Endpoints

### `POST /ripeness/predict`

**Controller**: `RipenessController.Predict`
**Change**: Response shape is fixed (was double-encoded JSON string).

**Request**:
```json
{ "inputJson": "yellow soft banana from Colombia" }
```

**Response 200**:
```json
{
  "label": "ripe",
  "confidence": 0.7812,
  "model": "regression"
}
```

**Response 400** (empty/null inputJson):
```json
{ "error": "invalid_argument" }
```

**Response 503** (native library not loaded):
```json
{
  "type": "https://tools.ietf.org/html/rfc9110#section-15.6.4",
  "title": "Service Unavailable",
  "status": 503
}
```

---

## TypeScript API Client

### `predictRipeness(baseUrl, inputJson)` — **New function**

**File**: `src/typescript/react/src/lib/api.ts`

```typescript
import type { Ripeness } from '@banana/ui';

export type RipenessResponse = {
  label: Ripeness;
  confidence: number;
};

export async function predictRipeness(
  baseUrl: string,
  inputJson: string
): Promise<RipenessResponse> {
  return requestJson<RipenessResponse>(baseUrl, '/ripeness/predict', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ inputJson }),
  });
}
```

**Notes**:
- Uses the existing `requestJson<T>` helper already in `api.ts`.
- `baseUrl` resolves via `resolveApiBaseUrl()` (VITE_BANANA_API_BASE_URL or Electron bridge).
- `Ripeness` is imported from `@banana/ui` — after the type fix, this is `'ripe'|'unripe'|'overripe'`.

---

## Shared UI Component Contract

### `<RipenessLabel value={Ripeness} />` — **Type fix**

**File**: `src/typescript/shared/ui/src/components/RipenessLabel.tsx`

```typescript
// Updated COLORS map
const COLORS: Record<Ripeness, string> = {
  ripe:     "bg-yellow-200 text-yellow-900",
  unripe:   "bg-green-200 text-green-900",
  overripe: "bg-amber-700 text-amber-50",
};
```

**Consumer contract**: The `value` prop accepts exactly `'ripe' | 'unripe' | 'overripe'`. Any other string is a TypeScript type error at the call site.

---

## Native C ABI (unchanged, documented for reference)

### `banana_core_predict_ripeness`

**Header**: `src/native/core/banana_core.h`
**Signature**: `int banana_core_predict_ripeness(const char* input_json, char** out_json)`

**Callee** (after fix): `banana_ml_predict_regression_score(input_json, &score)` from `banana_ml_models.h`

**Output JSON** (`*out_json`, caller must free):
```json
{"label":"ripe","confidence":0.7812,"model":"regression"}
```

**Status codes returned**: `BANANA_OK`, `BANANA_INVALID_ARGUMENT`, any code propagated from `banana_ml_predict_regression_score`.

---

## Pipeline Step Contract

### `IPipelineStep<PipelineContext>` — **New interface**

```csharp
// Minimum viable contract for this slice
public interface IPipelineStep<T>
{
    int Order { get; }
    Task<PipelineStepResult> ExecuteAsync(T context, CancellationToken ct);
}
```

**Execution guarantee**: `PipelineRunner<T>` executes steps in ascending `Order`. First failure halts execution. No step after a failure is called.

**`PipelineContext` addition**: Property `string? InputJson` added to carry the raw request payload for validation steps. All existing properties (`Route`, `StartedAt`, `LastStatus`, `Diagnostics`) are preserved.

# Data Model: Domain Value Delivery

**Branch**: `004-domain-value-delivery` | **Phase**: 1 | **Date**: 2026-04-26

---

## Entities

### `RipenessResult` — New (ASP.NET Pipeline layer)

**Location**: `src/c-sharp/asp.net/Pipeline/RipenessResult.cs`

```csharp
namespace Banana.Api.Pipeline;

public sealed record RipenessResult(string Label, double Confidence, string Model);
```

| Field | Type | Source | Constraint |
|-------|------|--------|-----------|
| `Label` | `string` | Native JSON `label` field | One of `"ripe"`, `"unripe"`, `"overripe"` |
| `Confidence` | `double` | Native JSON `confidence` field | `0 < confidence ≤ 1` |
| `Model` | `string` | Native JSON `model` field | e.g., `"regression"` |

**JSON wire shape** (after fix):
```json
{ "label": "ripe", "confidence": 0.81, "model": "regression" }
```

**Before fix** (broken current state):
```json
{ "json": "{\"label\":\"ripe\",\"confidence\":0.81,\"model\":\"regression\"}" }
```

---

### `PipelineStepResult` — New (ASP.NET Pipeline layer)

**Location**: `src/c-sharp/asp.net/Pipeline/PipelineStepResult.cs`

```csharp
namespace Banana.Api.Pipeline;

public sealed record PipelineStepResult(bool IsSuccess, Microsoft.AspNetCore.Mvc.ProblemDetails? Problem = null)
{
    public static PipelineStepResult Ok() => new(true);
    public static PipelineStepResult Fail(string title, int status = 400) =>
        new(false, new ProblemDetails { Title = title, Status = status });
}
```

---

### `IPipelineStep<T>` — New (ASP.NET Pipeline layer)

**Location**: `src/c-sharp/asp.net/Pipeline/IPipelineStep.cs`

```csharp
namespace Banana.Api.Pipeline;

public interface IPipelineStep<T>
{
    int Order { get; }
    Task<PipelineStepResult> ExecuteAsync(T context, CancellationToken ct);
}
```

---

### `PipelineRunner<T>` — New (ASP.NET Pipeline layer)

**Location**: `src/c-sharp/asp.net/Pipeline/PipelineRunner.cs`

```csharp
namespace Banana.Api.Pipeline;

public sealed class PipelineRunner<T>
{
    private readonly IEnumerable<IPipelineStep<T>> _steps;
    public PipelineRunner(IEnumerable<IPipelineStep<T>> steps) =>
        _steps = steps.OrderBy(s => s.Order);

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

---

### `InputValidationStep` — New (ASP.NET Pipeline layer)

**Location**: `src/c-sharp/asp.net/Pipeline/Steps/InputValidationStep.cs`

```csharp
namespace Banana.Api.Pipeline.Steps;

public sealed class InputValidationStep : IPipelineStep<PipelineContext>
{
    public int Order => 10;

    public Task<PipelineStepResult> ExecuteAsync(PipelineContext ctx, CancellationToken ct)
    {
        if (string.IsNullOrWhiteSpace(ctx.InputJson))
            return Task.FromResult(PipelineStepResult.Fail("input_json is required", 400));
        return Task.FromResult(PipelineStepResult.Ok());
    }
}
```

**Note**: `PipelineContext` gains an `InputJson` property (nullable string) to carry the request payload through the pipeline.

---

### `Ripeness` — Modified (Shared UI)

**Location**: `src/typescript/shared/ui/src/types.ts`

```typescript
// Before
export type Ripeness = 'green'|'yellow'|'spotted'|'brown';

// After
export type Ripeness = 'ripe'|'unripe'|'overripe';
```

**COLORS map** in `RipenessLabel.tsx`:

| Key | Before | After |
|-----|--------|-------|
| `ripe` | (missing) | `bg-yellow-200 text-yellow-900` |
| `unripe` | (missing) | `bg-green-200 text-green-900` |
| `overripe` | (missing) | `bg-amber-700 text-amber-50` |
| `green` | `bg-green-200 text-green-900` | removed |
| `yellow` | `bg-yellow-200 text-yellow-900` | removed |
| `spotted` | `bg-amber-300 text-amber-900` | removed |
| `brown` | `bg-amber-700 text-amber-50` | removed |

---

### `banana_core_predict_ripeness` — Modified (Native C)

**Location**: `src/native/core/banana_ripeness.c`

**Current implementation**: `count_term_ci` keyword-counting loop producing heuristic score.

**New implementation**:
```c
int banana_core_predict_ripeness(const char* input_json, char** out_json) {
    double score = 0.0;
    const char* label;
    char payload[256];

    if (!input_json || !out_json) return BANANA_INVALID_ARGUMENT;

    int rc = banana_ml_predict_regression_score(input_json, &score);
    if (rc != BANANA_OK) return rc;

    if (score < 0.35)      label = "unripe";
    else if (score > 0.72) label = "overripe";
    else                   label = "ripe";

    snprintf(payload, sizeof(payload),
        "{\"label\":\"%s\",\"confidence\":%.4f,\"model\":\"regression\"}",
        label, score);

    // use existing duplicate_json helper
    return duplicate_json(payload, out_json);
}
```

**State transitions** (score → label):
```
[0.00, 0.35)  →  "unripe"
[0.35, 0.72]  →  "ripe"
(0.72, 1.00]  →  "overripe"
```

---

### React ripeness state — New (React App)

**Location**: `src/typescript/react/src/App.tsx` (additions only)

```typescript
// New state co-located in App() function
const [ripenessInput, setRipenessInput] = useState('');
const [ripenessResult, setRipenessResult] = useState<{ label: Ripeness; confidence: number } | null>(null);
const [ripenessError, setRipenessError] = useState<string | null>(null);
const [ripenessLoading, setRipenessLoading] = useState(false);
```

---

## Entity Relationships

```
banana_core_predict_ripeness (native C)
    └─ calls banana_ml_predict_regression_score (native ML domain)
    └─ produces JSON: { label, confidence, model }

RipenessController (ASP.NET)
    └─ calls NativeBananaClient.PredictBananaRipeness → string json
    └─ deserializes to RipenessResult
    └─ PipelineRunner runs InputValidationStep (Order=10)
    └─ returns Ok(RipenessResult)

predictRipeness() (React api.ts)
    └─ POST /ripeness/predict → { label: Ripeness, confidence: number }

App.tsx (React)
    └─ calls predictRipeness()
    └─ renders <RipenessLabel value={result.label} />

RipenessLabel (shared UI)
    └─ maps Ripeness type to Tailwind bg class
```

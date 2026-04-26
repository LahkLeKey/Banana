# Orchestrator Boundary (G5)

The contract surface each model exposes to the future chatbot
orchestrator. Designed so adding a new model does NOT require
orchestrator changes beyond registration.

## Per-model contract

Every model image, when loaded by the API, MUST expose:

```ts
type ModelVerdict = {
    model: string;            // e.g. "banana", "not-banana", "ripeness"
    label: string;            // model-specific (e.g. "banana", "ripe")
    probability: number;      // [0, 1]
    embedding: number[] | null; // optional; null when cheap-path skipped it
    explanation?: string;     // optional free-text; deferred to the escalation model
    did_escalate?: boolean;   // optional; existing not-banana semantic
};
```

This is a superset of today's `EnsembleVerdict` shape -- existing
consumers already see `label`, `probability`, `embedding`, and
`did_escalate`; `model` is new but defaults to the only model in
the response today.

## Orchestrator registration

The orchestrator (future SPIKE) will resolve model paths from a
manifest of the form:

```json
{
    "models": {
        "not-banana": { "image": "data/not-banana/images/<sha>.tar", "channel": "stable" },
        "banana":     { "image": "data/banana/images/<sha>.tar",     "channel": "stable" },
        "ripeness":   { "image": "data/ripeness/images/<sha>.tar",   "channel": "stable" }
    }
}
```

The manifest is the single source of truth; the orchestrator
never hard-codes per-model paths. The current
`BANANA_NOT_BANANA_MODEL_PATH` env override remains for
single-model dev workflows.

## What the orchestrator does NOT need from a model

- Free-text generation (deferred with the escalation model).
- Streaming inference (deferred; current API is request /
  response).
- Cross-model attention / shared state (deferred until a model
  combination demands it).

# Implementation Plan: DS Model Ops Complexity Scale-Up

**Branch**: `171-ds-model-ops-complexity-scale-up` | **Date**: 2026-05-03 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/171-ds-model-ops-complexity-scale-up/spec.md`

## Overview

Deliver the five DS spike outcomes (166-170) as production-ready contracts across ASP.NET + React + `@banana/ui` with minimal backend scope and TypeScript-first integration. The implementation focuses on typed exports, shared utility reuse, and visible operator/workbench UI behavior.

## Technical Context

**Language/Version**: TypeScript (React + shared package), C# ASP.NET
**Primary Dependencies**: React app under `src/typescript/react`, shared package `@banana/ui` under `src/typescript/shared/ui`, ASP.NET API under `src/c-sharp/asp.net`
**Storage**: No new persistence; telemetry config response and promotion audit UI state are in-memory for this slice
**Testing**: Bun/TypeScript type-check and unit tests, React page-level behavior checks, ASP.NET endpoint contract check
**Target Platform**: Local + compose runtime channels, browser UI for React pages, ASP.NET API host
**Project Type**: Monorepo web feature with shared UI library exports and one API endpoint
**Performance Goals**: No measurable regression in Operator/Knowledge/Functions/BananaAI page render paths
**Constraints**: Preserve existing API contracts except new telemetry route; avoid native/runtime model changes
**Scale/Scope**: 1 new controller endpoint, shared type/utility exports, 4 React page integrations

## Constitution Check

*GATE: Must pass before implementation. Re-check after design is finalized.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Domain Core First | PASS | No native domain rule duplication; scope is API contract + UI typing.
| II. Layered Interop Contract | PASS | ASP.NET addition remains controller-level read endpoint; existing flow unchanged.
| III. Spec First Delivery | PASS | Spec exists before implementation planning.
| IV. Verifiable Quality Gates | PASS | Endpoint, shared exports, and page behavior each map to independent validation.
| V. Documentation and Wiki Parity | PASS | No runtime contract behavior shift requiring wiki/runbook expansion in this slice.
| VI. One-Window Interactive Validation | PASS | Page changes are verifiable through existing compose + integrated browser workflow.

Post-design check: PASS. No constitutional violations introduced by this design.

## Architecture Decisions

1. Keep shared domain contracts in `@banana/ui` as the single source for DS-facing React pages.
2. Add telemetry config as a dedicated ASP.NET controller route (`GET /operator/telemetry/config`) with static response shape for this spike.
3. Move embedding drift math to a shared export (`computeCosineDrift`) to eliminate page-local duplicate logic.
4. Keep promotion audit tracking UI-local in Functions page state for this feature; no backend schema changes.
5. Use additive typing only (`optional` metric fields, exported type aliases) to avoid breaking existing consumers.

## Implementation Sequence (Dependency Ordered)

1. Shared `@banana/ui` contract baseline (types + exports)
  - Extend metrics and add exported types/utilities/components used by downstream pages.
2. Spike 166 backend contract
  - Add ASP.NET telemetry config endpoint and align frontend API call target.
3. Spike 167 + 169 KnowledgePage integration
  - Switch to shared `TrainingMetricCard` and shared drift utility/type.
4. Spike 168 BananaAI ripeness typing
  - Update `predictRipeness` call signature and consume exported `RipenessResult`.
5. Spike 170 Functions promotion audit UI
  - Append and render `PromotionAuditEntry` after promotion success.
6. Validation sweep
  - Run scoped API + shared-ui + React checks and confirm acceptance scenarios.

## Per-Area Design Notes

### Spike 166 - Telemetry Config Endpoint

- API file: `src/c-sharp/asp.net/Controllers/TelemetryController.cs` (new)
- Route: `GET /operator/telemetry/config`
- Response shape:

```json
{
  "sample_rate": 100,
  "unit": "percent"
}
```

- Frontend usage: `src/typescript/react/src/pages/OperatorPage.tsx`
- Behavior: fetch on load, render telemetry card, show safe error state on failure.

### Spike 167 - Training Metric Card Export

- Shared type update: `src/typescript/shared/ui/src/types.ts`
- Type shape update on `TrainingRunResult.metrics`:

```ts
{
  accuracy?: number;
  f1?: number;
  centroid_embedding?: number[];
}
```

- New shared component: `src/typescript/shared/ui/src/components/TrainingMetricCard.tsx`
- Shared exports: `src/typescript/shared/ui/src/index.ts`
- Consumer: `src/typescript/react/src/pages/KnowledgePage.tsx`
- Rendering rule: gracefully handle undefined `accuracy`/`f1` as zero-value display.

### Spike 168 - RipenessResult Type Export

- Shared type export: `src/typescript/shared/ui/src/types.ts`

```ts
type RipenessResult = {
  label: Ripeness;
  confidence: number;
};
```

- API helper update: `src/typescript/react/src/lib/api.ts`
- `predictRipeness` signature target: accepts `{ sample: string }` directly.
- Consumer update: `src/typescript/react/src/pages/BananaAIPage.tsx` imports and uses `RipenessResult` from `@banana/ui`.

### Spike 169 - Embedding Drift Utility

- Shared utility export: `computeCosineDrift(a: number[], b: number[]): number`
- Shared export surface: `src/typescript/shared/ui/src/index.ts`
- Shared type export:

```ts
type EmbeddingDriftSummary = {
  lane: TrainingLane;
  drift: number | null;
};
```

- Consumer refactor: `src/typescript/react/src/pages/KnowledgePage.tsx` replaces inline drift implementation with shared utility.
- Behavior rule: null drift when fewer than two embeddings exist for a lane.

### Spike 170 - Promotion Audit Entry Type

- Shared type export: `src/typescript/shared/ui/src/types.ts`

```ts
type PromotionAuditEntry = {
  run_id: string;
  promoted_at: string;
  lane: TrainingLane;
};
```

- Consumer integration: `src/typescript/react/src/pages/FunctionsPage.tsx`
- Behavior: after `promoteTrainingWorkbenchRun` resolves, append a new `PromotionAuditEntry` to local state and render audit list.

## Validation Approach

1. Spike 166 API contract
  - Add/execute ASP.NET endpoint test or integration assertion for `GET /operator/telemetry/config` status 200 and exact shape.
  - Manual operator UI check: telemetry card shows `100 percent`; failure path renders non-crashing error state.
2. Spike 167 shared component/types
  - Type-check `@banana/ui` and React app to verify new metric fields are accepted.
  - Page-level check: KnowledgePage renders metric cards for lanes; undefined metric values do not throw.
3. Spike 168 ripeness typing
  - Type-check BananaAI page imports of `RipenessResult`.
  - Behavioral check: ripeness request payload is `{ sample }` (no JSON string wrapping) and response fields bind cleanly.
4. Spike 169 drift utility
  - Unit tests for `computeCosineDrift` equal vectors -> `0`, orthogonal vectors -> `1`, and defensive handling of invalid vectors.
  - Page-level check: drift sidebar uses shared utility and renders null placeholder when insufficient runs.
5. Spike 170 promotion governance
  - Functions page behavior test: successful promotion appends visible audit entry with `run_id`, `promoted_at`, `lane`.

## Risk Notes

1. Shared export drift risk: changing `@banana/ui` exports can break existing imports; mitigate with additive exports and full TS type-check.
2. Metric optionality risk: missing metrics in history payload can cause display regressions; mitigate with explicit fallback rendering.
3. Cosine drift numeric edge cases: zero vectors or mismatched vector lengths can produce NaN; mitigate with guarded utility implementation and tests.
4. API/frontend contract mismatch: telemetry response casing must match UI expectation (`sample_rate`, `unit`); mitigate with endpoint test + fetch integration check.
5. Promotion audit UX consistency: local-only audit entries reset on reload; acceptable for this spike and documented as non-persistent behavior.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/171-ds-model-ops-complexity-scale-up/
├── plan.md
└── spec.md
```

### Source Code Impact

```text
src/c-sharp/asp.net/Controllers/
└── TelemetryController.cs (new)

src/typescript/shared/ui/src/
├── index.ts
├── types.ts
└── components/
   └── TrainingMetricCard.tsx (new)

src/typescript/react/src/
├── lib/api.ts
└── pages/
   ├── OperatorPage.tsx
   ├── KnowledgePage.tsx
   ├── BananaAIPage.tsx
   └── FunctionsPage.tsx
```

**Structure Decision**: Keep all DS-facing contracts centralized in `@banana/ui`, with one minimal ASP.NET route addition and page-level React adoption.

## Complexity Tracking

No constitution violations requiring exception handling.

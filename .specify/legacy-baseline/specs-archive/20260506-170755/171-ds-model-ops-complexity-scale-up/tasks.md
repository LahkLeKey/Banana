# Tasks: DS Model Ops Complexity Scale-Up (171)

**Input**: Design documents from `.specify/specs/171-ds-model-ops-complexity-scale-up/`
**Prerequisites**: `plan.md`, `spec.md`

## Phase 1: Foundational Shared Contracts (Groups A, B, C)

**Purpose**: Establish shared UI types/utilities/component and API endpoint contracts required by downstream React wiring.

- [x] T001 Extend `TrainingRunResult.metrics` with optional `accuracy`, `f1`, and `centroid_embedding` fields in `src/typescript/shared/ui/src/types.ts`.
  Files: `src/typescript/shared/ui/src/types.ts`
  Acceptance: TypeScript accepts `metrics.accuracy`, `metrics.f1`, and `metrics.centroid_embedding` as optional fields on training run results without breaking existing consumers.
  Depends on: None.

- [x] T002 Export `RipenessResult = { label: Ripeness, confidence: number }` from `src/typescript/shared/ui/src/types.ts`.
  Files: `src/typescript/shared/ui/src/types.ts`
  Acceptance: `RipenessResult` is exported and importable from shared UI type exports with no compile errors.
  Depends on: None.

- [x] T003 Export `PromotionAuditEntry = { run_id: string, promoted_at: string, lane: TrainingLane }` from `src/typescript/shared/ui/src/types.ts`.
  Files: `src/typescript/shared/ui/src/types.ts`
  Acceptance: `PromotionAuditEntry` is exported and available to React pages through shared UI type imports.
  Depends on: None.

- [x] T004 Export `EmbeddingDriftSummary = { lane: TrainingLane, drift: number | null }` from `src/typescript/shared/ui/src/types.ts`.
  Files: `src/typescript/shared/ui/src/types.ts`
  Acceptance: `EmbeddingDriftSummary` is exported with `drift` nullable to represent lanes with insufficient embedding history.
  Depends on: None.

- [x] T005 [P] Implement and export `computeCosineDrift(a: number[], b: number[]): number` in `src/typescript/shared/ui/src/index.ts`.
  Files: `src/typescript/shared/ui/src/index.ts`
  Acceptance: The utility is exported from `@banana/ui` and handles equal vectors and invalid/zero-length vector inputs without throwing.
  Depends on: None.

- [x] T006 Create `TrainingMetricCard` in `src/typescript/shared/ui/src/components/TrainingMetricCard.tsx` and export it from `src/typescript/shared/ui/src/index.ts`.
  Files: `src/typescript/shared/ui/src/components/TrainingMetricCard.tsx`, `src/typescript/shared/ui/src/index.ts`
  Acceptance: `TrainingMetricCard` is importable from `@banana/ui` and renders metric values with safe fallback behavior for missing optional metrics.
  Depends on: T001.

- [x] T007 [P] Create `TelemetryController.cs` with `GET /operator/telemetry/config` returning `{ sample_rate: 100, unit: "percent" }`, and register controller routing in `Program.cs`.
  Files: `src/c-sharp/asp.net/Controllers/TelemetryController.cs`, `src/c-sharp/asp.net/Program.cs`
  Acceptance: Calling `GET /operator/telemetry/config` returns status 200 and the exact response shape with `sample_rate` and `unit`.
  Depends on: None.

**Checkpoint**: Shared contracts and API endpoint are complete; Group D React integrations can begin.

---

## Phase 2: User Story 3 - Ripeness Signal Typed Response (Priority: P2)

**Goal**: Replace stringly ripeness typing with shared `RipenessResult` contract.

**Independent Test**: BananaAI flow compiles and displays `label` and `confidence` from typed ripeness response without JSON double-encoding.

- [x] T008 [US3] Update `predictRipeness` to accept `{ sample: string }` and return typed `RipenessResult`; update BananaAIPage to use `RipenessResult` from `@banana/ui`.
  Files: `src/typescript/react/src/lib/api.ts`, `src/typescript/react/src/pages/BananaAIPage.tsx`
  Acceptance: Ripeness requests send `{ sample }` payload directly, and BananaAIPage uses shared typed fields (`label`, `confidence`) without casts.
  Depends on: T001, T002, T003, T004, T005, T006, T007.

---

## Phase 3: User Story 2 - Training Metric Cards (Priority: P1)

**Goal**: Render shared training metric cards in KnowledgePage using shared optional metrics contract.

**Independent Test**: KnowledgePage uses `TrainingMetricCard` from `@banana/ui` and renders lanes even when metrics are missing.

- [x] T009 [US2] Wire `TrainingMetricCard` from `@banana/ui` into KnowledgePage by replacing or supplementing inline `LaneMetricsCard`.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`
  Acceptance: KnowledgePage metric section is backed by shared `TrainingMetricCard` and shows safe fallback values when `accuracy` or `f1` are absent.
  Depends on: T001, T002, T003, T004, T005, T006, T007.

---

## Phase 4: User Story 4 - Embedding Drift Sidebar (Priority: P2)

**Goal**: Compute and display lane drift with shared cosine drift utility.

**Independent Test**: KnowledgePage drift sidebar uses `computeCosineDrift` and renders `—` for lanes with fewer than two embeddings.

- [x] T010 [US4] Replace inline `cosineDrift` in KnowledgePage with `computeCosineDrift` from `@banana/ui` for drift sidebar rendering.
  Files: `src/typescript/react/src/pages/KnowledgePage.tsx`
  Acceptance: Drift values come from shared utility, equal vectors evaluate to `0`, and insufficient data produces nullable drift display without runtime errors.
  Depends on: T001, T002, T003, T004, T005, T006, T007.

---

## Phase 5: User Story 5 - Promotion Governance Audit (Priority: P3)

**Goal**: Surface promotion audit entries in FunctionsPage after successful promotion actions.

**Independent Test**: Triggering promotion appends a visible audit entry with run id, timestamp, and lane.

- [x] T011 [P] [US5] Add `PromotionAuditEntry` state in FunctionsPage, append an entry after `promoteTrainingWorkbenchRun` resolves, and render audit list below the promote button.
  Files: `src/typescript/react/src/pages/FunctionsPage.tsx`
  Acceptance: After successful promotion, a new audit row appears showing `run_id`, `promoted_at`, and `lane`; initial page load shows an empty audit list safely.
  Depends on: T001, T002, T003, T004, T005, T006, T007.

---

## Phase 6: User Story 1 - Telemetry Rate Visible In Operator UI (Priority: P1)

**Goal**: Expose live telemetry sampling config in Operator UI.

**Independent Test**: OperatorPage displays telemetry `sample_rate` and `unit` from API response; fetch failure renders an inline non-crashing error state.

- [x] T012 [P] [US1] Add telemetry config card to OperatorPage by fetching `GET /operator/telemetry/config` on mount and rendering `sample_rate`.
  Files: `src/typescript/react/src/pages/OperatorPage.tsx`
  Acceptance: OperatorPage fetches telemetry config on mount and displays `sample_rate` with `unit`; failed fetch path shows an error card/state without crashing the page.
  Depends on: T001, T002, T003, T004, T005, T006, T007.

---

## Dependencies & Execution Order

### Group/Phase Dependency Chain

- Group A (`T001`-`T005`) has no dependencies and defines shared type/utility baselines.
- Group B (`T006`) depends on `T001`.
- Group C (`T007`) is independent of Groups A/B.
- Group D (`T008`-`T012`) depends on completion of `T001` through `T007`.

### Story Dependency Summary

- `US3` (`T008`) depends on foundational shared/API baseline.
- `US2` (`T009`) depends on foundational shared/API baseline.
- `US4` (`T010`) depends on foundational shared/API baseline.
- `US5` (`T011`) depends on foundational shared/API baseline.
- `US1` (`T012`) depends on foundational shared/API baseline.

### Parallel Opportunities

- `T005` and `T007` can run in parallel with other foundational tasks because they touch independent files.
- After `T001`-`T007` complete, `T008`, `T011`, and `T012` can run in parallel.
- `T009` and `T010` should run sequentially because both update `src/typescript/react/src/pages/KnowledgePage.tsx`.

## Implementation Strategy

### MVP Slice

1. Complete foundational baseline (`T001`-`T007`).
2. Deliver telemetry UI end-to-end (`T012`) for a complete P1 vertical slice (API + UI).
3. Validate `GET /operator/telemetry/config` and OperatorPage card behavior before moving on.

### Incremental Completion

1. Deliver shared contract consumers: `T008`, `T009`, `T010`, `T011`.
2. Validate each user story independently using the independent test criteria in each phase.
3. Run final type-check and page smoke verification across BananaAI, Knowledge, Functions, and Operator surfaces.

## Notes

- Task IDs are fixed to the requested spike mapping (`T001`-`T012`).
- Acceptance criteria are intentionally brief and implementation-executable.
- Dependencies are explicit per task to support agent handoff and incremental PR slicing.

---
description: "Tasks for v2 ASP.NET pipeline regeneration"
---

# Tasks: ASP.NET Pipeline (v2)

## Phase 0 — Inventory

- [ ] T001 Snapshot all HTTP routes + response shapes into `research.md`.
- [ ] T002 List all `*Native` shadow DTOs and their record counterparts.
- [ ] T003 List all pipeline-context string keys and their producers/consumers.

## Phase 1 — Typed pipeline context

- [ ] T010 Define typed `PipelineContext` with explicit properties.
- [ ] T011 [P] Migrate steps in dependency order; remove string keys per migrated step.
- [ ] T012 [P] Update unit tests to use typed context.

## Phase 2 — DTO collapse

- [ ] T020 Replace `*Native` types with mappers (generated or single hand-written).
- [ ] T021 [P] Update `NativeBananaClient` to use unified DTOs.
- [ ] T022 [P] Update tests/fakes accordingly.

## Phase 3 — Interop seam slimming

- [ ] T030 Introduce default abstract `NativeBananaClientBase` or codegen for fakes.
- [ ] T031 [P] Migrate fakes under `tests/{unit,integration}` to the new pattern.
- [ ] T032 Confirm adding a method touches ≤2 test files.

## Phase 4 — Data access decision

- [ ] T040 Decide: fold `DataAccess/` into `NativeInterop/` or formalize role.
- [ ] T041 Update DI wiring + docs in plan.

## Phase 5 — Native cutover

- [ ] T050 Switch P/Invoke target to v2 wrapper (`006`).
- [ ] T051 Run unit + integration + e2e suites.
- [ ] T052 Remove v1 native fallback paths.

## Phase 6 — Training-data and model metadata alignment

- [ ] T060 Define managed banana/not-banana training-data contract expectations and mapping from artifact metadata.
- [ ] T061 [P] Expose model metadata + threshold provenance in `007` banana/not-banana API responses.
- [ ] T062 [P] Add typed fallback behavior for missing/corrupt artifacts and no-signal classification input.

## Phase 7 — Chatbot interoperability

- [ ] T070 Define banana-vs-not-banana chatbot interoperability contract with `008` (session/message + classification semantics).
- [ ] T071 [P] Implement replay/idempotency-compatible classification behavior for fallback/cutover scenarios.
- [ ] T072 [P] Add integration tests for compatibility between `007` and `008` banana/not-banana outputs.

## Phase 8 — Drift and parity guards

- [ ] T080 Add CI checks for corpus/training-script/native-runtime/managed-contract drift.
- [ ] T081 Wire parity failures into release gate reporting.
- [ ] T082 Document remediation workflow for training/artifact/chat parity regressions.

## Dependencies

- T010 blocks Phase 2/3 step migrations.
- T030 should land before bulk fake updates in T031.
- T050 requires `006` Phase 4 cutover.
- T060 blocks parity assertions in T072 and drift gates in T080.
- T070 blocks T071 and chatbot fallback validation.

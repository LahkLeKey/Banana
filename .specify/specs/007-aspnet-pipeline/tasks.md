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

## Dependencies

- T010 blocks Phase 2/3 step migrations.
- T030 should land before bulk fake updates in T031.
- T050 requires `006` Phase 4 cutover.

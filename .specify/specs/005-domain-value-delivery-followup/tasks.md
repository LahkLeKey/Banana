---
description: "Task list for Domain Value Delivery Follow-Up (Phase 2)"
---

# Tasks: Domain Value Delivery Follow-Up (Phase 2)

**Input**: Design documents from `.specify/specs/005-domain-value-delivery-followup/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/api-contracts.md
**Branch**: `005-domain-value-delivery-followup`
**Date**: 2026-04-26

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Parallelizable task
- **[US#]**: User story label tied to spec.md
- Every task includes an explicit file path

## Phase 1: Setup

- [ ] T001 Confirm active feature pointer in `.specify/feature.json` targets `.specify/specs/005-domain-value-delivery-followup` (supports FR-005 traceability)
- [ ] T002 [P] Re-run planning scaffolding check with `.specify/scripts/bash/setup-plan.sh --json` and record output in `.specify/specs/005-domain-value-delivery-followup/plan.md` (supports FR-005 reproducibility)
- [ ] T003 [P] Verify baseline gates by running `dotnet test tests/unit`, `dotnet test tests/e2e`, and `bun run --cwd src/typescript/api test` before implementation changes (supports FR-006 regression safety)

---

## Phase 2: Foundational (Blocking)

- [X] T004 Introduce `NativeStatusCode.NativeUnavailable` in `src/c-sharp/asp.net/NativeInterop/NativeStatusCode.cs` (implements FR-001)
- [X] T005 Update interop exception mapping in `src/c-sharp/asp.net/NativeInterop/NativeBananaClient.cs` so load/symbol failures return `NativeStatusCode.NativeUnavailable` (implements FR-001)
- [X] T006 Update 503 status payload mapping in `src/c-sharp/asp.net/Pipeline/StatusMapping.cs` to emit `{ error: "native_unavailable", remediation: "Set BANANA_NATIVE_PATH to a valid native library path." }` (implements FR-001)
- [X] T007 [P] Add typed mapping contracts in `src/c-sharp/asp.net/Pipeline/Results/BinaryClassificationResult.cs` and `src/c-sharp/asp.net/Pipeline/Results/NotBananaClassificationResult.cs` (implements FR-002)
- [X] T008 [P] Add shared mapper abstraction in `src/c-sharp/asp.net/Pipeline/Mapping/INativeJsonMapper.cs` and implementation in `src/c-sharp/asp.net/Pipeline/Mapping/NativeJsonMapper.cs` (implements FR-002)
- [X] T009 Register mapper DI in `src/c-sharp/asp.net/Program.cs` for controller/service consumption (implements FR-002)

**Checkpoint**: Native-unavailable contract and reusable JSON mapping primitives are in place.

---

## Phase 3: User Story 1 - Native Runtime Failure Contract Is Accurate (Priority: P1)

**Goal**: native runtime load failures return deterministic 503 + `native_unavailable` payload.

**Independent Test**: invalid `BANANA_NATIVE_PATH` -> `POST /ripeness/predict` returns 503 with native remediation.

- [X] T010 [US1] Add/adjust native-unavailable status mapping unit tests in `tests/unit/StatusMappingTests.cs`
- [X] T011 [US1] Add interop-unavailable mapping tests in `tests/unit/NativeBananaClientTests.cs`
- [X] T012 [US1] Add API contract assertion for native unavailable payload in `tests/e2e/Contracts/E2eRunnerContractTests.cs`
- [X] T013 [US1] Update ripeness/runtime contract assertions in `tests/unit/RipenessControllerTests.cs` to expect `native_unavailable`
- [X] T014 [US1] Add remediation wording note to `src/c-sharp/asp.net/README.md` (or nearest API runtime doc if README absent)
- [X] T015 [US1] Validate US1 with `dotnet test tests/unit --filter "FullyQualifiedName~StatusMapping|FullyQualifiedName~NativeBananaClient|FullyQualifiedName~RipenessController"`

---

## Phase 4: User Story 2 - API Contract Ownership Is Centralized (Priority: P2)

**Goal**: ML and not-banana payload shaping is performed by pipeline/service mapping abstractions, not ad-hoc controller JSON logic.

**Independent Test**: controller tests prove HTTP orchestration only; mapping tests validate payload shape.

- [X] T016 [P] [US2] Add mapping service tests in `tests/unit/NativeJsonMapperTests.cs` for valid/invalid JSON payloads
- [X] T017 [US2] Refactor `src/c-sharp/asp.net/Controllers/BananaMlController.cs` to delegate JSON mapping to mapper abstraction
- [X] T018 [US2] Refactor `src/c-sharp/asp.net/Controllers/NotBananaController.cs` to delegate JSON mapping to mapper abstraction
- [X] T019 [P] [US2] Add/update typed result models used by ML/not-banana responses in `src/c-sharp/asp.net/Pipeline/Results/`
- [X] T020 [US2] Update controller tests in `tests/unit/BananaMlControllerTests.cs` and `tests/unit/NotBananaControllerTests.cs` to assert no ad-hoc JSON shaping behavior
- [X] T021 [US2] Run `dotnet test tests/unit --filter "FullyQualifiedName~BananaMlController|FullyQualifiedName~NotBananaController|FullyQualifiedName~NativeJsonMapper"`

---

## Phase 5: User Story 3 - TypeScript API Exposes Ripeness Endpoint (Priority: P2)

**Goal**: Fastify exposes `POST /ripeness/predict` with typed validation and response parity.

**Independent Test**: route test covers success + validation failure + upstream pass-through failure.

- [X] T022 [P] [US3] Create ripeness route schema + handler in `src/typescript/api/src/routes/ripeness.ts`
- [X] T023 [US3] Register ripeness route in `src/typescript/api/src/index.ts`
- [X] T024 [P] [US3] Add route tests for success/validation/upstream-error in `src/typescript/api/src/routes/ripeness.test.ts`
- [X] T025 [US3] Update any shared API route typing helpers if needed in `src/typescript/api/src/routes/` to support ripeness schema reuse
- [X] T026 [US3] Run `bun run --cwd src/typescript/api test src/routes/ripeness.test.ts`

---

## Phase 6: User Story 4 - Harvest/Truck HTTP Surfaces Are Reachable (Priority: P3)

**Goal**: harvest and truck operations exposed by `INativeBananaClient` are reachable through ASP.NET HTTP endpoints with consistent status mapping.

**Independent Test**: per operation family, one success and one failure-path assertion exists.

- [X] T027 [P] [US4] Add harvest endpoints in `src/c-sharp/asp.net/Controllers/HarvestController.cs` for create/add-bunch/status
- [X] T028 [P] [US4] Add truck endpoints in `src/c-sharp/asp.net/Controllers/TruckController.cs` for register/load/unload/relocate/status
- [X] T029 [US4] Add harvest endpoint tests in `tests/unit/HarvestControllerTests.cs`
- [X] T030 [US4] Add truck endpoint tests in `tests/unit/TruckControllerTests.cs`
- [X] T031 [US4] Ensure endpoint registration/discovery works in `src/c-sharp/asp.net/Program.cs` and update any route docs near controllers
- [X] T032 [US4] Run `dotnet test tests/unit --filter "FullyQualifiedName~HarvestController|FullyQualifiedName~TruckController"`

---

## Phase 7: Integration Lane Codification + Polish

- [X] T033 Codify `tests/e2e` as integration-equivalent contract lane in `tests/README.md` (implements FR-005)
- [X] T034 [P] Add explicit contract lane test file `tests/e2e/Contracts/ApiContractLaneTests.cs` for ripeness + harvest + truck endpoint checks (implements FR-005, FR-006)
- [X] T035 Update run instructions in `.specify/specs/005-domain-value-delivery-followup/quickstart.md` with exact validation commands used in CI/local (implements FR-005)
- [X] T036 [P] Update `.specify/specs/005-domain-value-delivery-followup/contracts/api-contracts.md` if implementation altered payload/route details (implements FR-006)
- [X] T037 Run full validation: `dotnet test tests/unit && dotnet test tests/e2e && bun run --cwd src/typescript/api test` (implements FR-006)
- [X] T038 Capture completion evidence and mark task statuses in `.specify/specs/005-domain-value-delivery-followup/tasks.md` (supports FR-005 auditability)

### Phase 7 Completion Evidence (2026-04-26)

- `dotnet test tests/unit` -> Passed (`30` total, `0` failed)
- `dotnet test tests/e2e` -> Passed (`10` total, `0` failed)
- `bun run --cwd src/typescript/api test` -> Passed (`16` total, `0` failed)

---

## Dependencies & Execution Order

- Phase 1 -> Phase 2 -> US1/US2/US3/US4 -> Phase 7.
- US1 depends on Phase 2.
- US2 depends on Phase 2 and can run parallel with US3.
- US4 depends on Phase 2 and can run parallel with US2/US3.
- Phase 7 runs after all user stories complete.

## Parallel Opportunities

- T002/T003 in Setup.
- T007/T008 in Foundational.
- T016 + T019 in US2.
- T022 + T024 in US3.
- T027 + T028 in US4.
- T034 + T036 in Phase 7.

## MVP Suggestion

- MVP = US1 + US2 + US3 with T037 partial validation (`dotnet test tests/unit` + ripeness route test).
- Follow with US4 and Phase 7 for full delivery closure.
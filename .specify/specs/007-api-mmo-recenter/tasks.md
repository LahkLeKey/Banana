# Tasks: API MMO Recenter

**Input**: Design documents from `.specify/specs/007-api-mmo-recenter/`

**Prerequisites**: `plan.md` (required), `spec.md` (required), `research.md`, `data-model.md`, `contracts/`

**Tests**: Validation and integration evidence is required for each user story based on spec success criteria SC-001..SC-006.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (`[US1]`, `[US2]`, `[US3]`)
- Include exact file paths in descriptions

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Establish bounded-context scaffolding, route namespaces, and baseline task/test wiring for this feature.

- [X] T001 Create bounded-context directory scaffolding under `src/typescript/api/src/domains/gameplay-session-orchestration/`, `src/typescript/api/src/domains/player-identity-account/`, `src/typescript/api/src/domains/progression-inventory/`, and `src/typescript/api/src/domains/player-insights-web/` with placeholder index files.
- [X] T002 [P] Add v1 namespace route entrypoints `src/typescript/api/src/routes/v1/gameplay.ts` and `src/typescript/api/src/routes/v1/player.ts` with TODO stubs and typed Fastify plugin exports.
- [X] T003 [P] Register v1 gameplay/player route modules in `src/typescript/api/src/index.ts` while preserving existing route compatibility.
- [X] T004 [P] Add feature scope notes and execution commands in `src/typescript/api/README.md` for `/v1/gameplay/*` and `/v1/player/*`.
- [X] T005 Capture initial ownership matrix and route-to-domain mapping sheet in `.specify/specs/007-api-mmo-recenter/checklists/ownership-matrix.md`.

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Implement shared contracts and runtime enforcement that all user stories depend on.

**CRITICAL**: Complete this phase before starting user-story implementation.

- [X] T006 Implement shared domain DTOs and zod schemas for v1 contracts in `src/typescript/api/src/lib/contracts/v1/gameplay.ts`, `src/typescript/api/src/lib/contracts/v1/player.ts`, and `src/typescript/api/src/lib/contracts/v1/error.ts`.
- [X] T007 [P] Implement bounded-context ownership guard helpers in `src/typescript/api/src/lib/domainOwnership.ts` and apply route-owner assertions in `src/typescript/api/src/index.ts`.
- [X] T008 [P] Add correlation-id, actor-scope, and idempotency extraction middleware in `src/typescript/api/src/middleware/requestContext.ts` and register in `src/typescript/api/src/index.ts`.
- [X] T009 Enforce Neon source-of-record alias synchronization and strict bootstrap failure rules in `src/typescript/api/src/services/databaseRuntime.ts` for `NEON_DATABASE_URL`, `DATABASE_URL`, and `BANANA_PG_CONNECTION`.
- [X] T010 [P] Add exactly-once mutation settlement interfaces and persistence adapters in `src/typescript/api/src/domains/progression-inventory/settlement/idempotencyStore.ts` and `src/typescript/api/src/domains/progression-inventory/settlement/ledgerSettlementService.ts`.
- [X] T011 Create SQL DDL for idempotency and mutation-ledger uniqueness constraints in `src/typescript/api/src/domains/progression-inventory/persistence/sql/001_idempotency_ledger_constraints.sql`.
- [X] T012 [P] Add domain error taxonomy and response translators in `src/typescript/api/src/lib/errors/domainErrors.ts` and `src/typescript/api/src/lib/errors/fastifyErrorMapper.ts` for `400/401/403/409/503` semantics.
- [X] T013 Implement API contract inventory + drift validator script in `scripts/validate-api-parity-governance.sh` with route-to-owner and contract version checks.
- [X] T014 [P] Add foundational contract and runtime tests in `src/typescript/api/src/lib/contracts/v1/contracts.test.ts` and `src/typescript/api/src/services/databaseRuntime.source-of-record.test.ts`.

**Checkpoint**: Foundation ready; user stories can be implemented independently.

---

## Phase 3: User Story 1 - Realtime Session Orchestration for Connected Players (Priority: P1) 🎯 MVP

**Goal**: Deliver authoritative DX12 gameplay orchestration APIs for session admission, command settlement, rejoin continuity, and termination.

**Independent Test**: Run concurrent multi-client session tests where join, command, rejoin, and termination actions converge on one consistent authoritative state with no duplicate accepted mutations.

### Tests for User Story 1

- [X] T015 [P] [US1] Add gameplay admissions/commands contract tests in `src/typescript/api/src/routes/v1/gameplay.contract.test.ts` against `/v1/gameplay/sessions/admissions` and `/v1/gameplay/sessions/{sessionId}/commands`.
- [X] T016 [P] [US1] Add gameplay rejoin/terminate contract tests in `src/typescript/api/src/routes/v1/gameplay.lifecycle.contract.test.ts`.
- [X] T017 [US1] Add concurrent gameplay orchestration integration test (SC-001) in `src/typescript/api/src/integration/gameplay-session-consistency.integration.test.ts` with measurable 2s reflection assertions.
- [X] T018 [US1] Add failure-injection idempotency integration test (SC-004) in `src/typescript/api/src/integration/gameplay-idempotency-failure.integration.test.ts`.

### Implementation for User Story 1

- [X] T019 [P] [US1] Implement gameplay admission service in `src/typescript/api/src/domains/gameplay-session-orchestration/services/admissionService.ts` with actor-scope validation.
- [X] T020 [P] [US1] Implement gameplay command orchestration service in `src/typescript/api/src/domains/gameplay-session-orchestration/services/commandService.ts` with authoritative tick resolution.
- [X] T021 [P] [US1] Implement gameplay rejoin continuity service in `src/typescript/api/src/domains/gameplay-session-orchestration/services/rejoinService.ts`.
- [X] T022 [P] [US1] Implement gameplay termination service in `src/typescript/api/src/domains/gameplay-session-orchestration/services/terminationService.ts`.
- [X] T023 [US1] Wire exactly-once settlement into gameplay mutating paths in `src/typescript/api/src/domains/gameplay-session-orchestration/pipelines/settlementPipeline.ts`.
- [X] T024 [US1] Implement gameplay v1 routes in `src/typescript/api/src/routes/v1/gameplay.ts` and bind them to domain services.
- [X] T025 [US1] Persist gameplay lifecycle change records in `src/typescript/api/src/domains/gameplay-session-orchestration/persistence/domainChangeRecorder.ts`.
- [X] T026 [US1] Record US1 evidence outputs for SC-001 and SC-004 in `artifacts/api/007-us1-session-orchestration.txt`.

**Checkpoint**: US1 is independently shippable as MVP.

---

## Phase 4: User Story 2 - Unified Player Account and Progression Across Client and Web (Priority: P2)

**Goal**: Deliver website player account/progression/inventory APIs with one authoritative player truth model shared with gameplay.

**Independent Test**: Update progression/inventory from gameplay and account from web API, then verify both channels resolve to matching persisted authoritative records.

### Tests for User Story 2

- [X] T027 [P] [US2] Add account view/update contract tests in `src/typescript/api/src/routes/v1/player.account.contract.test.ts`.
- [X] T028 [P] [US2] Add progression/inventory contract tests in `src/typescript/api/src/routes/v1/player.progression-inventory.contract.test.ts`.
- [X] T029 [US2] Add cross-channel parity integration test (SC-002) in `src/typescript/api/src/integration/player-cross-channel-parity.integration.test.ts`.

### Implementation for User Story 2

- [X] T030 [P] [US2] Implement player identity/account services in `src/typescript/api/src/domains/player-identity-account/services/accountQueryService.ts` and `src/typescript/api/src/domains/player-identity-account/services/accountMutationService.ts`.
- [X] T031 [P] [US2] Implement progression/inventory query services in `src/typescript/api/src/domains/progression-inventory/services/progressionQueryService.ts` and `src/typescript/api/src/domains/progression-inventory/services/inventoryQueryService.ts`.
- [X] T032 [US2] Implement account optimistic-version and deterministic conflict handling in `src/typescript/api/src/domains/player-identity-account/pipelines/accountConflictPipeline.ts`.
- [X] T033 [US2] Implement player account/progression/inventory v1 routes in `src/typescript/api/src/routes/v1/player.ts`.
- [X] T034 [US2] Enforce self-scope authorization for player APIs in `src/typescript/api/src/middleware/playerScopeAuthorization.ts` and apply to `src/typescript/api/src/routes/v1/player.ts`.
- [X] T035 [US2] Write unified truth-model reconciliation checks in `src/typescript/api/src/domains/progression-inventory/validation/playerTruthModelValidator.ts`.
- [X] T036 [US2] Record US2 evidence outputs for SC-002 in `artifacts/api/007-us2-cross-channel-consistency.txt`.

**Checkpoint**: US1 and US2 are independently functional with shared authoritative truth.

---

## Phase 5: User Story 3 - Website Player Insights and Self-Service Visibility (Priority: P3)

**Goal**: Deliver player insights APIs with no-data semantics, freshness metadata, and derived read models from authoritative records.

**Independent Test**: Validate insights for active and new players, including no-data responses and freshness metadata backed by persisted records.

### Tests for User Story 3

- [X] T037 [P] [US3] Add insights endpoint contract tests in `src/typescript/api/src/routes/v1/player.insights.contract.test.ts`.
- [X] T038 [US3] Add player insights usability/no-data integration tests (SC-003) in `src/typescript/api/src/integration/player-insights-usability.integration.test.ts`.

### Implementation for User Story 3

- [X] T039 [P] [US3] Implement insight projection builder in `src/typescript/api/src/domains/player-insights-web/services/insightProjectionService.ts`.
- [X] T040 [P] [US3] Implement insight repository adapters over authoritative records in `src/typescript/api/src/domains/player-insights-web/persistence/insightReadRepository.ts`.
- [X] T041 [US3] Implement `/v1/player/insights` route behavior and no-data semantics in `src/typescript/api/src/routes/v1/player.ts`.
- [X] T042 [US3] Record US3 evidence outputs for SC-003 in `artifacts/api/007-us3-player-insights.txt`.

**Checkpoint**: All user stories are independently testable and aligned to bounded subdomains.

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Deployment readiness, drift reduction hardening, and release evidence for Fly + Neon operations.

- [X] T043 [P] Add Fly runtime readiness checks and release guard script updates in `scripts/deploy-api-fly.sh`.
- [X] T044 Add Fly + Neon runtime contract assertions in `src/typescript/api/fly.toml` and document env/readiness expectations in `src/typescript/api/README.md`.
- [X] T045 [P] Add CI gate for contract drift and source-of-record enforcement in `.github/workflows/banana.yml`.
- [X] T046 Add Neon connectivity/readiness and rollback evidence capture tasks in `.specify/specs/007-api-mmo-recenter/checklists/fly-neon-readiness.md`.
- [X] T047 [P] Execute and record strict contract drift validation output in `artifacts/api/007-versioned-contract-registration.txt`.
- [X] T048 Execute and record idempotency failure-path validation output in `artifacts/api/007-idempotency-failure-injection.txt`.
- [X] T049 Execute and record Fly health/connectivity/rollback readiness outputs in `artifacts/api/007-fly-health.txt`, `artifacts/api/007-neon-connectivity.txt`, and `artifacts/api/007-rollback-readiness.txt`.

---

## Dependencies & Execution Order

### Phase Dependencies

- **Phase 1 (Setup)**: No dependencies.
- **Phase 2 (Foundational)**: Depends on Phase 1; blocks all user stories.
- **Phase 3 (US1)**: Depends on Phase 2.
- **Phase 4 (US2)**: Depends on Phase 2; should run after US1 MVP sign-off for safer rollout.
- **Phase 5 (US3)**: Depends on Phase 2 and consumes US2 player APIs/read models.
- **Phase 6 (Polish)**: Depends on all targeted user stories complete.

### User Story Dependency Graph

- **US1 (P1)**: Start first after foundation; MVP slice.
- **US2 (P2)**: Starts after foundation; integrates with shared settlement and truth-model validation from US1/Phase 2.
- **US3 (P3)**: Starts after US2 route/read-model surfaces are available.

### Task-Level Ordering Notes

- Complete T006-T014 before T015+.
- Complete US1 tests (T015-T018) before US1 implementation tasks (T019-T026).
- Complete US2 tests (T027-T029) before US2 implementation tasks (T030-T036).
- Complete US3 tests (T037-T038) before US3 implementation tasks (T039-T042).
- Execute T043-T049 after implementation to produce release evidence.

---

## Parallel Execution Examples

### US1 Parallel Example

- Run T015 and T016 in parallel (distinct test files).
- Run T019, T020, T021, and T022 in parallel (separate service files).

### US2 Parallel Example

- Run T027 and T028 in parallel (distinct contract tests).
- Run T030 and T031 in parallel (separate bounded-context service files).

### US3 Parallel Example

- Run T039 and T040 in parallel (service vs persistence adapter files).

---

## Implementation Strategy

### MVP First (US1)

1. Finish Phase 1 and Phase 2 completely.
2. Deliver US1 (T015-T026) and validate SC-001 + SC-004.
3. Use US1 as MVP checkpoint before advancing broad website-facing surfaces.

### Incremental Delivery

1. Deliver US1 authoritative gameplay orchestration APIs.
2. Deliver US2 unified account/progression/inventory APIs and cross-channel parity.
3. Deliver US3 player insights read models.
4. Complete Fly/Neon readiness and drift-reduction gates in Phase 6.

### Definition of Ready for `/speckit.implement`

- All tasks reference concrete monorepo file paths.
- Bounded-context ownership and API versioning tasks are explicit.
- Validation evidence tasks map to SC-001 through SC-006.
- Fly + Neon + idempotency + drift-reduction gates are included and measurable.

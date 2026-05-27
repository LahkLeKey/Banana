# Tasks: Gameplay/API Persistence Continuity

**Input**: Design documents from `.specify/specs/016-gameplay-api-continuity/`

**Prerequisites**: `plan.md`, `spec.md`

**Tests**: Deterministic and failure-path tests are required for each user story.

## Phase 1: Setup (Shared Infrastructure)

Path targets:
- `src/native/engine/win32_dx12_poc/scene/`
- `src/typescript/api/src/`
- `tests/native/`
- `tests/integration/`
- `artifacts/native/016-gameplay-api-continuity/`
- `artifacts/api/016-gameplay-api-continuity/`

- [x] T001 Create feature evidence directories for native and API continuity artifacts
- [x] T002 Inventory runtime/API continuity contract touchpoints in API and native owning paths
- [x] T003 [P] Capture baseline coherent transition signatures and current API persistence behavior

---

## Phase 2: Foundational (Blocking Prerequisites)

- [x] T004 Define canonical continuity payload schema and ownership boundaries
- [x] T005 [P] Add payload validation helpers for route/variant/signature coherence
- [x] T006 [P] Add explicit reject-path diagnostics for invalid continuity payloads
- [x] T007 Add focused test harness entry points (native + API) for continuity suite

**Checkpoint**: Foundation ready

---

## Phase 3: User Story 1 - Cross-Slice Progress Persists (Priority: P1)

**Goal**: Connected transition round-trips preserve progression state.

- [x] T008 [P] [US1] Add deterministic round-trip continuity test (native)
- [x] T009 [P] [US1] Add integration persistence round-trip test (API/integration)
- [x] T010 [US1] Implement transition checkpoint persistence write path
- [x] T011 [US1] Implement transition checkpoint rehydration read path
- [x] T012 [US1] Capture round-trip runtime/API evidence

---

## Phase 4: User Story 2 - API/Runtime Contract Alignment (Priority: P1)

**Goal**: Runtime continuity payload and API schema stay aligned.

- [x] T013 [P] [US2] Add schema drift guard tests for runtime/API continuity payloads
- [x] T014 [P] [US2] Add failure-path tests for invalid payload diagnostics
- [x] T015 [US2] Wire canonical payload validation into API persistence ingress path
- [x] T016 [US2] Emit aligned contract diagnostics in native/API evidence logs

---

## Phase 5: User Story 3 - Persistence Growth Path (Priority: P2)

**Goal**: New persisted fields can be added via one owning contract path.

- [x] T017 [P] [US3] Add mutation safety test for one new persisted field
- [x] T018 [P] [US3] Add disabled/unsupported field diagnostics path test
- [x] T019 [US3] Implement one new persisted field through owning continuity contract path
- [x] T020 [US3] Document add-a-field workflow in API/native continuity diagnostics docs

---

## Phase 6: Polish and Cross-Cutting

- [x] T021 [P] Update `.specify/specs/execution-dashboard.md` with spec 016 status
- [x] T022 Update `.specify/specs/016-gameplay-api-continuity/plan.md` with final command set
- [x] T023 [P] Capture final focused continuity suite output in artifacts
- [x] T024 Run final focused validation path and record summary in both artifacts

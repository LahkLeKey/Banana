# Tasks: Banana Engineer

**Input**: Design documents from `.specify/specs/001-react-portal-game-client/`
**Prerequisites**: plan.md, spec.md

## Phase 1: Baseline and Scope Guardrails

- [ ] T001 [US3] Inventory active workflow/spec roots and identify out-of-scope entries for retirement or archive. (FR-007)
- [ ] T002 [US3] Define and document active-root guardrails to prevent reintroduction of non-pivot workflow/spec files. (FR-007)

## Phase 2: WASM Landing + 2.5D Shell Pivot

- [ ] T003 [US1] Implement WASM-backed landing route bootstrap in the React app entry flow. (FR-001, FR-004)
- [ ] T004 [US1] Build landing presentation with 2.5D scene conventions instead of flat web-page framing. (FR-002)
- [ ] T005 [US2] Rework shell/navigation to preserve 2.5D game-client continuity after landing handoff. (FR-002, FR-003)
- [ ] T006 [US2] Validate desktop and mobile-sized viewport behavior for landing and primary routes. (FR-003)

## Phase 3: Minimal Automation for Safe Shipping

- [ ] T007 [US3] Confirm React CI lane in `.github/workflows/banana.yml` validates WASM landing build/runtime contracts without workflow sprawl. (FR-006, FR-007)
- [ ] T008 [US3] Verify deployment path requirements for React portal releases stay explicit and functional with WASM assets. (FR-004)
- [ ] T009 [US3] Validate required env wiring (`VITE_BANANA_API_BASE_URL`) in build/runtime lanes. (FR-005)

## Phase 4: Verification and Evidence

- [ ] T010 [US1] Run targeted frontend validation (`bun run build` and landing boot checks) for WASM route startup. (FR-001, FR-004)
- [ ] T011 [US2] Run screenshot/runtime checks proving 2.5D continuity across viewport profiles. (FR-002, FR-003)
- [ ] T012 [US3] Run CI lane(s) proving minimal automation contract remains intact for this pivot. (FR-006, FR-007)
- [ ] T013 [US3] Record feature evidence and residual risk notes in this spec directory. (FR-007)

## Phase 5: Procedural Asset Compiler (C)

- [ ] T014 [US4] Define deterministic generation inputs (seed/profile/rules) for baseline Banana Engineer landing and shell asset sets. (FR-009)
- [ ] T015 [US4] Implement a C-based procedural asset compiler that emits web-consumable generated asset bundles for the React/WASM client. (FR-008, FR-010)
- [ ] T016 [US4] Integrate generated bundle ingestion into the React build/bootstrap path without requiring manual art authoring for baseline scenes. (FR-004, FR-010)
- [ ] T017 [US4] Add CI/local validation that re-runs compiler generation and verifies deterministic output + bundle compatibility with landing/shell runtime. (FR-006, FR-009, FR-010)

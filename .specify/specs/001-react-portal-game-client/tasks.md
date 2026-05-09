# Tasks: React Portal Game Client Pivot

**Input**: Design documents from `.specify/specs/001-react-portal-game-client/`
**Prerequisites**: plan.md, spec.md

## Phase 1: Baseline and Scope Guardrails

- [ ] T001 [US3] Inventory active workflow/spec roots and identify out-of-scope entries for retirement or archive. (FR-002, FR-005)
- [ ] T002 [US3] Define and document active-root guardrails to prevent reintroduction of non-pivot workflow/spec files. (FR-005)

## Phase 2: React Portal Game-Client Pivot

- [ ] T003 [US1] Rework top-level portal shell toward game-client framing while preserving route stability. (FR-001)
- [ ] T004 [US1] Update navigation and page copy to mission-control/game-client language where approved. (FR-001)
- [ ] T005 [US1] Validate responsive behavior on key routes after shell update. (FR-001)

## Phase 3: Minimal Automation for Safe Shipping

- [ ] T006 [US2] Confirm React CI lane in `.github/workflows/banana.yml` remains minimal and build-focused for portal shipping. (FR-003)
- [ ] T007 [US2] Verify deployment path requirements for React portal releases stay explicit and functional. (FR-004)
- [ ] T008 [US2] Validate required env wiring (`VITE_BANANA_API_BASE_URL`) in build/runtime lanes. (FR-001, FR-003)

## Phase 4: Verification and Evidence

- [ ] T009 [US1] Run targeted frontend validation (`bun run build` and screenshot/runtime checks where applicable). (FR-001)
- [ ] T010 [US2] Run CI lane(s) proving minimal automation contract is intact. (FR-003, FR-004)
- [ ] T011 [US3] Record feature evidence and residual risk notes in this spec directory. (FR-005)

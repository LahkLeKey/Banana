# Tasks: Banana Engineer

**Input**: Design documents from `.specify/specs/001-react-portal-game-client/`
**Prerequisites**: plan.md, spec.md

## Phase 1: Baseline and Scope Guardrails

- [x] T001 [US3] Inventory active workflow/spec roots and identify out-of-scope entries for retirement or archive. (FR-007)
- [x] T002 [US3] Define and document active-root guardrails to prevent reintroduction of non-pivot workflow/spec files. (FR-007)

## Phase 2: WASM Landing + 2.5D Shell Pivot

- [x] T003 [US1] Implement WASM-backed landing route bootstrap in the React app entry flow. (FR-001, FR-004)
- [x] T004 [US1] Build landing presentation with native 2.5D scene conventions instead of flat web-page framing. (FR-002)
- [x] T005 [US2] Rework shell/navigation to preserve 2.5D game-client continuity after landing handoff. (FR-002, FR-003)
- [x] T006 [US2] Validate desktop and mobile-sized viewport behavior for landing and primary routes. (FR-003)

## Phase 3: Minimal Automation for Safe Shipping

- [x] T007 [US3] Confirm React CI lane in `.github/workflows/banana.yml` validates WASM landing build/runtime contracts without workflow sprawl. (FR-006, FR-007)
- [x] T008 [US3] Verify deployment path requirements for React portal releases stay explicit and functional with WASM assets. (FR-004)
- [x] T009 [US3] Validate required env wiring (`VITE_BANANA_API_BASE_URL`) in build/runtime lanes. (FR-005)

## Phase 4: Verification and Evidence

- [x] T010 [US1] Run targeted frontend validation (`bun run build` and landing boot checks) for WASM route startup. (FR-001, FR-004)
- [x] T011 [US2] Run screenshot/runtime checks proving 2.5D continuity across viewport profiles. (FR-002, FR-003)
- [x] T012 [US3] Run CI lane(s) proving minimal automation contract remains intact for this pivot. (FR-006, FR-007)
- [x] T013 [US3] Record feature evidence and residual risk notes in this spec directory. (FR-007)

## Phase 5: Procedural Asset Compiler (C)

- [x] T014 [US4] Define deterministic generation inputs in procedural-generation-inputs.md (seed/profile/width/height/cellularAutomataIterations/override order) for baseline Banana Engineer landing and shell asset sets. (FR-009)
- [x] T015 [US4] Implement a C-based procedural asset compiler that emits web-consumable generated asset bundles for the React/WASM client. (FR-008, FR-010)
- [x] T016 [US4] Integrate generated bundle ingestion into the React build/bootstrap path without requiring manual art authoring for baseline scenes. (FR-004, FR-010)
- [x] T017 [US4] Add CI/local validation that re-runs compiler generation and verifies deterministic output + bundle compatibility with landing/shell runtime. (FR-006, FR-009, FR-010)

## Phase 6: DDD + SOLID Architecture Hardening

- [x] T018 [US4] Split procedural generation domain model/rules from orchestration and JSON/file output adapters. (FR-011)
- [x] T019 [US4] Introduce stable algorithm contracts so WFC, cellular automata, and future strategies can be composed without cross-layer rewrites. (FR-011)
- [x] T020 [US4] Add architecture guard checks/tests proving deterministic behavior remains intact after DDD/SOLID decomposition. (FR-009, FR-011)

## Phase 7: Viewport Architecture Simplification

- [x] T021 [US1] Remove all React/Tailwind styling constraints (h-[100dvh], md:, object-contain, flex/justify-center, etc.) from GameEnginePage and simplify JSX to fixed-position div + canvas. Let C/WASM engine own all viewport sizing logic. (FR-001, FR-002)
- [x] T022 [US2] Extend C WASM build to expose canvas resize/fullscreen handlers and viewport context from native code so responsive scaling is driven entirely by the engine. (FR-002, FR-003)
- [x] T023 [US3] Document HUD overlay mounting points (fixed positioning above canvas) for future menu/status/controls integration. (FR-013)
- [x] T024 [US3] Validate viewport scaling and responsiveness across desktop/tablet/mobile-portrait/mobile-landscape; confirm C/WASM engine adapts rendering correctly to all screen sizes. (FR-002, FR-003)

## Phase 8: Native Procedural Terrain Runtime

- [x] T025 [US1] Render deterministic C-generated terrain directly in the runtime 2.5D scene (no React sprite terrain overlays). (FR-002, FR-008, FR-012)
- [x] T026 [US2] Tune terrain layering, palette colors, and material readability so terrain remains visible behind entity meshes across viewport sizes and stays aligned with the visual rendering contract. (FR-003, FR-012)
- [x] T027 [US3] Add runtime validation evidence proving terrain visibility and C-owned viewport resizing on mobile + desktop. (FR-003, FR-006, FR-012)

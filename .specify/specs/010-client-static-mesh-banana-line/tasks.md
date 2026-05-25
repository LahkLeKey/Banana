# Tasks: Client Static Mesh Generators and Banana Line Travel

**Input**: Design documents from .specify/specs/010-client-static-mesh-banana-line/

**Prerequisites**: plan.md, spec.md

**Tests**: Native deterministic tests are required for each user story increment.

## Phase 1: Setup (Shared Infrastructure)

- [x] T001 Create native terrain static mesh scaffold files in src/native/engine/runtime/terrain/terrain_static_mesh.h and src/native/engine/runtime/terrain/terrain_static_mesh.c
- [x] T002 Wire terrain static mesh source in src/native/engine/CMakeLists.txt
- [x] T003 Create baseline playtest executable in tests/native/runtime/terrain/runtime_terrain_static_mesh_generation_test.c

## Phase 2: Foundational (Blocking Prerequisites)

- [x] T004 Define finalized profile metadata from the eight canonical regions in docs/banana-archipelago-worldbuilding.md and update runtime/terrain/terrain_static_mesh.c
- [x] T005 Add generator contract version and profile version drift checks between native and API/client bootstrap contracts
- [x] T006 Add artifact capture path for deterministic profile generation outputs under artifacts/native

## Phase 3: User Story 1 - Client Static Mesh Baselines (Priority: P1)

- [x] T007 [US1] Expand canonical region profile set with county anchors, county metadata, and named biome intents
- [x] T008 [US1] Add deterministic parity tests for all profile hubs and sampled local chunk offsets
- [x] T009 [US1] Run focused native playtest suite and publish parity evidence

## Phase 4: User Story 2 - Banana Line Travel (Priority: P1)

- [x] T010 [US2] Extend Banana Line routing to support route IDs and intermediate corridor stops
- [x] T011 [US2] Add route determinism tests across all county-to-county and county-to-islands pairs
- [x] T012 [US2] Validate route traversal streaming envelope assumptions in runtime playtest passes

## Phase 5: User Story 3 - Storage Budget Enforcement (Priority: P2)

- [x] T013 [US3] Add API contract checks that reject baseline mesh blob persistence payloads
- [x] T014 [US3] Add storage-budget simulation test covering deltas plus progression ledgers under 10 GB cap
- [x] T015 [US3] Add operator-facing storage diagnostics for terrain deltas and progression ledgers

## Phase 6: Polish and Cross-Cutting

- [x] T016 Update .specify/specs/execution-dashboard.md with 010 status progress
- [x] T017 Refresh quickstart playtest commands after each generator increment
- [x] T018 Capture end-to-end evidence bundle for deterministic generation, Banana Line routes, and DB budget checks

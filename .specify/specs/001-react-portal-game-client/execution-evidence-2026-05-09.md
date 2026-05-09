# Execution Evidence - 2026-05-09

## Completed Deliverables

- T001/T002: active root inventory and guardrails documented.
- T003: root route now serves a WASM landing entry page.
- T004: landing page uses 2.5D visual framing and WASM lifecycle UX.
- T005: workspace shell visual framing aligned with landing continuity.
- T007/T008/T009: CI/env/deployment wiring validated in canonical harness + local build flow.
- T010: targeted frontend validation run via `bun run build` with required env contract.
- T006/T011: desktop/mobile viewport runtime checks and screenshot captures completed.
- Legacy React overlay cleanup: removed random dashboard module overlays from default workspace route and replaced with mission-control cards + quick actions.
- Primary engine route simplified to viewport-only presentation: full-screen 2.5D WASM surface with C-generated asset overlay and no dashboard chrome.
- Legacy router cutover: root route now serves the new engine surface and prior routes redirect back to `/`.
- Dead legacy React route surface removed from source tree after router cutover (unused pages, shell, and orphaned telemetry test deleted).
- Deterministic procedural-generation input contract documented and emitted through the canonical generated asset manifest.
- Native compiler output canonicalized to `banana-generated-assets.manifest.json`; stale duplicate manifest output removed from the generation path.
- Native asset compiler integration test and React build both passed against the updated deterministic manifest contract.
- Minimal automation contract validators now pass after restoring the missing prompt contracts and syncing prompt wiki snapshots.
- Procedural generation architecture split confirmed in the native toolchain (`domain/`, `application/`, `infrastructure/`) with no monolithic compiler file owning rules, orchestration, and JSON output together.
- Stable procedural algorithm contracts now mediate generation service orchestration via `banana_asset_algorithm_contract_t` and a default composable WFC + cellular automata contract.
- Native generation tests now include explicit architecture guards for the decomposed contract seam, including invalid-contract rejection and deterministic behavior through the contract path itself.

## Changed Files

- `.specify/feature.json`
- `.specify/specs/001-react-portal-game-client/active-root-inventory.md`
- `.specify/specs/001-react-portal-game-client/active-root-guardrails.md`
- `.specify/specs/001-react-portal-game-client/tasks.md`
- `.specify/specs/001-react-portal-game-client/heartbeat-log.md`
- `src/typescript/react/src/pages/LandingPage.tsx`
- `src/typescript/react/src/lib/router.tsx`
- `.specify/specs/001-react-portal-game-client/viewport-runtime-checks-2026-05-09.md`
- `.specify/specs/001-react-portal-game-client/procedural-generation-inputs.md`
- `src/native/engine/tools/banana_asset_compiler.c`
- `src/native/engine/tools/infrastructure/banana_asset_json_writer.c`
- `src/native/engine/tools/infrastructure/banana_asset_json_writer.h`
- `src/typescript/react/scripts/prepare-procedural-assets.mjs`
- `tests/native/engine/test_engine_asset_compiler_integration.c`
- removed legacy React route surface files under `src/typescript/react/src/pages/` and `src/typescript/react/src/components/`

## Validation Commands and Results

1. `bun run build` in `src/typescript/react`
   - First run failed as expected when `VITE_BANANA_API_BASE_URL` was missing.
   - Confirms env contract enforcement in `vite.config.ts`.
2. `VITE_BANANA_API_BASE_URL=https://api.banana.engineer bun run build`
   - Passed.
   - `prepare-wasm-assets.mjs` copied WASM artifacts.
   - `prepare-procedural-assets.mjs` generated and copied procedural bundles.
3. `.specify/scripts/bash/validate-spec-boundaries.sh --spec .specify/specs/001-react-portal-game-client/spec.md`
   - Passed.
4. `.specify/scripts/bash/validate-task-traceability.sh --spec .specify/specs/001-react-portal-game-client/spec.md --tasks .specify/specs/001-react-portal-game-client/tasks.md --report artifacts/spec-validation/001-react-portal-game-client-traceability.md`
   - Passed with 20/20 mapped, 0 drift.
5. Runtime viewport checks using integrated browser + Playwright controls
   - Checked `/` and `/workspace` at `1366x900` and `390x844`.
   - Verified expected landing/shell text markers and no horizontal overflow.
6. CI-lane contract probes
   - `bash scripts/check-workflow-runtime-deprecations.sh --workflow .github/workflows/banana.yml` -> pass.
   - `bash scripts/validate-workflow-dependencies.sh` -> informational no-match for orchestrate wildcard lanes.
   - `python .specify/scripts/validate-ai-contracts.py` -> fail due missing legacy prompt contract files unrelated to landing pivot implementation.
7. Legacy overlay cleanup validation
   - Updated workspace route to remove legacy overlay-heavy module grid.
   - Visual verification in integrated browser confirms mission-control-only default surface.
   - `VITE_BANANA_API_BASE_URL=https://api.banana.engineer bun run build` remains passing after cleanup.
8. Viewport-only engine route validation
   - Updated `/game-engine` to bypass workspace shell header and auxiliary panels.
   - Visual verification confirms only the viewport is visible with generated C assets composited into the scene.
   - `VITE_BANANA_API_BASE_URL=https://api.banana.engineer bun run build` passes after the simplification.
9. Legacy router redirect validation
   - Updated route table so the new engine lives at `/`.
   - Verified `/workspace` and `/game-engine` now redirect to `/` in the browser.
   - `VITE_BANANA_API_BASE_URL=https://api.banana.engineer bun run build` passes after the router cutover.
10. Dead route-surface cleanup validation
   - Deleted unreachable legacy page components and the unused workspace shell after root-engine cutover.
   - Removed orphaned `TelemetryDashboardPage` test with its page implementation.
   - `VITE_BANANA_API_BASE_URL=https://api.banana.engineer bun run build` passes after cleanup, with reduced bundled CSS footprint.
11. Procedural input contract validation
   - Added `.specify/specs/001-react-portal-game-client/procedural-generation-inputs.md` documenting canonical deterministic inputs: seed, profile, width, height, cellular automata iterations, and rule metadata.
   - Updated the native compiler manifest to emit deterministic input fields, algorithm metadata, rule metadata, and the canonical file name `banana-generated-assets.manifest.json`.
   - Updated the React prepare step to clear stale generated artifacts before compile/copy so only canonical outputs remain in the build contract.
12. Procedural compiler and ingestion validation
   - `VITE_BANANA_API_BASE_URL=https://api.banana.engineer bun run build` passed after manifest contract changes.
   - `cmake --preset default -DBANANA_ENABLE_ENGINE=ON && cmake --build --preset default --target banana_test_engine_asset_compiler_integration && ctest --preset default -R banana_test_engine_asset_compiler_integration --output-on-failure` passed.
   - Native integration test now verifies manifest kind/compiler metadata, deterministic input fields, post-process metadata, adjacency rule metadata, and checksum contract.
13. Minimal automation contract validation
   - `bash scripts/check-workflow-runtime-deprecations.sh --workflow .github/workflows/banana.yml` passed.
   - `bash scripts/validate-workflow-dependencies.sh` returned informational no-match for `orchestrate-*.yml` and did not report dependency drift for the active workflow surface.
   - `python .specify/scripts/validate-ai-contracts.py` passed after adding `.github/prompts/iterate-the-backlog.prompt.md` and `.github/prompts/focus-on-open-pull-requests.prompt.md` and syncing prompt wiki snapshots with `scripts/workflow-sync-wiki.sh`.
14. Architecture split evidence
   - Domain rules and algorithms remain isolated in `src/native/engine/tools/domain/`.
   - Application orchestration remains isolated in `src/native/engine/tools/application/banana_asset_generation_service.c`.
   - JSON/file output adapters remain isolated in `src/native/engine/tools/infrastructure/banana_asset_json_writer.c`.
   - No single compiler file now owns deterministic rules, orchestration, and persistence simultaneously.
15. Stable algorithm contract validation
   - Added `banana_asset_algorithm_contract_t` to formalize the tile-generation and post-process seam.
   - `banana_generate_terrain_cells_with_contract(...)` now routes orchestration through the algorithm contract instead of hard-coding concrete WFC/CA calls.
   - `cmake --build --preset default --target banana_test_engine_asset_generation && ctest --preset default -R banana_test_engine_asset_generation --output-on-failure` passed with assertions for the exposed default contract names and composable execution path.
16. Architecture guard validation
   - Added native tests proving the contract path preserves deterministic output for repeated same-seed runs.
   - Added native guard tests proving invalid algorithm contracts are rejected instead of being silently accepted.
   - `cmake --build --preset default --target banana_test_engine_asset_generation && ctest --preset default -R banana_test_engine_asset_generation --output-on-failure` passed after the new guard checks were added.

## Residual Risks

- No open procedural-generation architecture tasks remain in spec 001.

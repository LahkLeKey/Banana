# Heartbeat Log: Spec 051 Native+AI+WASM

## 2026-05-15

### Feature Pointer Activation

- Updated `.specify/feature.json` to target `.specify/specs/051-ai-gameplay-loop`.
- Preserved cross-spec continuity note in Spec 050 so multiplayer baseline authority is explicit while Spec 051 executes native+WASM rehydration.

### Unified Spec Merge

- Absorbed Spec 050 authority (multiplayer baseline + anti-cheat governance + evidence posture) into Spec 051.
- Marked Spec 050 as absorbed/reference-only and routed active planning/tasks to Spec 051 artifacts.

### Baseline Capture

- Captured that AI loop code was partially wired in engine but full runtime was blocked by root native build drift.
- Captured runtime compose failure due to missing legacy core source references in root native target.
- Captured web viewport dependency on WASM artifacts (`engine.js`, `engine.wasm`, `engine.version.json`) and need for orchestration-level publication.

### Confirmed Breakpoints

- Root `banana_native` build still referenced deleted legacy files under `src/native/core`.
- Wrapper implementation still depended on removed `banana_core_*` and `banana_dal_*` contracts.
- Domain migration status was uneven: some services implemented, several still stubs or partial compatibility anchors.
- WASM build path existed but was not fully baked into canonical runtime orchestration channels.

### Decisions Ratified

- Treat native domain bake-in and WASM orchestration as part of the same execution unit for this spec.
- Preserve ABI/symbol stability while rerouting implementations to active domain services.
- Fail loudly with actionable guidance when web runtime artifacts/toolchain are missing.

### Confidence

- Confidence to continue implementation without additional discovery: 0.88
- Main risks: wrapper/domain parity gaps and runtime channel coupling across native + web artifact flows.

### Next Heartbeats

- Native rebind heartbeat: root `banana_native` build succeeds without legacy core references.
- Wrapper heartbeat: ABI-compatible wrapper delegates to active domain services.
- WASM heartbeat: artifacts generated/published by canonical orchestration path.
- Runtime heartbeat: compose runtime+apps starts and web viewport shows AI-driven movement.

### Worldgen Domainization (WFC + Cellular Automata)

- Added explicit engine domain services for terrain synthesis:
	- `src/native/engine/domain/services/wfc_worldgen_service.{h,c}`
	- `src/native/engine/domain/services/cellular_worldgen_service.{h,c}`
- Rewired `engine.c` terrain generation to use WFC synthesis followed by Cellular Automata smoothing.
- Updated `src/native/engine/CMakeLists.txt` so both native and WASM engine targets include the new domain services.
- Updated `src/native/engine/domain/services/README.md` documenting service responsibilities.
- Validation: `src/native/engine/engine.c` diagnostics clean and `cmake --build` from `src/native/engine/build` succeeds.

### WASM Parallel Contract + CI Hard Gate

- Added pthread/OpenMP contract flags to WASM build surfaces:
	- `src/native/engine/CMakeLists.txt` (WASM target pthread/OpenMP options)
	- `scripts/build-engine-wasm.sh` (pthread/OpenMP/pool flags + worldgen service sources)
- Added CI hard gate script: `scripts/validate-wasm-parallel-contract.sh`.
- Added monorepo CI job: `Runtime / wasm parallel contract gate` in `.github/workflows/banana.yml`.
- Added regression coverage:
	- `tests/scripts/test_validate_wasm_parallel_contract.py`
	- `.specify/scripts/validate-ai-contracts.py` checks for required workflow/build-script fragments.

## 2026-05-16

### Compose Runtime Unblock (Wrapper ML Shared Include)

- Fixed native wrapper compile blocker in `src/native/wrapper/domain/ml/shared/banana_wrapper_ml_json.h` by removing the stale include `domain/banana_ml_models.h` and introducing local shared compatibility structs used by the JSON serializer.
- Added missing `<stdio.h>` include in `src/native/truck/domain/services/truck_service.c` to resolve `snprintf` declaration warnings in containerized native builds.
- Re-ran workspace task `Compose: frontend react up + ready`; native builder image completed and copied `libbanana_native.so` successfully.

### Runtime Readiness Evidence

- Verified compose profile status with `docker compose --profile runtime --profile apps ps`:
	- `postgres` running and healthy
	- `api-fastify` running
	- `react-app` running
- Verified tail logs (`docker compose --profile runtime --profile apps logs --tail 80 react-app api-fastify`):
	- API listening on `:8080`
	- React Vite server ready on `:5173`
- Residual note: API log reports Prisma webhooks unavailable in this lane; runtime/app readiness remains green for the gameplay loop and renderer validation path.

### Legacy Truck/Logistics Domain Removal

- Removed the native truck domain files:
	- `src/native/truck/domain/entities/truck.h`
	- `src/native/truck/domain/services/truck_service.h`
	- `src/native/truck/domain/services/truck_service.c`
- Removed truck domain wiring from root native build graph in `CMakeLists.txt`.
- Updated wrapper compatibility layer (`src/native/wrapper/banana_core_compat.c`) so ABI truck exports remain present but now return `BANANA_INVALID_ARGUMENT` as explicit unsupported compatibility endpoints.
- Validation:
	- `cmake --build build/native --config Release --target banana_native` succeeds and emits `build/native/Release/banana_native.dll`.
	- Full solution build still reports unrelated pre-existing test include failures in `tests/native/test_ml_transformer*.c` (missing legacy header path), but core native library build is green.

### Gameplay Asset Import Path (Infra-Relevant)

- Updated React gameplay asset prep script `src/typescript/react/scripts/prepare-procedural-assets.mjs` to import generated assets from either:
	- `artifacts/generated-assets` (primary compiler output), or
	- `generated-assets-integration` (integration fallback source).
- This keeps gameplay asset ingestion aligned with infra/runtime channels even when compiler artifacts are unavailable in a lane.
- Validation:
	- Ran `node scripts/prepare-procedural-assets.mjs` from `src/typescript/react`.
	- Confirmed imported outputs in `src/typescript/react/public/generated-assets/`:
		- `banana-generated-assets.manifest.json`
		- `banana-generated-palette.json`
		- `banana-generated-terrain.json`

### Banana.Engineer Concept Asset Import

- Added gameplay concept metadata file: `generated-assets-integration/banana-engineer-theme.json`.
- Added React concept prep script: `src/typescript/react/scripts/prepare-concept-assets.mjs`.
- Wired script into React asset/build pipeline in `src/typescript/react/package.json`:
	- `prepare:concept-assets`
	- `prepare:assets` now includes concept prep
	- `build` now includes concept prep before TypeScript/Vite build
- Imported provided concept art URLs into repo integration path `generated-assets-integration/concept-art/` and exposed them to runtime via `src/typescript/react/public/concept-assets/`.

Validation:
	- Ran `node scripts/prepare-concept-assets.mjs` from `src/typescript/react`.
	- Confirmed runtime outputs:
		- `banana-engineer-concept-assets.manifest.json`
		- `banana-engineer-theme.json`
		- `source-01.webp`
		- `source-02.webp`
		- `source-03.webp`
		- `source-05.webp`
		- `source-06.webp`
	- One provided URL currently returns `HTTP 404` during strict import verification (slot 4) and is tracked as missing in theme metadata.

### Concept Art Correction + 2.5D Character Pipeline

- Replaced prior concept imports with user-provided chat attachments and republished concept assets:
	- `generated-assets-integration/concept-art/source-01.png` ... `source-07.png`
	- `src/typescript/react/public/concept-assets/source-01.png` ... `source-07.png`
- Updated `generated-assets-integration/banana-engineer-theme.json` to the attachment-based PNG source set (no missing slots).
- Extended concept importer to accept `png/jpg/jpeg/webp` source files.

- Added source-controlled 2.5D animated character pack seed for gameplay generation:
	- `generated-assets-integration/characters/banana-tempest/character.2p5d.json`
	- Includes layered billboard+bone model contract, clip set (`idle`, `run`, `jump`, `slash`, `cast`, `hurt`), and generation plan for batch rig/clip export.

- Added character asset prep pipeline:
	- New script `src/typescript/react/scripts/prepare-character-assets.mjs`
	- `src/typescript/react/package.json` now includes `prepare:character-assets`
	- `prepare:assets` and `build` now run concept + character prep for runtime consistency.

Validation:
	- Ran `node scripts/prepare-concept-assets.mjs && node scripts/prepare-character-assets.mjs` from `src/typescript/react`.
	- Confirmed runtime outputs:
		- `src/typescript/react/public/concept-assets/banana-engineer-concept-assets.manifest.json`
		- `src/typescript/react/public/characters/banana-character-assets.manifest.json`
		- `src/typescript/react/public/characters/banana-tempest/character.2p5d.json`

### C-Only Asset Generation Finalization (No Integration Folder)

- Removed legacy integration directories from both repo root and React-local paths:
	- `generated-assets-integration/`
	- `src/typescript/react/generated-assets-integration/`
- Kept React asset prep scripts strictly artifact-based and C-generated:
	- `src/typescript/react/scripts/prepare-procedural-assets.mjs` -> `artifacts/generated-assets`
	- `src/typescript/react/scripts/prepare-concept-assets.mjs` -> `artifacts/generated-concepts`
	- `src/typescript/react/scripts/prepare-character-assets.mjs` -> `artifacts/generated-characters`
- Added Windows-safe character generator compile behavior by linking `-lm` only on non-Windows platforms in `src/typescript/react/scripts/prepare-character-assets.mjs`.
- Updated native defaults and tests away from integration path assumptions:
	- `src/native/engine/tools/banana_character_vector_compiler.c` default out dir now `artifacts/generated-characters`
	- `tests/native/engine/test_engine_asset_compiler_integration.c` now writes/reads under `artifacts/generated-assets`

Validation:
	- Ran `node scripts/prepare-procedural-assets.mjs` from `src/typescript/react`; generated runtime copies under `src/typescript/react/public/generated-assets/`.
	- Ran `node scripts/prepare-concept-assets.mjs` from `src/typescript/react`; generated split-mesh runtime copies under `src/typescript/react/public/concept-assets/meshes/` plus manifest.
	- Ran `node scripts/prepare-character-assets.mjs` from `src/typescript/react`; generated character runtime copies under `src/typescript/react/public/characters/banana-tempest/` including `vectors/` and `clips/`.

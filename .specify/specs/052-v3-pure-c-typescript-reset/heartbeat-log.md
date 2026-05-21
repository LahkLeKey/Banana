# Heartbeat Log: Spec 052 V3 Fresh Reset

## 2026-05-16

- Created Spec 052 as new active reset authority.
- Archived prior active specs (050 and 051) under `.specify/legacy-baseline/specs-archive/2026-05-v3-reset`.
- Captured initial codebase reality scan for pure C + TypeScript baseline direction.
- Added v3 steering rule: prioritize gameplay/customer-facing capabilities only; non-gameplay surfaces are excluded.
- Added v3 execution rule: implementation must run in explicit parallel lanes with clear boundaries.
- Executed monorepo nuke-for-v3 reset and rebuilt a minimal scaffold baseline.
- Reset `src/native` and `src/typescript/*` package surfaces to placeholder scaffolds.
- Reset `tests`, `data`, `docker`, `docs`, and `scripts` to minimal scaffold placeholders.
- Recorded latest reset archive pointer in `.legacy/latest-v3-nuke-stamp`.
- Published explicit V3 scope gate checklist in `v3-scope-gate-checklist.md`.
- Published V3 bootstrap docket in `v3-bootstrap-docket.md` with dependency/risk order and lane merge policy.
- Executed full monorepo root sweep and archived legacy top-level surfaces under `.legacy/codebases/20260516-011230/root-sweep`.
- Resolved locked `src` path by in-place prune + scaffold rewrite; final `src` now contains scaffold-only files.
- Updated `.legacy/latest-v3-nuke-stamp` to `20260516-011230`.
- Recovered VS Code workspace artifacts after workspace loss:
	- `Banana.code-workspace`
	- `.vscode/tasks.json`
	- `.vscode/settings.json`
- Verified recovered workspace operation with smokes:
	- native configure/build (`cmake -S src/native -B out/v3-native`, `cmake --build out/v3-native`)
	- API/UI scaffold import checks via `bun -e`

### Current State

- V3 baseline is now physical scaffold reality, not planning-only intent.
- Legacy implementation drift is removed from active surfaces.
- Stage 3 bootstrap planning tasks are completed and execution-ready.
- Next work is additive slice implementation (S001-S006) on top of scaffold.
- Root repository state is now clean V3 baseline (`.git`, `.github`, `.legacy`, `.specify`, plus scaffold domains only).

### Historical Lane Handoff State (lineage-only)

- Lane A (Native C): ready for S001 (Native ABI Baseline).
- Lane B (API): ready for S002 (API Session Baseline).
- Lane C (Frontend): ready for S003 (Frontend Runtime Boot).
- Lane D (Shared UI): ready for S004 (Shared Gameplay Contracts).
- Stitch slices queued: S005 (contracts stitch), S006 (native/API boundary stitch).

### Historical Execution Evidence (S001-S004, lineage-only)

- These draft slice names and richer runtime references predate the finalized scaffold-only authority set.
- Keep them for archive traceability only; do not use them as active retained-capability evidence.

- S001 native baseline executed:
	- `cmake -S src/native -B out/v3-native` (pass)
	- `cmake --build out/v3-native` (pass)
	- shared library artifact built (`banana_native`)
- S002 API session baseline executed:
	- `bun -e` smoke for `startSession` + `apiHealth` (pass)
- S003 frontend runtime boot executed:
	- Electron boot smoke (`node -e import(...)`) (pass)
	- React + React Native boot smoke (`bun -e`) (pass)
- S004 shared gameplay contracts executed:
	- `normalizeMovement` smoke (`bun -e`) (pass)

### Historical Updated Handoff

- Lane A: S001 complete.
- Lane B: S002 complete; ready for S005.
- Lane C: S003 complete; ready for S005.
- Lane D: S004 complete; ready for S005.

### Historical Next Focus

- Start S001/S002/S003/S004 in parallel.
- Apply daily lane merge cadence.
- Escalate only stitch-slice blockers (S005/S006) into central heartbeat.
| 2026-05-16T07:20:23Z | go-copilot-start | 92 | continue | no | At or above threshold 80%. startup gate |

### Planning Refresh

- Ran `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json` and received a pass result.
- Recorded startup confidence gate pass at 92% for `go-copilot-start`.
- Generated `plan.md`, `research.md`, `data-model.md`, `quickstart.md`, and baseline contracts from live scaffold evidence.
- Updated agent context to point at `.specify/specs/052-v3-pure-c-typescript-reset/plan.md`.
- Confirmed active V3 authority is the current scaffold source tree plus Spec 052 pointers; archived specs and legacy code remain lineage-only.
- Marked unverified richer runtime claims in planning artifacts as non-authoritative until corresponding live source paths exist.

## 2026-05-17

### Setup and foundational checkpoints

- Re-ran `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json` and received a pass result.
- Re-ran `.specify/scripts/bash/spec-confidence-gate.sh --confidence 92 --step "go-copilot-start" --threshold 80 --notes "startup gate"` and recorded a pass result.
- Confirmed `.specify/feature.json` still points to `.specify/specs/052-v3-pure-c-typescript-reset`.
- Confirmed `.specify/specs/README.md` still lists Spec 052 as the sole active baseline.
- Reconciled `plan.md` and `data-model.md` so retained capabilities now require live runtime entry paths and the planning drilldowns are part of the active artifact inventory.
- Created root `.gitignore` and `.dockerignore` to cover the current C/TypeScript scaffold and Docker delivery surfaces.

### Reusable review slots

- **Authority review**: `spec.md`, `research.md`, `contracts/baseline-authority.md`
- **Evidence trace review**: `codebase-scan.md`, `v3-scope-gate-checklist.md`
- **Lane readiness review**: `contracts/execution-lanes.md`, `v3-bootstrap-docket.md`, `quickstart.md`
- **Final readiness review**: `tasks.md`, this heartbeat log, and the lane-local validation commands

### Baseline-authority review evidence

- Passed: Spec 052 remains the single active V3 authority.
- Passed: `.specify/legacy-baseline/**` and `.legacy/**` are recorded as lineage-only references.
- Passed: stale viewport-first assumptions and historical claims such as `startSession`, `apiHealth`, and `normalizeMovement` remain non-authoritative until current source paths reintroduce them.

### Retained-capability trace review

- `native-abi-baseline` -> `src/native/CMakeLists.txt`, `src/native/include/banana_native_v3.h`, `src/native/scaffold/native_entry.c`
- `api-shell-baseline` -> `src/typescript/api/package.json`, `src/typescript/api/src/index.ts`
- `react-shell-baseline` -> `src/typescript/react/package.json`, `src/typescript/react/src/index.ts`
- `electron-shell-baseline` -> `src/typescript/electron/package.json`, `src/typescript/electron/main.js`
- `react-native-shell-baseline` -> `src/typescript/react-native/package.json`, `src/typescript/react-native/index.ts`
- `shared-contract-shell-baseline` -> `src/typescript/shared/ui/package.json`, `src/typescript/shared/ui/src/index.ts`

### Lane-readiness review

- Lane A owns `src/native/**` and remains ready for native scaffold stewardship.
- Lane B owns `src/typescript/api/**` and remains ready for API gameplay orchestration planning.
- Lane C owns the React, Electron, and React Native runtime shells and remains ready for frontend planning.
- Lane D owns `src/typescript/shared/ui/**` and remains ready for shared gameplay contract planning.
- Reserved stitch slices remain limited to gameplay-contract and native/API boundary joins.

### Final readiness summary

- Cross-artifact authority, evidence, and lane ownership language now agree across the active Spec 052 artifact set.
- The active retained baseline is the scaffold-only repository state, not historical slice or richer runtime claims.
- Downstream planning can proceed from lane-local evidence without reopening archived specs for authority decisions.
- Reviewer note: earlier heartbeat references to additive slices and richer runtime names are preserved as historical context only and do not override the active artifact set.
| 2026-05-17T21:29:43Z | go-copilot-start | 92 | continue | no | At or above threshold 80%. startup gate |
| 2026-05-19T00:45:03Z | go-copilot-start | 92 | continue | no | At or above threshold 80%. autopilot execution pass |

## 2026-05-18

### Plan refresh after setup-template reset

- Re-ran `.specify/scripts/bash/setup-plan.sh --json` and confirmed the active feature still resolves to `.specify/specs/052-v3-pure-c-typescript-reset`.
- Re-ran `.specify/scripts/bash/spec-extension-preflight.sh --update-first --json` and received a pass result.
- Re-ran `.specify/scripts/bash/spec-confidence-gate.sh --confidence 92 --step "go-copilot-start" --threshold 80 --notes "startup gate"` and recorded a pass result.
- Rebuilt `plan.md` after the setup script recopied the template over the active plan.
- Reconciled `research.md`, `contracts/execution-lanes.md`, `codebase-scan.md`, and `v3-bootstrap-docket.md` to remove stale restore/server-authority assumptions and realign them to the scaffold-only active baseline.
- Confirmed `.github/copilot-instructions.md` still points at `.specify/specs/052-v3-pure-c-typescript-reset/plan.md`.
- Reaffirmed the four bounded execution lanes as planning handoff guides only; no archived capability claims remain active retained evidence.
| 2026-05-19T00:50:07Z | go-copilot-start | 92 | continue | no | At or above threshold 80%. startup gate |
| 2026-05-19T00:52:58Z | go-copilot-start | 90 | continue | no | At or above threshold 80%. startup gate |

### Implementation autopilot progress (manual continuation)

- Restored runtime implementation surfaces from `origin/049-arpg-view-actors` into the active branch: `src/native/engine`, `scripts/build-engine-wasm-emsdk.sh`, `scripts/build-engine-wasm.sh`, `scripts/build-wasm.sh`, `src/typescript/api`, `src/typescript/react`, `src/typescript/shared/ui`, and `src/typescript/shared/resilience`.
- Verified native lane compiles after restore with `cmake -S src/native -B out/v3-native && cmake --build out/v3-native`.
- TypeScript lane build checks currently blocked by missing dependencies/toolchain in the restored surfaces:
  - `npm run build --prefix src/typescript/api` failed (unresolved package imports such as `@fastify/helmet`).
  - `npm run build --prefix src/typescript/react` failed (`tsc` not found in environment).
  - `npm run build --prefix src/typescript/shared/ui` failed (unresolved runtime imports such as `react/jsx-dev-runtime`).
- Workflow run `eda38bf1` remains in `implement` step and is actively traversing restore + wiring tasks; no human-input prompt has been emitted in this phase yet.

### T023 restore verification follow-up

- Confirmed `scripts/build-engine-wasm-emsdk.sh`, `scripts/build-engine-wasm.sh`, and `scripts/build-wasm.sh` match `origin/049-arpg-view-actors` with no content drift.
- Hardened `scripts/build-engine-wasm-emsdk.sh` so local verification no longer depends on a single `/tmp/emsdk` layout:
	- prefers `emcc` on `PATH`
	- falls back to `EMSDK_PATH` or `/tmp/emsdk`
	- falls back to the CI image `emscripten/emsdk:3.1.57` when Docker is available
- Revalidated the wrapper with `bash -n scripts/build-engine-wasm-emsdk.sh` and an execution probe; syntax passes and the runtime failure is now an explicit toolchain-missing error instead of a hardcoded path failure.
- Local artifact production remains blocked on this workstation because no Emscripten toolchain is active and Docker Desktop cannot be started from the current session (`com.docker.service` is stopped; service start attempt returned access denied / system error 5).
- T023 remains open pending one of:
	- local `emcc` activation via emsdk
	- `EMSDK_PATH` pointing at a valid emsdk root
	- Docker Desktop started with sufficient permissions so the CI-style container build can run

### T023 completion note

- Bootstrapped a local emsdk under `.tools/emsdk` and used it to complete the WASM build path on Windows Git Bash without Docker.
- Updated `scripts/build-engine-wasm.sh` and `scripts/build-engine-wasm-emsdk.sh` so the build can use the exact emsdk compiler entrypoint exported by the wrapper (`EMCC_SCRIPT`) instead of relying only on `emcc` being on `PATH`.
- Added `engine_serialize.c` to the WASM source list so `engine_serialize_reset` links correctly.
- Final validation passed with `source .tools/emsdk/emsdk_env.sh && EMSDK_PATH="$PWD/.tools/emsdk" bash scripts/build-engine-wasm-emsdk.sh --release`.
- Verified outputs:
	- `out/wasm/engine.js`
	- `out/wasm/engine.wasm`
	- `src/typescript/react/public/wasm/engine.js`
	- `src/typescript/react/public/wasm/engine.wasm`
	- `artifacts/wasm/banana-wasm-engine.js`
	- `artifacts/wasm/banana-wasm-engine.wasm`

### Playable baseline validation (spec-driven)

- Revalidated lane-local build gates with package-owned scripts:
	- `bun run --cwd src/typescript/shared/ui build`
	- `bun run --cwd src/typescript/api build`
	- `bun run --cwd src/typescript/react build`
	- `npm run --prefix src/typescript/electron build`
	- `npm run --prefix src/typescript/react-native build`
- Revalidated native gameplay baseline:
	- `cmake -S src/native -B out/v3-native`
	- `cmake --build out/v3-native`
	- `ctest --test-dir out/v3-native -C Debug --output-on-failure` (pass: `banana_engine_serialize_test`)
- Revalidated server-authority gameplay surfaces:
	- `bun test src/typescript/api/src/routes/game-session.test.ts` (pass; session flow, websocket snapshots, and input invariants)
	- `bun test src/typescript/shared/ui/src/domains/ServerSnapshotIntegration.test.ts` (pass; snapshot reconciliation + replay)
	- `bun test --cwd src/typescript/react src/pages/__tests__/GameEnginePage.context-menu.test.tsx` (pass; gameplay page interactions)
- Current outcome: Spec 052 implementation lanes are green with playable server-authority gameplay baseline evidence captured from native, API, shared-contract, and React runtime surfaces.

### Live playable smoke (manual runtime)

- Started API runtime with explicit native binding path:
	- `cd /c/Github/Banana && BANANA_NATIVE_PATH="/c/Github/Banana/out/v3-native/Debug/banana_native.dll" PORT=8081 HOST=127.0.0.1 bun run --cwd src/typescript/api dev`
- Started React runtime against local API:
	- `cd /c/Github/Banana && VITE_BANANA_API_BASE_URL="http://127.0.0.1:8081" bun run --cwd src/typescript/react dev --host 127.0.0.1 --port 5173`
- Confirmed session-room and room creation path in browser (`/session-room` -> `/game-engine`) and verified active websocket session state.
- Fixed live crash in `src/typescript/react/src/pages/GameEnginePage.tsx` by defaulting snapshot players to an empty array when omitted (`setSessionPlayers(payload.players ?? [])`).
- Revalidated page-level gameplay tests after fix:
	- `bun test --cwd src/typescript/react src/pages/__tests__/GameEnginePage.context-menu.test.tsx` (pass)
- Verified playable signal in live UI:
	- Session status: `connected`
	- Server tick advanced during runtime readback (`143` -> `252`)
	- Engine asset version rendered (`v20260519-023752-ec64cfe1e9df`)

### Shared-world netcode identity slice

- Added server-side player identity decoration to authoritative snapshot entities in `src/typescript/api/src/routes/game-session.ts`.
	- Snapshots now include optional per-player actor metadata (`ownerPlayerId`, `ownerPlayerName`, `tooltipLabel`) for player entities.
- Expanded local default CORS allowlist in `src/typescript/api/src/index.ts` to include `127.0.0.1` dev origins so local host alias mismatch does not block room bootstrap.
- Extended React snapshot typing in `src/typescript/react/src/lib/api.ts` for the new optional player identity metadata fields.
- Added `src/typescript/react/src/components/ActorTooltipOverlay.tsx` and wired it into `src/typescript/react/src/pages/GameEnginePage.tsx`.
	- Client now renders lightweight tooltip labels above player actors from server-authoritative snapshot data.
	- Labels show randomized server-assigned player identity suffixes (for example `player · eml53g`) while account registration remains deferred.
- Validation evidence:
	- `bun test src/typescript/api/src/routes/game-session.test.ts` (pass)
	- `bun test --cwd src/typescript/react src/pages/__tests__/GameEnginePage.context-menu.test.tsx` (pass)
	- `bun run --cwd src/typescript/api build` (pass)
	- `bun run --cwd src/typescript/react build` (pass)
	- Live smoke on `http://127.0.0.1:5173/session-room`:
		- room creation succeeded
		- `/game-engine` reached connected websocket state
		- server tick advanced continuously (`0` -> `402` observed)
		- actor label overlay rendered randomized player ID tooltip

### Shared-world identity regression coverage

- Added API route-level assertion coverage to ensure websocket snapshot payloads include player identity metadata on player entities (`ownerPlayerId`, `ownerPlayerName`, `tooltipLabel`) in `src/typescript/api/src/routes/game-session.test.ts`.
- Added React component coverage for actor label rendering behavior in `src/typescript/react/src/components/ActorTooltipOverlay.test.tsx`.
- Validation rerun:
	- `bun test src/typescript/api/src/routes/game-session.test.ts` (pass)
	- `bun test --cwd src/typescript/react src/components/ActorTooltipOverlay.test.tsx` (pass)
	- `bun test --cwd src/typescript/react src/pages/__tests__/GameEnginePage.context-menu.test.tsx` (pass)

### Overworld server-authority contract refinement

- Converted gameplay room model from multi-room selection to one static authoritative shared room (`sessionId=overworld`) in `src/typescript/api/src/routes/game-session.ts` and `src/typescript/react/src/pages/SessionRoomPage.tsx`.
- Added server timing telemetry to authoritative snapshot/status payloads:
	- `targetTps`
	- `currentTps`
	- `averageTickMs`
	- `lagMs`
- Exposed TPS/lag readouts in the gameplay player overlay (`src/typescript/react/src/components/PlayerListOverlay.tsx`) via updated API contracts (`src/typescript/react/src/lib/api.ts`).
- Added compose-first local runtime channel for inspectable container logs:
	- `docker-compose.yml` (`api-overworld`, `react-overworld`)
	- `docker/README.md`
	- `.specify/specs/052-v3-pure-c-typescript-reset/quickstart.md`
- Updated Spec 052 requirements and task docket to encode next native extension direction: GUID-rooted `PlayerEntity` base with pluggable controller strategy for human/AI/automation lanes.

### Native PlayerEntity + controller strategy implementation

- Implemented GUID-rooted native player entity bindings in `src/native/engine/engine.c` with immutable GUID mapping, per-player controller strategy (`human`/`ai`), active-state tracking, and per-player movement intent buffering.
- Extended native entity model in `src/native/engine/world/entity.h` to carry `player_guid` and `controller_kind` metadata.
- Extended native serialization in `src/native/engine/engine_serialize.c` so authoritative snapshots emit `playerGuid` and `controllerKind` on player entities.
- Added native API hooks in `src/native/engine/engine.h`:
	- `engine_player_upsert(...)`
	- `engine_player_apply_input(...)`
- Wired API/native mapping in:
	- `src/typescript/api/src/services/nativeEngine.ts` (new `syncPlayers(...)` contract + FFI bindings)
	- `src/typescript/api/src/routes/game-session.ts` (session players + inputs synced into native each authoritative tick)
- Coverage expansion:
	- `tests/native/engine_serialize_test.c` now asserts GUID/controller metadata serialization and player upsert/input execution path.
- Validation rerun:
	- `cmake -S src/native -B out/v3-native` (pass)
	- `cmake --build out/v3-native` (pass)
	- `ctest --test-dir out/v3-native -C Debug --output-on-failure` (pass)
	- `bun test src/typescript/api/src/routes/game-session.test.ts` (pass)
	- `bun run --cwd src/typescript/api build` (pass)

### Compose runtime channel verification

- Verified container-first authoritative overworld runtime from repository root:
	- `docker compose config` (pass)
	- `docker compose up -d api-overworld react-overworld` (pass)
	- `docker compose ps` (both services up)
	- `docker compose logs --tail=120 api-overworld react-overworld` (expected startup/install logs)
	- `curl http://127.0.0.1:8081/health` => `{"status":"ok"}`
	- `curl http://localhost:5173/` returned React HTML shell
	- `docker compose down` (pass)

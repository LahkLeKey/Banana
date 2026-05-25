# 010 End-to-End Evidence Bundle

Date: 2026-05-25

## Scope

This bundle captures the final deterministic generation and storage-boundary checks for Spec 010 before merge readiness.

## Native Deterministic Playtest

Command:
- cmake --build out/v3-native --target banana_runtime_terrain_static_mesh_generation_test
- C:/Github/Banana/out/v3-native/engine/Debug/banana_runtime_terrain_static_mesh_generation_test.exe

Result:
- E2E_NATIVE_PASS

CTest Harness:
- ctest -C Debug -R static_mesh_generation --output-on-failure
- 1/1 passed, 0 failed

Validation Notes:
- Deterministic static-mesh generation parity remains stable.
- Banana Line route parity and route-envelope checks remain stable.
- Typed static-mesh subdomain layout compiles and executes under the same playtest surface.

## API Storage Boundary and Drift Playtest

Command:
- bun test src/domains/persistent-world-orchestration/services/storageBudgetService.test.ts src/domains/persistent-world-orchestration/services/versionDriftService.test.ts src/routes/world.persistent.contract.test.ts

Result:
- E2E_API_PASS
- 11 pass, 0 fail

Validation Notes:
- Mesh-like payload persistence is rejected by authoritative policy checks.
- Storage diagnostics and budget simulation cover terrain deltas plus progression ledgers under the 10 GB cap.
- Version drift diagnostics cover synchronized and divergent native/client contract states.

## Merge Readiness Decision

Status: READY_FOR_PLAYTEST_AND_MERGE

Remaining manual step:
- Execute your normal interactive runtime playtest session in the target channel and attach any player-observable notes to the PR summary.

## DX12 POC Launch-Gate Recovery

Issue observed:
- DX12 POC startup blocked at engine preflight with `reason=6 state=6` (`OFFLINE_UNVERIFIABLE`) when local launch-gate verification env flags were unset.

Fix applied:
- Added development-mode launch-gate defaults in [src/native/engine/win32_dx12_poc/app/main.c](src/native/engine/win32_dx12_poc/app/main.c) before `engine_init`.
- Defaults apply only when mode is development (or unset and defaulting to development), so production/staging behavior is preserved.

Validation:
- `cmake --build out/v3-native --target banana_engine_win32_dx12_poc`
- `C:/Github/Banana/out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe`
- Observed successful runtime attach and clean shutdown (`[engine] initialized`, `[engine] shutdown complete`).

Regression gate:
- `ctest -C Debug -R "launch_gate_(matrix|mode_policy)" --output-on-failure`
- 2/2 passed, 0 failed.

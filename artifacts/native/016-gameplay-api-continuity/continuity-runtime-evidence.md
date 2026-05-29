# 016 Gameplay/API Continuity Runtime Evidence

Date: 2026-05-26

## Scope

Capture runtime-side continuity checkpoint signatures and coherent transition diagnostics used by API persistence handoff.

## Initial Runtime Contract Inventory

Owning runtime surfaces:

- `src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.h`
- `src/native/engine/win32_dx12_poc/scene/demo_scene_catalog.c`
- `src/native/engine/win32_dx12_poc/app/main.c`

Key continuity APIs/signals already available:

- `banana_poc_demo_scene_catalog_coherent_transition_connected(...)`
- `banana_poc_demo_scene_catalog_coherent_transition_signature(...)`
- Runtime log lines for `coherent-world` and `coherent-transition`

## Baseline Runtime Signature

Representative command:

- `BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=2 ./out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe`

Representative output:

- `[dx12-poc] coherent-world variant=2 ... status=ok consistent=1`

## Planned Runtime Validation Commands

- `ctest -C Debug -R "runtime_demo_scene_catalog_(transition_continuity|transition_drift|coherent_profile)_test" --test-dir out/v3-native --output-on-failure`
- `BANANA_DX12_POC_AUTOTEST=1 BANANA_DX12_POC_AUTOTEST_SECONDS=1 BANANA_DX12_POC_SCENE_VARIANT=<variant> ./out/v3-native/engine/Debug/banana_engine_win32_dx12_poc.exe`

## Implemented Runtime Continuity Harness + Test

- New deterministic round-trip test:
	- `tests/native/runtime/engine/runtime_demo_scene_catalog_continuity_roundtrip_test.c`
- New CMake target registration:
	- `banana_runtime_demo_scene_catalog_continuity_roundtrip_test`
- New workspace harness task:
	- `.vscode/tasks.json` -> `Native: continuity suite (v3)`

## Runtime Validation Execution (US1)

- Command:
	- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_coherent_profile_test -- -m:1`
	- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_transition_continuity_test -- -m:1`
	- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_continuity_roundtrip_test -- -m:1`
	- `ctest -C Debug --test-dir out/v3-native -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|continuity_roundtrip_test)" --output-on-failure`
- Result:
	- `100% tests passed, 0 tests failed out of 3`
	- `banana_runtime_demo_scene_catalog_coherent_profile_test: Passed`
	- `banana_runtime_demo_scene_catalog_transition_continuity_test: Passed`
	- `banana_runtime_demo_scene_catalog_continuity_roundtrip_test: Passed`

## Runtime Alignment Drift Diagnostics (US2)

Validation command:

- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_transition_drift_test -- -m:1`
- `ctest -C Debug --test-dir out/v3-native -R "banana_runtime_demo_scene_catalog_transition_drift_test|banana_runtime_demo_scene_catalog_transition_continuity_test|banana_runtime_demo_scene_catalog_continuity_roundtrip_test" --output-on-failure`

Result:

- `100% tests passed, 0 tests failed out of 3`
- `banana_runtime_demo_scene_catalog_transition_continuity_test: Passed`
- `banana_runtime_demo_scene_catalog_transition_drift_test: Passed`
- `banana_runtime_demo_scene_catalog_continuity_roundtrip_test: Passed`

Alignment note:

- Runtime transition drift/continuity diagnostics now have a paired API strict-schema drift/failure suite in `artifacts/api/016-gameplay-api-continuity/continuity-contract-evidence.md`.

## Add-a-Field Workflow (Native/API Coordination)

For new continuity fields (for example `checkpointContextTag`), keep native continuity diagnostics stable while API contract ownership evolves:

1. Keep native transition signatures and connectivity checks deterministic via:
	- `banana_poc_demo_scene_catalog_coherent_transition_connected(...)`
	- `banana_poc_demo_scene_catalog_coherent_transition_signature(...)`
2. Add/adjust API contract fields under the single owning continuity schema path (`persistentWorld.ts`).
3. Ensure API signature derivation and diagnostics policy updates remain in `continuityPayloadService.ts`.
4. Re-run native drift/continuity suite:
	- `banana_runtime_demo_scene_catalog_transition_continuity_test`
	- `banana_runtime_demo_scene_catalog_transition_drift_test`
	- `banana_runtime_demo_scene_catalog_continuity_roundtrip_test`
5. Re-run API continuity harness (`test:continuity`, `test:integration:continuity`) and log both outputs in artifacts.

## Final Focused Runtime Validation (T023-T024)

Command set:

- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_coherent_profile_test -- -m:1`
- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_transition_continuity_test -- -m:1`
- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_transition_drift_test -- -m:1`
- `cmake --build out/v3-native --config Debug --target banana_runtime_demo_scene_catalog_continuity_roundtrip_test -- -m:1`
- `ctest -C Debug --test-dir out/v3-native -R "banana_runtime_demo_scene_catalog_(coherent_profile_test|transition_continuity_test|transition_drift_test|continuity_roundtrip_test)" --output-on-failure`

Result:

- `100% tests passed, 0 tests failed out of 4`
- `banana_runtime_demo_scene_catalog_coherent_profile_test: Passed`
- `banana_runtime_demo_scene_catalog_transition_continuity_test: Passed`
- `banana_runtime_demo_scene_catalog_transition_drift_test: Passed`
- `banana_runtime_demo_scene_catalog_continuity_roundtrip_test: Passed`

Final runtime summary:

- Coherent profile integrity, connected-transition continuity, drift detection, and round-trip determinism all pass in focused native validation.

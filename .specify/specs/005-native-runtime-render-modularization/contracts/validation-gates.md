# Contract: Validation Gates

## Purpose

Define the non-negotiable validation gates for each mergeable slice in the native runtime/render modularization effort.

## Global Gates

1. `cmake --build out/v3-native`
2. Focused `ctest --test-dir out/v3-native -C Debug -R ...` gate for the touched slice
3. `ctest --test-dir out/v3-native -C Debug --output-on-failure` before feature closeout
4. Evidence capture under `artifacts/native/`
5. No unapproved public ABI changes under `src/native/include/`

## Slice Matrix

| Slice | Required Focused Tests | Additional Evidence |
|-------|------------------------|---------------------|
| S1 Composition root | `banana_runtime_engine_host_test`, `banana_runtime_engine_lifecycle_test`, `banana_runtime_engine_state_test`, `banana_runtime_engine_tick_test`, `banana_runtime_architecture_dependency_guard_test` | Note which responsibilities left `engine_composition.c` |
| S2 Tick sequencing | `banana_runtime_engine_tick_test`, `banana_runtime_tick_input_phase_test`, `banana_runtime_tick_budget_policy_test`, `banana_runtime_tick_post_phase_test`, `banana_engine_input_contract_transition_test` | Record any newly introduced phase or orchestration helper |
| S3 DX12 backend diagnostics | `banana_engine_dx12_runtime_smoke_test`, `banana_dx12_projection_policy_test`, `banana_dx12_scene_overlay_frame_test` | Capture backend status / fallback evidence and touched ownership boundaries |
| S4 DX12 POC host | `banana_engine_win32_dx12_poc_startup_smoke`, `banana_dx12_objective_policy_test`, `banana_dx12_scene_flow_test`, `banana_dx12_gameplay_lane_test`, `banana_dx12_gameplay_click_lane_test` | Record which responsibilities left `win32_dx12_poc/main.c` |
| S5 Governance hardening | `banana_runtime_architecture_dependency_guard_test` plus at least one focused runtime test and one focused DX12 test for touched surfaces | Update concern-split checklist / exception ledger for each oversized file |

## Failure Handling

1. If the focused gate fails, repair the touched slice before expanding scope.
2. If the architecture guard fails, either fix the dependency direction immediately or document a time-bound exception tied to the next slice.
3. If DX12-specific tests fail after a decomposition change, treat it as a backend parity regression, not a test-only issue.

## Evidence Naming

Use one artifact file per validation pass:

```text
artifacts/native/ctest-debug-YYYY-MM-DD-slice-id.txt
```

Examples:

```text
artifacts/native/ctest-debug-2026-05-24-s2.txt
artifacts/native/ctest-debug-2026-05-24-s5.txt
```
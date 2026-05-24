# Quickstart: Native Runtime Render Modularization

## Purpose

Use this guide to execute the planned modularization slices with the smallest sufficient validation surface at each step.

## Prerequisites

- Run from the repository root.
- Use the existing native build directory `out/v3-native`.
- On Windows, keep DX12-specific validation on the same host where the native build already succeeds.

## Configure And Build

```bash
cmake -S src/native -B out/v3-native
cmake --build out/v3-native
```

If the workspace task is preferred, use `Native: build (v3)` after the configure step.

## Slice Gates

### S1: Composition root decomposition

```bash
ctest --test-dir out/v3-native -C Debug -R "banana_runtime_engine_(host|lifecycle|state)_test|banana_runtime_engine_tick_test|banana_runtime_architecture_dependency_guard_test" --output-on-failure
```

### S2: Remaining tick sequencing extraction

```bash
ctest --test-dir out/v3-native -C Debug -R "banana_runtime_engine_tick_test|banana_runtime_tick_input_phase_test|banana_runtime_tick_budget_policy_test|banana_runtime_tick_post_phase_test|banana_engine_input_contract_transition_test" --output-on-failure
```

### S3: DX12 backend diagnostics decomposition

```bash
ctest --test-dir out/v3-native -C Debug -R "banana_engine_dx12_runtime_smoke_test|banana_dx12_projection_policy_test|banana_dx12_scene_overlay_frame_test" --output-on-failure
```

### S4: Win32 DX12 POC host slimming

```bash
ctest --test-dir out/v3-native -C Debug -R "banana_engine_win32_dx12_poc_startup_smoke|banana_dx12_objective_policy_test|banana_dx12_scene_flow_test|banana_dx12_gameplay_lane_test|banana_dx12_gameplay_click_lane_test" --output-on-failure
```

### S5: Governance hardening

```bash
ctest --test-dir out/v3-native -C Debug -R "banana_runtime_architecture_dependency_guard_test|banana_runtime_engine_tick_test|banana_engine_dx12_runtime_smoke_test" --output-on-failure
```

## Broad Native Gate

```bash
ctest --test-dir out/v3-native -C Debug --output-on-failure
```

## Evidence Capture

For each slice, archive the focused validation output under `artifacts/native/` using a date-and-slice-stamped filename, for example:

```text
artifacts/native/ctest-debug-2026-05-24-s1.txt
artifacts/native/ctest-debug-2026-05-24-s3.txt
```

Also record whether the touched file ended in one of these states:

1. Split into smaller single-responsibility files.
2. Reduced to a thin coordinator over named helpers.
3. Left as a temporary exception with a follow-up slice.

## Closeout Check

Before closing the feature, confirm all of the following:

1. `src/native/include/` did not change in an ABI-breaking way.
2. The architecture guard covers any newly introduced runtime/render seam.
3. DX12 smoke and projection checks are still green after backend or POC changes.
4. The concern-split evidence for each touched oversized file is recorded in feature artifacts.

## Requirement Traceability

- FR-001 / FR-004 / FR-005 / FR-009: Validate through runtime tick focused tests and concern-split checklist updates.
- FR-002 / FR-003: Validate through ABI-stable code edits and focused behavior-parity suites.
- FR-006: Validate through DX12 diagnostics-focused suites and helper-boundary extraction.
- FR-007: Validate through architecture dependency guard test and rule-table updates.
- FR-008: Validate by adding focused assertions/rules without removing existing tests.
- FR-010: Validate by confirming no gameplay or rendering feature redesign in touched files.

Focused test locations follow subdomain folders:
- Runtime tick: `tests/native/runtime/tick/`
- DX12 render diagnostics/policies: `tests/native/render/dx12/`
- Architecture guards: `tests/native/architecture/`

## Evidence Paths

- `artifacts/native/005-us1-runtime-tick.txt`
- `artifacts/native/005-us2-dx12-diagnostics.txt`
- `artifacts/native/005-us3-architecture-guards.txt`
- `artifacts/native/005-final-focused-suite.txt`
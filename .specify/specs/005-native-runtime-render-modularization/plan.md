# Implementation Plan: Native Runtime Render Modularization

**Branch**: `native-ddd-phase13-arch-guards` | **Date**: 2026-05-24 | **Spec**: `.specify/specs/005-native-runtime-render-modularization/spec.md`

**Input**: Feature specification from `.specify/specs/005-native-runtime-render-modularization/spec.md`

## Summary

Phase-13 branch work already extracted the highest-value `engine_tick` seams into dedicated input, budget, and post-processing modules, and introduced a first-pass runtime architecture dependency guard. This plan treats that work as the baseline and focuses the remaining modularization on the composition root, the oversized DX12/render runtime surfaces, and governance hardening that keeps new boundaries enforceable. Delivery remains incremental: each slice must stay mergeable, preserve the public ABI, avoid gameplay or rendering redesign, and close with explicit focused validation before a broader native CTest sweep.

## Technical Context

**Language/Version**: C11 via CMake 3.20

**Primary Dependencies**: `banana_engine_core` static library, Win32 + D3D12/DXGI/D3DCompiler on Windows DX12 paths, OpenMP, CTest

**Storage**: N/A

**Testing**: Native CTest targets under `tests/native` executed from `out/v3-native`

**Target Platform**: Windows 10+ for DX12 runtime and POC coverage; Ubuntu WSL2/Linux remains relevant for non-DX12 compile and broader native validation

**Project Type**: Native C engine library with desktop runtime harness and platform-specific render backend adapters

**Performance Goals**: Preserve current runtime tick ordering and frame behavior; keep DX12 fallback, smoke, and gameplay-lane behavior parity while decomposition proceeds

**Constraints**: Public ABI must remain stable; no broad rewrites; additive tests only; native C implementation files should move toward single-responsibility; only `CMakeLists.txt` may remain intentionally large without explicit exception handling

**Scale/Scope**: `src/native/engine/runtime`, `src/native/engine/render`, `src/native/engine/win32_dx12_poc`, and `tests/native` guard/seam coverage for runtime-render modularization

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- [x] Player trust and disclosure alignment verified. This is internal native refactor work only and does not change player-facing claims, AI disclosures, controller claims, or minimum requirements.
- [x] Storefront governance artifacts identified when public Steam copy is affected. No storefront artifacts are required for this feature because scope is limited to internal native modularization.
- [x] Cross-domain contracts mapped for touched layers and required docs are queued in-scope. Touched layers are native runtime/render internals plus Spec Kit governance artifacts; public ABI and TypeScript/runtime consumer contracts remain unchanged.
- [x] Quality gates defined with measurable checks for deterministic behavior, integration paths, and failure handling. Each mergeable slice below names focused CTest targets plus a broad native sweep.
- [x] Reproducible delivery path identified for target runtime channels and evidence artifacts listed for release validation. Native validation runs via existing CMake/CTest entry points in `out/v3-native`, with artifact capture under `artifacts/native/`.
- [x] Constitution 1.3.0 concern-split rule acknowledged. In-scope native implementation files must either split into smaller units, become thin coordinators, or be logged as explicit temporary exceptions; only `CMakeLists.txt` retains standing large-file exception status.

**Post-Design Re-check**: Pass. The design artifacts below preserve ABI, keep runtime channels unchanged, and convert the remaining work into test-backed native-only slices.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/005-native-runtime-render-modularization/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   ├── runtime-render-boundary-contract.md
│   └── validation-gates.md
└── tasks.md
```

### Source Code (repository root)

```text
src/native/
├── CMakeLists.txt
└── engine/
    ├── CMakeLists.txt
    ├── runtime/
    │   ├── engine_composition.c
    │   ├── engine_tick.c
    │   ├── tick_input_phase.c
    │   ├── tick_budget_policy.c
    │   ├── tick_post_phase.c
    │   └── orchestration/
    ├── render/
    │   ├── backend_dx12.c
    │   ├── backend_dx12_projection_policy.c
    │   └── renderer.c
    └── win32_dx12_poc/
        ├── main.c
        ├── objective_policy.c
        └── scene_flow.c

tests/native/
├── runtime_engine_tick_test.c
├── runtime_tick_input_phase_test.c
├── runtime_tick_budget_policy_test.c
├── runtime_tick_post_phase_test.c
├── runtime_architecture_dependency_guard_test.c
├── engine_dx12_runtime_smoke_test.c
├── dx12_projection_policy_test.c
├── dx12_scene_flow_test.c
├── dx12_objective_policy_test.c
└── dx12_gameplay_lane_test.c
```

**Structure Decision**: This feature remains entirely within the native engine domain. Implementation work is confined to runtime orchestration, render backend internals, DX12 POC host files, and focused native tests plus feature-local planning artifacts.

## Storefront Deliverables

Not applicable. This feature does not change public Steam copy, store assets, release checklists, or runtime disclosure surfaces.

## Slice Plan

| Slice | Goal | Primary Files | Focused Merge Gate |
|-------|------|---------------|--------------------|
| S1 | Finish composition-root decomposition so `engine_composition.c` only wires context and lifecycle entry points | `src/native/engine/runtime/engine_composition.c`, new/existing lifecycle/bootstrap helpers, adjacent tests | `cmake --build out/v3-native` and `ctest --test-dir out/v3-native -C Debug -R "banana_runtime_engine_(host|lifecycle|state)_test|banana_runtime_engine_tick_test|banana_runtime_architecture_dependency_guard_test"` |
| S2 | Complete tick-flow modularization by extracting any remaining pre-gameplay sequencing into named runtime phases or orchestration helpers without reopening completed input/budget/post seams | `src/native/engine/runtime/engine_tick.c`, new `runtime/tick_*` or `runtime/orchestration/*` helpers, tick-focused tests | `ctest --test-dir out/v3-native -C Debug -R "banana_runtime_engine_tick_test|banana_runtime_tick_input_phase_test|banana_runtime_tick_budget_policy_test|banana_runtime_tick_post_phase_test|banana_engine_input_contract_transition_test"` |
| S3 | Split DX12 backend diagnostics and transport concerns into explicit backend services with ownership-aligned telemetry and failure surfaces | `src/native/engine/render/backend_dx12.c`, new DX12 helper modules, DX12 tests | `ctest --test-dir out/v3-native -C Debug -R "banana_engine_dx12_runtime_smoke_test|banana_dx12_projection_policy_test|banana_dx12_scene_overlay_frame_test" --output-on-failure` |
| S4 | Shrink the Win32 DX12 POC host so `main.c` becomes a thin runtime shell over objective/scene policies and explicit startup diagnostics | `src/native/engine/win32_dx12_poc/main.c`, POC policy helpers, POC smoke/lane tests | `ctest --test-dir out/v3-native -C Debug -R "banana_engine_win32_dx12_poc_startup_smoke|banana_dx12_objective_policy_test|banana_dx12_scene_flow_test|banana_dx12_gameplay_lane_test|banana_dx12_gameplay_click_lane_test" --output-on-failure` |
| S5 | Harden governance so concern splits and dependency direction are enforced instead of documented only | `tests/native/runtime_architecture_dependency_guard_test.c`, any supporting guard/checklist artifacts, possibly CMake wiring for new tests | `ctest --test-dir out/v3-native -C Debug -R "banana_runtime_architecture_dependency_guard_test|banana_runtime_engine_tick_test|banana_engine_dx12_runtime_smoke_test" --output-on-failure` |

## Validation Strategy

### Focused gates per slice

1. Build the native tree from the existing configured output directory: `cmake --build out/v3-native`.
2. Run the focused CTest regex for the slice being changed.
3. When a slice touches DX12-specific code, include the Windows-only DX12 smoke and POC tests listed in the slice gate.
4. Archive evidence to `artifacts/native/ctest-debug-<date>-<slice>.txt` after each slice-level validation pass.

### Broad gate before closing the feature

1. `ctest --test-dir out/v3-native -C Debug --output-on-failure`
2. Confirm no public ABI signature changes were introduced in `src/native/include/`.
3. Update the concern-split checklist / exception ledger for every touched oversized file.
4. Re-run the architecture dependency guard after any new runtime/render file is introduced.

## Concern-Split Governance

The feature closes only when each in-scope oversized implementation file has one of the following outcomes recorded in feature artifacts:

1. Split into smaller single-responsibility files with matching targeted tests.
2. Reduced to a thin coordinator over named policies/services.
3. Marked as an explicit temporary exception with owner, reason, and a follow-up slice.

The initial exception candidates identified during planning are `src/native/engine/render/backend_dx12.c`, `src/native/engine/win32_dx12_poc/main.c`, and secondarily `src/native/engine/render/renderer.c` if DX12 extraction reveals additional backend-agnostic seams.

## Risks And Mitigations

| Risk | Why It Matters | Mitigation |
|------|----------------|------------|
| Reopening completed tick seams | Could waste current branch progress and increase merge risk | Treat `tick_input_phase`, `tick_budget_policy`, and `tick_post_phase` as baseline; only extract what remains around them |
| DX12 modularization changes observable behavior | Backend fallback and camera/projection behavior are regression-prone | Keep smoke, projection, overlay, and gameplay-lane tests green for every DX12 slice |
| Guardrails remain too weak | Documentation alone will not prevent dependency drift | Expand architecture guard coverage and keep checklist evidence in-repo with each slice |
| Hidden ABI drift during internal refactor | Native consumers depend on stable exported signatures | Keep edits inside runtime/render internals and explicitly check `src/native/include/` before closeout |

## Complexity Tracking

No constitution violations or exceptional complexity justifications are required at planning time.

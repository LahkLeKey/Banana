# Research: Native Runtime Render Modularization

## Decision 1: Treat the current phase-13 `engine_tick` seams as the baseline, not as open design work

**Decision**: Plan from the already-landed `tick_input_phase`, `tick_budget_policy`, and `tick_post_phase` seams instead of scheduling a fresh tick split.

**Rationale**: `runtime/engine_tick.c` is already a small coordinator over those modules, so reopening that work would create churn without reducing the real remaining hotspots.

**Alternatives considered**: Re-plan `engine_tick.c` from scratch. Rejected because it ignores current branch progress and does not improve the highest remaining risk surfaces.

## Decision 2: Prioritize remaining decomposition by file responsibility and size, not by subsystem label alone

**Decision**: Sequence remaining work around the composition root and the largest DX12 files: `runtime/engine_composition.c`, `render/backend_dx12.c`, and `win32_dx12_poc/main.c`.

**Rationale**: Current line counts and function inventory show orchestration files are already small, while DX12 backend and POC host files still aggregate multiple concerns and remain the highest regression risk.

**Alternatives considered**: Spread work evenly across runtime, render, and test files. Rejected because it would dilute each slice and make merge reviews less falsifiable.

## Decision 3: Preserve ABI by constraining refactors to internal runtime/render seams

**Decision**: Keep all modularization behind existing runtime/render internal entry points and avoid changing public headers under `src/native/include/`.

**Rationale**: The feature spec requires ABI stability, and current composition and backend files already provide internal seams where responsibility can be moved safely.

**Alternatives considered**: Introduce new public ABI entry points for each decomposed service. Rejected because it expands contract surface and violates the incremental-refactor constraint.

## Decision 4: Use existing CMake + CTest entry points as the planning contract

**Decision**: Define slice gates in terms of `cmake --build out/v3-native` and focused `ctest --test-dir out/v3-native -C Debug -R ...` commands.

**Rationale**: The repo already registers focused tests for runtime tick, DX12 smoke/projection, POC policy flow, and architecture guard behavior, so the plan can reuse stable validation surfaces instead of inventing new orchestration.

**Alternatives considered**: Add a new planning-only validation wrapper script. Rejected because the existing build and test surfaces are sufficient and easier to keep aligned with implementation.

## Decision 5: Strengthen governance with in-repo guardrails plus evidence, not documentation alone

**Decision**: Pair any remaining decomposition with expanded architecture guard coverage and a concern-split checklist / exception ledger recorded in feature artifacts.

**Rationale**: The current architecture guard proves the repo already accepts automated dependency-direction enforcement; extending it is lower risk than adopting a new static-analysis stack mid-refactor.

**Alternatives considered**: Rely only on code review guidance, or introduce a full parser-based architecture linter immediately. The first is too weak to hold boundaries, and the second is too large for this incremental feature.

## Decision 6: Keep DX12 diagnostics ownership explicit at the backend boundary

**Decision**: Break DX12 work into transport/probe, telemetry/status, scene submission, and POC host concerns while keeping `backend_dx12.c` behaviorally equivalent during each slice.

**Rationale**: The backend currently mixes probe lifecycle, GPU resource management, projection fallback, overlay handling, and telemetry formatting, which obscures failure ownership.

**Alternatives considered**: Leave DX12 monolithic until after all runtime work finishes. Rejected because the backend is one of the largest remaining single files and directly called out by the feature scope.
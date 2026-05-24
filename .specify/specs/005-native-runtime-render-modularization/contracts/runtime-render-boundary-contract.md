# Contract: Runtime Render Boundary

## Intent

Keep native runtime modularization aligned with explicit dependency direction and ownership boundaries while preserving the public ABI.

## Layer Contract

| Layer | Allowed To Depend On | Must Not Depend On |
|-------|----------------------|--------------------|
| Composition root | Runtime orchestration, lifecycle helpers, backend selection/status helpers | Render transport internals beyond explicit adapter interfaces |
| Runtime orchestration | Policy services, application service ports, phase helpers | Render/UI adapter internals and reverse orchestration references |
| Runtime policy/domain services | Domain helpers, explicit runtime ports | Orchestration modules |
| Render adapters | Render helpers, platform APIs, backend-local diagnostics helpers | Runtime orchestration modules |
| Win32 DX12 POC host | Explicit scene/objective policies and engine/runtime entry points | Ad hoc gameplay logic spread across startup flow |

## ABI Preservation Rules

1. Public headers under `src/native/include/` remain unchanged unless a separately approved breaking change exists.
2. Modularization must occur behind existing runtime/render internal entry points.
3. New helper files may be added, but exported signatures must remain source-compatible for current consumers.

## Concern-Split Rules

1. Native C implementation files should own one primary concern.
2. `CMakeLists.txt` files are the only planned large-file exception by default.
3. Any non-`CMakeLists.txt` exception must name the owner, reason, and next slice that will reduce it.

## Diagnostics Ownership Rules

1. DX12 probe/bootstrap, transport lifecycle, scene submission, and telemetry/status formatting should move toward separate owning helpers.
2. Diagnostic strings must stay attributable to a single backend boundary after refactor.
3. Failure-path validation is required whenever backend status emission changes.

## Guardrail Enforcement

The minimum enforcement surface for this feature is:

1. `banana_runtime_architecture_dependency_guard_test`
2. Focused runtime tick tests for phase sequencing
3. Focused DX12 smoke/projection/POC tests when backend or host files move

If a new seam is introduced without one of those guardrails, the slice is incomplete.
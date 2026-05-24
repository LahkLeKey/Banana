# Contract: Native Runtime Render Boundaries

## Intent

Preserve the controller -> service -> pipeline -> native interop direction while keeping native implementation files small, explicit, and easy to split further.

## Dependency Direction

| Layer | Allowed Dependencies | Forbidden Dependencies |
|------|-----------------------|------------------------|
| Composition root | Runtime orchestration, lifecycle helpers, service-port providers | Render transport internals beyond explicit adapter seams |
| Runtime orchestration | Runtime phase helpers, domain/policy services, application service ports | Render/UI internals and reverse orchestration imports |
| Runtime policy/domain services | Typed value objects, service ports, domain helpers | Orchestration modules |
| Render adapters | Render-local helpers, platform APIs, backend diagnostics helpers | Runtime orchestration modules |
| Win32 DX12 POC host | Explicit policy/helper modules and stable engine entry points | Ad hoc gameplay logic embedded in startup shell |

## Concern-Split Rules

1. Native `.c` files should own one primary concern.
2. Thin coordinators are acceptable when they delegate to named helpers and remain easy to review.
3. `CMakeLists.txt` is the only standing large-file exception.
4. Any non-`CMakeLists.txt` exception must document owner, rationale, and next split slice in the feature checklist or native refactor ledger.

## ABI Preservation

1. Public headers under `src/native/include/` remain unchanged unless a separately approved ABI change exists.
2. New files must stay behind existing exported entry points.
3. Validation must include focused tests for any seam introduced by modularization.

## Guard Requirements

At minimum, the following must stay green when these boundaries move:

1. `banana_runtime_architecture_dependency_guard_test`
2. `banana_runtime_engine_tick_test`
3. DX12-focused smoke/projection/POC tests for touched render paths

## File-Size Governance

When a touched native file is still too large after a slice, record one of:

1. Split completed
2. Reduced to coordinator
3. Temporary exception with follow-up

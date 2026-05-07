# 007 DAL Runtime Contract Hardening

This feature slice hardens DAL runtime outcome behavior for profile-query paths.

## Scope

- Eliminate synthetic success outcomes for in-scope DAL non-success paths.
- Keep native-to-managed status mapping deterministic.
- Preserve success-path compatibility.

## Primary Implementation Surfaces

- `src/native/core/dal/banana_dal.c`
- `src/native/core/banana_status.c`
- `src/native/wrapper/banana_wrapper.h`
- `src/c-sharp/asp.net/Pipeline/StatusMapping.cs`
- `src/c-sharp/asp.net/NativeInterop/NativeStatusCode.cs`

## Required Validation Lanes

- `tests/native`
- targeted `tests/unit`
- `tests/e2e` contract lane (`FullyQualifiedName~Contracts`)

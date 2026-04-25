# Contract — Native Wrapper ABI

The native wrapper ABI is the public seam between v2 native and managed
consumers (primarily `007-aspnet-pipeline`).

## Stability rules

- Public symbol names in `banana_wrapper.h` are stable. Adding new symbols is allowed; renaming or removing is a breaking change.
- `NativeStatusCode` numeric values are stable. Reusing a value for new semantics is forbidden.
- Calling convention is platform default for shared libraries (`__cdecl` on Windows).

## Memory ownership

Each export documents one of:

- **Caller-owned out buffer**: caller passes pointer + capacity; callee writes; on overflow, returns a typed "buffer-too-small" status with required size.
- **Callee-owned returned buffer**: callee allocates; caller releases via the matching `banana_*_free` function exported alongside.

No mixed ownership inside a single export.

## Error reporting

- All fallible exports return `NativeStatusCode`.
- Detail strings (if any) are written to a caller-owned out buffer per the same rules above.
- No `errno`, no thread-local last-error, no silent zero-status with embedded error.

## DAL gating

- DAL exports check `BANANA_PG_CONNECTION` at entry.
- If unset, return `NativeStatusCode::ConfigMissing` (or v2 equivalent) with a remediation message in the optional out buffer.
- No silent skip; tests must assert this typed failure.

## Versioning

- A monotonic numeric `BANANA_WRAPPER_ABI_VERSION` constant is exported.
- Adding symbols increments minor; breaking changes require a major bump and a parent-spec update.

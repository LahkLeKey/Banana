# Feature Specification: Native Core (v2)

**Feature Branch**: `006-native-core`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`005-v2-regeneration`](../005-v2-regeneration/spec.md)  
**Baseline**: [`legacy-baseline/native-core.md`](../../legacy-baseline/native-core.md)

## Summary

Regenerate the native C layer (`src/native/{core,wrapper}` + DAL + tests) as a
small, well-typed surface that ASP.NET (`007`) can consume via P/Invoke.
Collapse speculative domain modules, unify memory-ownership and error
reporting, and keep the wrapper ABI numerically stable for managed consumers.

## User Scenarios & Testing

### User Story 1 — Stable wrapper ABI (Priority: P1)

As `007-aspnet-pipeline`, I can call the v2 wrapper without changing my
P/Invoke signatures or my `NativeStatusCode` numeric values.

**Independent Test**: Build v2 native, run existing managed integration tests; status codes match.

**Acceptance Scenarios**:

1. **Given** v2 native built, **When** managed `NativeBananaClient` calls each exported symbol, **Then** return values and status codes match v1 numeric semantics.
2. **Given** a domain function added in v2, **When** it returns a buffer, **Then** memory ownership is documented in `banana_wrapper.h` and matches the wrapper convention.

### User Story 2 — Lean core domain (Priority: P1)

As a maintainer, the v2 core contains only domain modules with explicit
consumers; speculative modules are removed or quarantined.

**Independent Test**: Module count and `.h`/`.c` LOC drop ≥30% versus v1; every remaining module has at least one direct consumer (managed code or another core module).

### User Story 3 — Unified error reporting (Priority: P2)

As a caller (managed or native), every fallible function reports errors via a
single typed status enum; no parallel `errno`-style or string fields.

**Independent Test**: `grep` for status patterns in `wrapper/` and `core/` shows one canonical enum and one canonical reporting function.

### User Story 4 — DAL gated cleanly (Priority: P2)

As a caller, DAL paths fail loudly with a remediation message when
`BANANA_PG_CONNECTION` is unset, instead of silently skipping or hanging.

**Acceptance Scenarios**:

1. **Given** `BANANA_PG_CONNECTION` unset, **When** a DAL function runs in tests, **Then** it returns a typed "config-missing" status and a remediation hint, and the test fails (not skips) unless explicitly marked as gated.

### Edge Cases

- A v1 module is genuinely needed but underused → keep, mark "low-traffic" in v2 plan.
- A v1 module has no consumer → drop, record in plan complexity table.
- A status code value collision between v1 and v2 → forbidden; bump to a new value and document.

## Requirements

### Functional Requirements

- **FR-001**: v2 MUST preserve every `NativeStatusCode` numeric value used by `007`'s `NativeStatusCode` enum.
- **FR-002**: v2 MUST preserve every public symbol name exported in `banana_wrapper.h` that has a managed consumer.
- **FR-003**: v2 MUST consolidate domain modules to those with at least one direct consumer.
- **FR-004**: v2 MUST adopt one error-reporting pattern across `core/` and `wrapper/`.
- **FR-005**: v2 MUST document buffer ownership in every wrapper export (caller-owned vs callee-owned).
- **FR-006**: v2 MUST keep `BANANA_PG_CONNECTION` and `BANANA_NATIVE_PATH` env contracts.
- **FR-007**: v2 MUST keep CMake target name `banana_wrapper` and Visual Studio 17 2022 / x64 Release build path on Windows.
- **FR-008**: Native tests under `tests/native/` MUST cover ABI surface end-to-end.

### Hard contracts to preserve

- `NativeStatusCode` numeric values (consumed by `007`).
- Public symbols in `banana_wrapper.h` consumed by managed interop.
- `BANANA_PG_CONNECTION`, `BANANA_NATIVE_PATH` env vars.
- CMake target name `banana_wrapper`.
- Cobertura native coverage path used by `014-test-coverage`.

### Pain points addressed (from baseline)

- Speculative domain modules → consolidated (FR-003).
- Mixed memory ownership → unified (FR-005).
- Fragmented error reporting → unified (FR-004).
- Silent DAL skip on unset PG conn → typed failure (User Story 4).

### Pain points deferred

- Header fan-out and recompilation cost → revisit after consolidation lands.
- `generated/` provenance → tracked separately under `013-ai-orchestration`.

### Key Entities

- **Wrapper export**: a public C function with documented ownership and status semantics.
- **Domain module**: a paired `.c/.h` with at least one consumer.
- **Status code**: enum value with stable numeric mapping.

## Success Criteria

- **SC-001**: Managed `NativeBananaClient` builds and tests pass against v2 native unchanged.
- **SC-002**: Core module count drops ≥30%.
- **SC-003**: One canonical error-reporting function across wrapper/core.
- **SC-004**: Native tests cover every exported wrapper symbol.

## Assumptions

- v2 native lives alongside v1 during cutover; `007` switches via build configuration.
- PostgreSQL schema changes (if any) coordinate with `008-typescript-api` to avoid collision.

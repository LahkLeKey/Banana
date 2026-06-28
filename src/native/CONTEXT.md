# Native C ABI Context

## Purpose

Define the public C ABI boundary and compatibility rules for native integrations.

## In scope

- Exported C headers and public symbols
- FFI payload layouts and memory ownership contracts
- ABI compatibility and versioning guarantees

## Out of scope

- Internal engine-only algorithms that are not part of the public ABI

## Ubiquitous language

- ABI: Application Binary Interface boundary consumed by non-C callers via FFI
- Contract: Stable expectation for function signatures, struct layouts, and ownership

## Core invariants

- Public ABI changes must be intentional and reviewed for compatibility impact
- Ownership and lifetime rules must be explicit for every pointer-bearing contract
- `banana_native` remains a wrapper/composition boundary and does not directly own extracted module internals
- Consumers should link native module targets and include public module headers instead of compiling internal module sources
- Public ABI install path remains stable (`${CMAKE_INSTALL_INCLUDEDIR}`) unless an approved ABI version bump is performed
- Public ABI header set (`banana_native_v3.h`, `banana_native_ui_abi.h`) remains frozen unless an approved ABI version bump is performed

## ABI change process

1. Propose the ABI change with explicit compatibility impact and migration notes.
2. Record the decision in ADR/context and include install/header contract deltas.
3. Bump ABI versioning signals (for example `banana_native_v3_abi_version` and any relevant contract version constants).
4. Run native contract checks and smoke tests before merge.
5. Keep install destination and header exports unchanged unless the approved versioning decision requires it.

## Key seams

- API service adapters that call into native via FFI
- Shared contract definitions consumed by TypeScript bindings
- Wrapper composition seam: `banana_native` links `banana_engine_core` and module targets while only compiling wrapper-owned entry translation units

## Open questions

- None yet

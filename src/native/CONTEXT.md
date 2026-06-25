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

## Key seams

- API service adapters that call into native via FFI
- Shared contract definitions consumed by TypeScript bindings

## Open questions

- None yet

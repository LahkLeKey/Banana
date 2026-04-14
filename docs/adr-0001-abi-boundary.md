# ADR-0001: Stable C ABI Wrapper Boundary

## Status

Accepted

## Context

The system contains legacy C logic that is stable and business-critical.
Directly exposing legacy functions to higher-level runtimes couples callers to historical implementation details.

## Decision

1. Keep legacy code intact and unmodified in `src/native/legacy`.
2. Introduce `src/native/wrapper` as the only exported ABI.
3. Use primitive parameters and fixed-layout structs for interop.
4. Standardize wrapper return statuses for deterministic error translation.
5. Standardize allocation/free via `cinterop_create_banana_message` and `cinterop_free`.

## Consequences

- Legacy logic is preserved and insulated.
- Managed and Node callers consume a narrow, stable contract.
- Future wrapper changes can remain backward compatible while legacy evolves internally.

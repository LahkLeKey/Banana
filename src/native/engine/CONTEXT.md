# Native Engine Runtime Context

## Purpose

Define native runtime behavior and engine internals independent of the public C ABI surface.

## In scope

- Engine internals and runtime orchestration
- Model/runtime integration behavior in native code
- Internal serialization and state transition behavior

## Out of scope

- Public API/service HTTP behavior
- Client UX concerns

## Ubiquitous language

- Runtime: Native execution environment and lifecycle
- Engine state: Internal mutable state driving native behavior

## Core invariants

- Runtime transitions should preserve internal consistency guarantees
- Internal changes should not silently break declared ABI contracts

## Key seams

- Public C ABI wrapper layer
- Upstream callers in API-side native bindings

## Open questions

- None yet

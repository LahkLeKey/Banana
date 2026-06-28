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
- Native module: A bounded native code unit with one public include and link surface
- Native math foundation: Shared deterministic numeric primitives module (Q16 fixed-point first) consumed by higher native modules
- Native runtime kernel: Shared runtime infrastructure primitives (parallel contract/model first) consumed by engine and tests through one link target
- Module DAG: Strict one-way dependency graph between native modules with no cycles
- Public module boundary: Target-based link plus module-prefixed public headers as the only supported consumer surface
- Phase gate: Required validation bundle for one module-extraction phase (build, boundary compliance, targeted tests, smoke checks)
- Phased one-shot extraction: Program-level incremental migration where each module move is one-shot within its phase

## Core invariants

- Runtime transitions should preserve internal consistency guarantees
- Internal changes should not silently break declared ABI contracts
- Native module dependencies must remain acyclic
- Shared deterministic math helpers must be owned by native math foundation, not duplicated per feature module
- Shared runtime infrastructure helpers must be owned by native runtime kernel, not duplicated per feature module
- External consumers must not include module-internal source paths
- Each module-extraction phase must pass its phase gate before the next phase starts

## Key seams

- Public C ABI wrapper layer
- Upstream callers in API-side native bindings

## Open questions

- None yet

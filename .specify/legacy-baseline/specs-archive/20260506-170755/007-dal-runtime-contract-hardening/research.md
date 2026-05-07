# Research: DAL Runtime Contract Hardening

**Branch**: `feature/006-c-upstream-value-spike` | **Phase**: 0 | **Date**: 2026-04-26

## R-01: DAL outcome taxonomy for dependency-unavailable states

**Question**: How should DAL behavior signal missing/unavailable linkage without synthetic success payloads?

**Decision**: Treat dependency-unavailable DAL execution as non-success and map it deterministically through existing or narrowly extended status semantics, never via synthetic success JSON.

**Rationale**: Preserves operational correctness and avoids false-positive success signals.

**Alternatives considered**:
- Continue synthetic success payloads for non-linked DAL: rejected due ambiguity and operational risk.
- Collapse all failures to one generic status: rejected because it harms diagnostics.

## R-02: Cross-layer status mapping strategy

**Question**: Where should DAL failure-to-HTTP mapping remain authoritative?

**Decision**: Keep native status emission in native code and maintain translation authority in `src/c-sharp/asp.net/Pipeline/StatusMapping.cs`.

**Rationale**: Preserves layered contract boundaries and existing architecture.

**Alternatives considered**:
- Controller-specific ad hoc mapping: rejected due drift risk.
- Native-emitted HTTP semantics: rejected as layer violation.

## R-03: Backward compatibility on successful path

**Question**: What compatibility constraints apply to successful DAL query behavior?

**Decision**: Successful configured DAL query behavior remains shape-compatible with existing consumers.

**Rationale**: The story objective is hardening failure semantics, not changing success payload contracts.

**Alternatives considered**:
- Simultaneous success payload redesign: rejected as scope expansion.

## R-04: Minimal validation lane set

**Question**: What is the smallest sufficient validation surface for this slice?

**Decision**: Require `tests/native` for DAL behavior, targeted `tests/unit` for status/interops, and `tests/e2e --filter "FullyQualifiedName~Contracts"` for consumer contract assurance.

**Rationale**: Captures domain behavior, mapping correctness, and contract-lane impact with bounded execution cost.

**Alternatives considered**:
- Unit-only validation: rejected because it misses native behavior confidence.
- Full monorepo validation for every iteration: rejected as unnecessarily broad for this bounded slice.

## R-05: Runtime contract handling

**Question**: Which env/runtime contracts must remain explicit?

**Decision**: Preserve explicit handling of `BANANA_PG_CONNECTION` (DAL gating) and `BANANA_NATIVE_PATH` (ASP.NET native wiring) with deterministic remediation hints.

**Rationale**: These are existing Banana runtime contracts and central to diagnostics.

**Alternatives considered**:
- Implicit fallback behavior with reduced diagnostics: rejected due support and triage cost.

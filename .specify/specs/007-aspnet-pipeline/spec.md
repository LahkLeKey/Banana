# Feature Specification: ASP.NET Pipeline & Native Interop (v2)

**Feature Branch**: `007-aspnet-pipeline`  
**Created**: 2026-04-25  
**Status**: Draft  
**Parent**: [`005-v2-regeneration`](../005-v2-regeneration/spec.md)  
**Baseline**: [`legacy-baseline/aspnet-pipeline.md`](../../legacy-baseline/aspnet-pipeline.md)

## Summary

Regenerate the ASP.NET API as a thin controller layer plus an ordered pipeline
that calls v2 native interop. Collapse `*Native` shadow DTOs, replace the
string-typed pipeline context, and reduce the `INativeBananaClient` blast
radius so adding a method does not cascade through ~10 fakes.

## User Scenarios & Testing

### User Story 1 — Stable HTTP surface (Priority: P1)

As a frontend (React, Electron, RN), my existing HTTP calls keep working
during and after v2 cutover.

**Acceptance Scenarios**:

1. **Given** v1 HTTP route shapes, **When** v2 lands, **Then** routes, request bodies, and response shapes match unless a documented breaking change is announced in this spec.
2. **Given** a frontend running against `VITE_BANANA_API_BASE_URL`, **When** v2 is deployed, **Then** no client changes are required for stable endpoints.

### User Story 2 — Typed pipeline context (Priority: P1)

As a pipeline-step author, I read and write context fields by typed property,
not string keys, so missing fields are compile errors.

### User Story 3 — Single interop seam (Priority: P1)

As a test author, adding a method to the interop seam does not require editing 10+ fakes; v2 uses code-generated or compositional fakes.

### User Story 4 — One DTO per record (Priority: P2)

As a maintainer, each domain record has one DTO; managed/native mapping is
generated or trivial, not a parallel `*Native` type per record.

### Edge Cases

- A v1 route is genuinely wrong → v2 may break it but MUST list it under "Breaking changes" with frontend migration.
- A pipeline step depends on another via implicit context key → migration step turns this into a typed dependency.

## Requirements

### Functional Requirements

- **FR-001**: v2 MUST preserve every public HTTP route + response shape currently consumed by frontends.
- **FR-002**: v2 MUST replace the string-typed pipeline context with typed fields.
- **FR-003**: v2 MUST keep one interop seam (interface or successor) and reduce fake-update fan-out (e.g., generated fakes or default abstract base).
- **FR-004**: v2 MUST collapse `*Native` shadow DTOs in `NativeInterop/` to a single DTO per record.
- **FR-005**: v2 MUST keep `BANANA_NATIVE_PATH` env contract.
- **FR-006**: v2 MUST keep `coverage-denominator.json` location and format consumed by `014-test-coverage`.
- **FR-007**: v2 MUST translate `NativeStatusCode` to HTTP consistently and document the mapping.

### Hard contracts to preserve

- HTTP routes and response shapes consumed by `009`, `010`, `011`.
- `NativeStatusCode` numeric values (depend on `006`).
- `BANANA_NATIVE_PATH` env var.
- `coverage-denominator.json` path and format.

### Pain points addressed

- Controllers duplicating pipeline concerns → controllers thin (FR-002 indirectly).
- `*Native` DTO sprawl → collapsed (FR-004).
- String-typed context bag → typed (FR-002).
- Adding interop method causes CS0535 cascade → reduced (FR-003).

### Pain points deferred

- `DataAccess` vs `NativeInterop` ownership split → revisit after typed pipeline lands.
- Middleware ordering documentation → covered in plan, not full spec.

### Key Entities

- **Pipeline step**: typed unit of work with declared inputs/outputs.
- **Interop seam**: single interface (or successor) for native calls.
- **Domain DTO**: one type per record; mapping to native is generated or trivial.

## Success Criteria

- **SC-001**: Frontend smoke against v2 returns identical responses for stable endpoints.
- **SC-002**: Pipeline context has zero `string` key lookups.
- **SC-003**: Adding an interop method updates ≤2 files in `tests/`.
- **SC-004**: `*Native` DTO count drops to zero (or formally equals 1:1 mapping).
- **SC-005**: AI contract guard remains green.

## Assumptions

- v2 builds against v2 native (`006`); cutover happens behind a build flag.
- HTTP API remains the canonical Banana API; `008-typescript-api` covers separate scope (or is consolidated under `008`).

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
radius so adding a method does not cascade through ~10 fakes. Include explicit
contracts for banana/not-banana training-data consumption, model artifact
metadata, and chatbot classification parity with `008` so cross-slice behavior
is deterministic during cutover.

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

### User Story 5 — Training-data and model artifact parity (Priority: P1)

As an ML maintainer, ASP.NET-managed inference and metadata contracts stay
aligned with corpus/training artifacts so native + managed classification do
not drift.

**Acceptance Scenarios**:

1. **Given** trained artifacts generated from banana/not-banana corpus,
	**When** ASP.NET endpoints classify input, **Then** responses declare model
	source and threshold provenance.
2. **Given** artifact metadata and thresholds change, **When** API responses are
	checked, **Then** managed classification contracts remain aligned with `008`
	score/chat semantics.

### User Story 6 — Banana-vs-not-banana chatbot interoperability (Priority: P1)

As a chatbot consumer, ASP.NET exposes or coordinates banana/not-banana
classification behavior compatible with chatbot sessions so fallback/cutover
between `007` and `008` is predictable.

**Acceptance Scenarios**:

1. **Given** equivalent banana/not-banana prompts, **When** evaluated through
	`007` and `008` contracts, **Then** label + threshold semantics are
	compatible.
2. **Given** a transient `008` outage, **When** `007` fallback is enabled,
	**Then** chatbot classification remains available with typed degradation
	metadata.

### Edge Cases

- A v1 route is genuinely wrong → v2 may break it but MUST list it under "Breaking changes" with frontend migration.
- A pipeline step depends on another via implicit context key → migration step turns this into a typed dependency.
- Trained artifact missing or corrupt at runtime → ASP.NET returns typed fallback/degradation details; no silent default.
- Model threshold differs from request override → response must declare threshold source.
- 007/008 contract drift for banana-vs-not-banana chat semantics → blocked by drift guard before release.

## Requirements

### Functional Requirements

- **FR-001**: v2 MUST preserve every public HTTP route + response shape currently consumed by frontends.
- **FR-002**: v2 MUST replace the string-typed pipeline context with typed fields.
- **FR-003**: v2 MUST keep one interop seam (interface or successor) and reduce fake-update fan-out (e.g., generated fakes or default abstract base).
- **FR-004**: v2 MUST collapse `*Native` shadow DTOs in `NativeInterop/` to a single DTO per record.
- **FR-005**: v2 MUST keep `BANANA_NATIVE_PATH` env contract.
- **FR-006**: v2 MUST keep `coverage-denominator.json` location and format consumed by `014-test-coverage`.
- **FR-007**: v2 MUST translate `NativeStatusCode` to HTTP consistently and document the mapping.
- **FR-008**: v2 MUST define managed training-data contract expectations for banana/not-banana corpus artifacts consumed by runtime inference.
- **FR-009**: v2 MUST expose model metadata (`source`, `recommended_threshold`, generated timestamp/metrics where available) for banana/not-banana runtime.
- **FR-010**: v2 banana/not-banana classification responses MUST include `label`, `banana_score`, `threshold`, and `threshold_source` semantics compatible with `008`.
- **FR-011**: v2 MUST provide typed behavior for no-signal and artifact-missing cases (invalid argument or explicit fallback metadata), never silent banana defaults.
- **FR-012**: v2 MUST define chatbot interoperability requirements for banana/not-banana classification compatibility with `008` sessions/messages.
- **FR-013**: v2 MUST support deterministic duplicate-message semantics where chatbot message replay/idempotency is enabled.
- **FR-014**: v2 MUST participate in AI contract drift checks covering corpus inputs, training scripts, native runtime outputs, and managed API contracts.
- **FR-015**: Integration tests MUST verify banana/not-banana classification parity expectations between `007` and `008` contracts.

### Hard contracts to preserve

- HTTP routes and response shapes consumed by `009`, `010`, `011`.
- `NativeStatusCode` numeric values (depend on `006`).
- `BANANA_NATIVE_PATH` env var.
- `coverage-denominator.json` path and format.
- Model-artifact metadata schema used by managed + TypeScript banana/not-banana runtime.
- Threshold provenance fields shared with `008` (`threshold`, `threshold_source`, model source).
- Training data contract alignment with `data/not-banana/corpus.json` and training workflow outputs.

### Pain points addressed

- Controllers duplicating pipeline concerns → controllers thin (FR-002 indirectly).
- `*Native` DTO sprawl → collapsed (FR-004).
- String-typed context bag → typed (FR-002).
- Adding interop method causes CS0535 cascade → reduced (FR-003).
- Training-data and runtime classifier expectations fragmented across native/managed/TS layers → explicit managed contract (FR-008 through FR-015).
- Chatbot banana-vs-not-banana fallback semantics undefined across `007`/`008` → explicit interoperability requirements (FR-012 through FR-015).

### Pain points deferred

- `DataAccess` vs `NativeInterop` ownership split → revisit after typed pipeline lands.
- Middleware ordering documentation → covered in plan, not full spec.

### Key Entities

- **Pipeline step**: typed unit of work with declared inputs/outputs.
- **Interop seam**: single interface (or successor) for native calls.
- **Domain DTO**: one type per record; mapping to native is generated or trivial.
- **Model artifact metadata**: runtime payload describing classifier source,
	threshold recommendations, and metrics provenance.
- **Classifier decision contract**: `{label, banana_score, threshold,
	threshold_source, model metadata}`.
- **Chat interoperability contract**: banana-vs-not-banana classification rules
	shared between ASP.NET and TypeScript chat experiences.

## Success Criteria

- **SC-001**: Frontend smoke against v2 returns identical responses for stable endpoints.
- **SC-002**: Pipeline context has zero `string` key lookups.
- **SC-003**: Adding an interop method updates ≤2 files in `tests/`.
- **SC-004**: `*Native` DTO count drops to zero (or formally equals 1:1 mapping).
- **SC-005**: AI contract guard remains green.
- **SC-006**: ASP.NET banana/not-banana responses expose threshold provenance and model source metadata.
- **SC-007**: No-signal and artifact-missing banana/not-banana requests produce typed, test-covered responses.
- **SC-008**: Cross-slice parity checks confirm `007` and `008` banana/not-banana semantics stay compatible for agreed prompts.
- **SC-009**: Drift guard fails CI when corpus/training/runtime/API contracts diverge.
- **SC-010**: Chatbot replay/idempotency interoperability behavior is defined and verified under fallback/cutover scenarios.

## Assumptions

- v2 builds against v2 native (`006`); cutover happens behind a build flag.
- HTTP API remains the canonical Banana API; `008-typescript-api` covers
	TypeScript-owned routes while `007` carries interoperability contracts for
	banana/not-banana training and chatbot semantics.

# Feature Specification: Full Brain Transformer Runnable C Code

**Feature Branch**: `[012-full-brain-transformer-runnable]`
**Created**: 2026-04-26
**Status**: Draft
**Source**: 009-ml-brain-domain-spike, [readiness-packet-full-brain.md](../009-ml-brain-domain-spike/analysis/readiness-packet-full-brain.md)
**Input**: User description: "drive Full Brain transformer scaffold into runnable C code with input format contract, embedding assertion test pattern, and attention-map logging"
## Problem Statement

Feature Specification: Full Brain Transformer Runnable C Code aims to improve system capability and user experience by implementing the feature described in the specification.


## In Scope *(mandatory)*

- Define and lock the Full Brain input format contract (context token schema): how Left Brain regression scores and Right Brain binary decisions are encoded into the transformer input under `src/native/core/domain/ml/transformer/`.
- Implement deterministic seed handling for the existing `banana_ml_transformer_classify` so identical inputs produce identical outputs across repeated calls.
- Add an attention-map logging hook (build-time or runtime-toggle) that emits attention weights to a structured buffer for explainability.
- Develop a new native test pattern for embedding-level assertions (existing scalar `assert(score >= 0.0)` patterns are insufficient): vector dimension assertion, vector norm bounds, element finite-value check.
- Add native lane tests for: deterministic output across repeated calls (idempotence), input format contract (well-formed context token block), null-pointer rejection, attention-map non-null when logging enabled.
- Document the input format contract and embedding output shape in `banana_ml_transformer.h` and `banana_wrapper_ml_transformer.h`.
- Register a dedicated `banana_test_ml_transformer` ctest target in `tests/native/CMakeLists.txt`.

## Out of Scope *(mandatory)*

- Left Brain regression work (covered by 010).
- Right Brain binary work (covered by 011).
- Production deployment of the transformer.
- ASP.NET API surface for transformer outputs (downstream slice once contracts here are pinned).
- Quantized inference benchmarking — recorded as residual U-001 follow-up.
- Real attention weights from a trained model — this slice keeps deterministic stub weights and validates the logging path, not model quality.
- Cross-domain integration smoke that runs Left -> Right -> Full end-to-end in a single test (downstream slice).

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Locked Input Format Contract (Priority: P1) MVP

As a Banana native author, I need a stable context-token schema for Full Brain inputs so Left Brain scores and Right Brain decisions can be passed in without surprise.

**Why this priority**: FB-R05 (input format must be standardized before implementation) was identified as the gating first deliverable in the readiness packet.

**Independent Test**: Inspect `banana_ml_transformer.h` for documented context token schema; run a contract test that constructs a context token block and exercises `banana_ml_transformer_classify`.

**Acceptance Scenarios**:

1. **Given** the transformer header, **When** read, **Then** it documents the context token schema (positions for Left score, Right label, raw feature vector slots).
2. **Given** a well-formed context token block, **When** classified, **Then** the call returns `BANANA_OK` and produces a non-null embedding/result buffer.
3. **Given** a NULL features pointer or NULL out_result pointer, **When** invoked, **Then** the call returns `BANANA_INVALID_ARGUMENT`.

---

### User Story 2 - Deterministic Seed Handling (Priority: P1)

As a Banana native author, I need identical inputs to produce identical outputs across repeated calls so the transformer can be unit-tested at all.

**Why this priority**: The existing smoke test in `test_ml_models.c` already asserts `strcmp(transformer_json, transformer_json_second) == 0`. This slice promotes that to a first-class contract and adds a structured idempotence test.

**Independent Test**: Run the new `banana_test_ml_transformer` and confirm 5 repeated calls with the same input produce byte-identical JSON output.

**Acceptance Scenarios**:

1. **Given** the same input invoked 5 times, **When** results are compared, **Then** all 5 outputs are byte-identical.
2. **Given** the same input invoked across two sequential test runs, **When** outputs are compared, **Then** they remain byte-identical (no time-based or PID-based randomness).

---

### User Story 3 - Embedding Assertion Test Pattern (Priority: P2)

As a Banana native author, I need a reusable embedding-level assertion helper because scalar assertion macros are insufficient for vector outputs.

**Why this priority**: FB-R04 (existing test patterns insufficient) was identified as a required deliverable in the readiness packet. Even with a stub embedding output, the assertion pattern is what unblocks future model upgrades.

**Independent Test**: Inspect `tests/native/test_ml_transformer.c` for a reusable helper such as `assert_embedding_valid(double* vec, size_t dim, double max_norm)` and confirm it covers dim, finite, norm bound.

**Acceptance Scenarios**:

1. **Given** a transformer output exposing an embedding vector (or fingerprint vector), **When** asserted, **Then** dimension matches the documented schema, all elements are finite, and L2 norm is below documented bound.
2. **Given** a NULL or zero-dim embedding, **When** asserted, **Then** the helper fails the test deterministically.

---

### User Story 4 - Attention-Map Logging Hook (Priority: P3)

As a Banana operator, I need an attention-map logging hook so explainability evidence is available when needed.

**Why this priority**: FB-R03 (explainability requires specialized tooling) was flagged in the readiness packet as in-scope from the start. A runtime-toggle hook (default off) keeps the cost out of the hot path while making the surface available.

**Independent Test**: Run `banana_test_ml_transformer` with attention logging enabled and confirm the attention buffer is populated; with logging disabled confirm no buffer is allocated.

**Acceptance Scenarios**:

1. **Given** attention logging is enabled (env var or compile flag), **When** classified, **Then** an attention buffer is populated with non-NaN weights summing close to 1.0 per attention head (or per-row for stub).
2. **Given** attention logging is disabled, **When** classified, **Then** no attention buffer is allocated and the standard JSON output is unchanged.

## Validation Lanes

| Lane | Required | Key Assertion |
|---|---|---|
| Native (`ctest --preset default`) | Yes | New `banana_test_ml_transformer` target passes (idempotence, embedding helper, null guard, attention toggle) |
| .NET Unit | Recommended | `INativeBananaClient` transformer signature stable |
| .NET E2E Contracts | Optional | HTTP surface deferred to downstream slice |
| Coverage | Yes | Transformer path contributes to 80% native threshold |

## Success Criteria

- `banana_test_ml_transformer` is registered, runs in `<2s`, and passes 100%.
- Transformer path coverage `>= 85%` line coverage.
- Input format context token schema documented in header.
- Embedding assertion helper exists and is reusable for future vector outputs.
- Attention-map toggle path is exercised in test and visibly emits weights when on.
- Idempotence verified across 5 repeated calls.

## Risks Inherited from SPIKE

| Risk | Mitigation in this slice |
|---|---|
| FB-R01 greenfield module | Build incrementally on existing scaffold; do not inherit Left/Right patterns uncritically |
| FB-R02 inference compute cost | Out-of-scope here; recorded as residual U-001 follow-up |
| FB-R03 explainability tooling | Attention-map logging hook in scope |
| FB-R04 existing test patterns insufficient | New embedding assertion helper required |
| FB-R05 input format unstandardized | Context token schema is first deliverable |
| U-001 quantization cost unknown | Out-of-scope; budget separate spec |

## Residual Follow-Ups

- Quantized inference evaluation (U-001) → separate spec when transformer body grows beyond stub.
- End-to-end Left -> Right -> Full integration smoke → separate spec once 010, 011, 012 are all green.
- ASP.NET HTTP surface for transformer output → separate spec.

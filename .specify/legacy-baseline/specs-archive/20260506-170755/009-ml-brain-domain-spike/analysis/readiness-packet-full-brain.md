# Readiness Packet: Full Brain Domain

**Domain**: Full Brain
**Model Family**: Transformer
**Date**: 2026-04-26
**Confidence**: High [H] (domain assignment); Medium [M] (implementation cost estimates — U-001)

---

## Objective

Implement and validate the Full Brain Transformer domain in Banana's native layer: an attention-based contextual synthesis model that accepts multi-signal inputs (including Left Brain scores and Right Brain decisions as context tokens), produces synthesized classifications or embeddings, and is exercised by a new native test pattern capable of embedding-level assertions.

---

## Scope Boundary

### In Scope (Follow-Up Slice)

- Native transformer model implementation or integration under `src/native/core/domain/ml/transformer/`.
- Wrapper ABI exports for Full Brain synthesis under `src/native/wrapper/domain/ml/transformer/`.
- Input format contract definition: how Left Brain scores and Right Brain decisions are encoded as context tokens.
- New native test pattern for embedding-level assertions (existing ctest assertion patterns are insufficient).
- Quantized inference evaluation sub-task to assess compute cost feasibility.
- Attention-map logging capability for explainability compliance.
- Documentation of Full Brain input/output contract.

### Out of Scope (Follow-Up Slice)

- Regression models (Left Brain domain).
- Binary models (Right Brain domain).
- Production deployment of the transformer (this slice is implementation and native validation only).
- ASP.NET API changes unless required to surface Full Brain synthesis outputs through HTTP (separate follow-up).

---

## Validation Lane Expectations

| Lane | Required | Key Assertions |
|---|---|---|
| Native (`ctest`) | Yes — new pattern required | Transformer produces output of expected type; attention-weight vector is non-null; synthesis output is stable across repeated inference calls with identical seed |
| .NET Unit | Yes | Signature stability of wrapper interop for Full Brain output; status code contract |
| .NET E2E Contracts | Optional in this slice | HTTP response shape stability for Full Brain output is a follow-up once API surface is defined |
| Coverage | Yes | Transformer path must contribute toward 80% coverage threshold |

**Note**: A new native test pattern must be developed for embedding assertions. Do not reuse the existing scalar assertion helpers from Left Brain tests — they are insufficient.

---

## Risk Items from Risk Register

| Risk ID | Risk | Mitigation |
|---|---|---|
| FB-R01 | Transformer not yet implemented — greenfield | Treat as a new module; budget accordingly; do not inherit Left/Right patterns uncritically |
| FB-R02 | Inference compute cost significantly higher | Evaluate quantized inference as first technical sub-task before full model implementation |
| FB-R03 | Explainability requires specialized tooling | Include attention-map logging in scope from the start — do not defer |
| FB-R04 | Existing native lane test patterns insufficient | Define new embedding assertion test pattern as a required deliverable of this slice |
| FB-R05 | Input format must be standardized before implementation | Define input format contract (context token schema) as first deliverable |

---

## First Deliverable

Define the Full Brain input format contract (context token schema for encoding Left Brain scores and Right Brain decisions) before implementing any model code — this contract gates both the implementation and the new native test pattern.

---

## Bounded Slice Summary

> Implement an attention-based transformer in the native C layer, starting with input format contract definition and compute-cost evaluation, then implement model + wrapper ABI + new embedding assertion test pattern. Attention-map logging is in scope. No production deployment or API surface changes in this slice.

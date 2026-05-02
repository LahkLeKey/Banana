# Implementation Plan: ML Brain Domain SPIKE

**Branch**: `009-ml-brain-domain-spike` | **Date**: 2026-04-26 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `.specify/specs/009-ml-brain-domain-spike/spec.md`

## Summary

Produce a planning-first SPIKE that formalizes Banana ML brain-domain roles (Left = Regression, Right = Binary, Full = Transformer), compares model-family tradeoffs with a normalized matrix, and outputs follow-up readiness packets per domain.

## Technical Context

**Language/Version**: Markdown planning artifacts with native C domain contract references
**Primary Dependencies**: Existing ML surfaces under `src/native/core/domain/ml/*`, wrappers under `src/native/wrapper/domain/ml/*`, and `banana_ml_models` contracts
**Storage**: N/A
**Testing**: Planning artifact review gates; native/unit/contract lane mapping definitions for follow-up stories
**Target Platform**: Banana monorepo planning workflow (Spec Kit)
**Project Type**: Discovery and prioritization SPIKE for ML domain strategy
**Performance Goals**: Enable one-session stakeholder selection of domain follow-up slices
**Constraints**: No production behavior changes in SPIKE; preserve existing native/wrapper contract boundaries
**Scale/Scope**: Three-domain framing across three model families with execution-ready follow-up recommendations

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Notes |
|-----------|--------|-------|
| I. Domain Core First | PASS | SPIKE anchored on native ML domain surfaces and contracts. |
| II. Layered Interop Contract | PASS | No interop behavior changes; contract implications documented only. |
| III. Spec First Delivery | PASS | Spec created before plan for feature 009. |
| IV. Verifiable Quality Gates | PASS | Follow-up lane expectations included as explicit outputs. |
| V. Documentation and Wiki Parity | PASS | Spec artifacts scoped under `.specify/specs/009-*`; no external workflow contract change required in this slice. |

**Post-design check (Phase 1)**: PASS. No constitution violations introduced by design artifacts.

## Project Structure

### Documentation (this feature)

```text
.specify/specs/009-ml-brain-domain-spike/
├── plan.md
├── research.md
├── data-model.md
├── quickstart.md
├── contracts/
│   └── ml-brain-domain-contract.md
└── tasks.md
```

### Source Code (repository root)

```text
src/native/core/domain/
├── banana_ml_models.c
├── banana_ml_models.h
└── ml/
    ├── regression/
    ├── binary/
    └── transformer/

src/native/wrapper/domain/ml/
├── regression/
├── binary/
└── transformer/

tests/native/
tests/unit/
tests/e2e/Contracts/
```

**Structure Decision**: Keep this slice documentation-first and evaluation-focused; follow-up implementation slices will be generated from SPIKE packets.

## Complexity Tracking

No constitution violations requiring justification.

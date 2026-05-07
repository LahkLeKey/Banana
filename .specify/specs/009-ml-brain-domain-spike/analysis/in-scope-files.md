# In-Scope Files: ML Brain Domain SPIKE

**Date**: 2026-04-26
**Purpose**: Enumerate the native and wrapper ML files in scope for this SPIKE's domain analysis.

## Scope Statement

This SPIKE is documentation-first and evaluation-focused. In-scope files are references only — no production code changes in this SPIKE.

## Native Core Domain Files

| Path | Purpose | Domain Relevance |
|---|---|---|
| `src/native/core/domain/banana_ml_models.c` | ML model runtime behavior | Primary ML domain surface |
| `src/native/core/domain/banana_ml_models.h` | ML domain headers and contracts | Contract boundary |
| `src/native/core/domain/ml/regression/` | Regression model implementation | Left Brain candidate |
| `src/native/core/domain/ml/binary/` | Binary model implementation | Right Brain candidate |
| `src/native/core/domain/ml/transformer/` | Transformer model implementation | Full Brain candidate |

## Native Wrapper Files

| Path | Purpose | Domain Relevance |
|---|---|---|
| `src/native/wrapper/domain/ml/regression/` | Regression wrapper ABI surface | Left Brain interop |
| `src/native/wrapper/domain/ml/binary/` | Binary wrapper ABI surface | Right Brain interop |
| `src/native/wrapper/domain/ml/transformer/` | Transformer wrapper ABI surface | Full Brain interop |

## Existing Core ML Contracts

| Path | Purpose | Domain Relevance |
|---|---|---|
| `src/native/core/banana_classify.c` | Classification entry point | Intersects Right Brain |
| `src/native/core/banana_ripeness.c` | Ripeness estimation | Intersects Left Brain |
| `src/native/core/banana_core.h` | Core header contracts | Shared dependency |

## Test Files (Reference Only)

| Path | Purpose |
|---|---|
| `tests/native/` | Native lane tests (used in readiness packet lane planning) |
| `tests/unit/` | .NET unit lane tests |
| `tests/e2e/Contracts/` | Contract stability tests |

## Out of Scope

- All non-ML native files (`banana_batch.c`, `banana_calc.c`, `banana_truck.c`, `banana_status.c`)
- ASP.NET pipeline layers (not changed in this SPIKE)
- React/Electron/mobile layers

## Notes

- Not all paths above may yet exist; readiness packets explicitly track whether follow-up implementation must create vs modify.
- Canonical source of truth for domain paths is this file during SPIKE execution.

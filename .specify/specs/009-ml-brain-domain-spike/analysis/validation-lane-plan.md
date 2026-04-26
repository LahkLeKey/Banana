# Validation Lane Plan: ML Brain Domain SPIKE

**Date**: 2026-04-26  
**Purpose**: Define lane expectations and contract implications for follow-up implementation slices per domain.

## Lane Ownership by Domain

| Domain | Native Lane | .NET Unit | .NET E2E | Coverage Gate |
|---|---|---|---|---|
| Left Brain (Regression) | ✅ Required | ✅ Required | Recommended | ✅ Yes |
| Right Brain (Binary) | ✅ Required | ✅ Required | Recommended | ✅ Yes |
| Full Brain (Transformer) | ✅ Required (new pattern) | ✅ Required | Optional in first slice | ✅ Yes |

---

## Native Lane Details

### Left Brain (Regression) — Native Lane

**Entry point**: `ctest --preset default-test`  
**New test file**: `tests/native/test_regression_left_brain.c`  
**Key assertions**:
1. Regression output is a float in the defined range (e.g., 0.0–1.0).
2. Input boundary vectors (all zeros, all max) produce valid outputs.
3. Out-of-range outputs are clamped at the wrapper boundary.

**Pattern**: Reuse existing native test helper patterns from `tests/native/test_dal_contracts.c`.

---

### Right Brain (Binary) — Native Lane

**Entry point**: `ctest --preset default-test`  
**New test file**: `tests/native/test_binary_right_brain.c`  
**Key assertions**:
1. Binary output is a valid class label from the defined class set.
2. Class probability is in [0.0, 1.0].
3. A known non-banana input produces is-banana=false (negative-sample test).
4. Operating point threshold produces expected class boundary behavior.

**Pattern**: Reuse existing native test helper patterns; add negative-sample vector fixture.

---

### Full Brain (Transformer) — Native Lane (New Pattern Required)

**Entry point**: `ctest --preset default-test`  
**New test file**: `tests/native/test_transformer_full_brain.c`  
**New assertion helpers**: Embedding vector assertion helpers (do not reuse scalar assertion helpers).  
**Key assertions**:
1. Transformer produces output of expected type (embedding vector or class label depending on configured head).
2. Attention-weight vector is non-null and sums to 1.0 per head.
3. Inference with identical inputs and seed produces identical outputs (determinism gate).
4. Attention-map logging does not crash or corrupt inference output.

**Pattern**: New pattern required. Design in follow-up Full Brain slice as first deliverable.

---

## .NET Unit Lane Details

All domain follow-up slices must include a signature stability test for new wrapper interop methods in `tests/unit/NativeBananaClientTests.cs` (or equivalent per-domain test file), following the pattern established in 007.

---

## Contract Impact Notes

| Contract | Impacted By | Change Type | Priority |
|---|---|---|---|
| `NativeBananaClient` P/Invoke surface | Left Brain, Right Brain, Full Brain follow-up | New method exports (additive) | Required in each slice |
| `StatusMapping.cs` | Left Brain, Right Brain | New status codes if regression/binary paths add non-success states | Required if new states added |
| HTTP response schema | Right Brain (is-banana) | Potentially extends current response with class probability | Evaluate in Right Brain slice |
| Embedding response format | Full Brain | New response type (embedding vector) if surfaced through API | Define in Full Brain API follow-up slice |

---

## Coverage Gate Requirements

All follow-up slices must:
1. Pass the existing coverage denominator guard: `bash scripts/check-banana-api-coverage-denominator.sh`
2. Meet or contribute toward the 80% coverage threshold enforced by the CI workflow.
3. Not regress existing lane pass rates.

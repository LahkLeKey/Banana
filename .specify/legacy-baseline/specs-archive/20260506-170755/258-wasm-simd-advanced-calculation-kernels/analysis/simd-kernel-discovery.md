# WASM SIMD Kernel Discovery & Design — Spec 258 Phase 1

**Date**: 2026-05-05
**Status**: Complete (T001–T008)
**Branch**: `feature/wasm-native-integration-spikes`

---

## T001 — Advanced Calculation Hotspot Inventory and SIMD Suitability

SIMD suitability is assessed against the Banana native calculation surface documented in spec 257 analysis.

### Hotspot Analysis

| Calculation Family | Source Module | Operation Character | SIMD Potential | Notes |
|---|---|---|---|---|
| **Regression scoring** | `banana_ml_regression.c` | Dot product of feature vector × weight vector over ~14 doubles (`BananaMlFeatureVector`) | **High** — 14-wide float64 dot product, tight inner loop | Tier 1 SIMD target |
| **Binary classification** | `banana_ml_binary.c` | Feature vector scoring + threshold compare, 2-class softmax | **High** — parallel score accumulation, vectorizable logit | Tier 1 SIMD target |
| **Transformer classify** | `banana_ml_transformer.c` | 4-dim embedding + 8-dim attention weights | **Medium** — small fixed dimensions; SIMD worth measuring for batch calls | Tier 2 candidate |
| **Calculation arithmetic** | `banana_calc.c` | Single multiply + overflow check (`int64_t product`) | **Low** — scalar saturates in 1–2 cycles; SIMD adds overhead for single call | Not a SIMD target |
| **Ripeness classification** | `banana_ripeness.c` | Calls `banana_ml_predict_regression_score` then threshold compare | **Derived** — SIMD benefit comes entirely from regression scoring sub-call | Inherits from regression |
| **Quantized embedding** | `_quant_embedding` export | int8 quantize 4 doubles → scale + zero-point | **Low-Medium** — too small to gain from SIMD in isolation, but batching exposes opportunity | Tier 2 candidate |

### SIMD Tier Classification

**Tier 1 — Primary SIMD targets** (clear throughput benefit, portable WASM SIMD primitives apply):
- `banana_ml_predict_regression_score` via `banana_ml_regression.c` — dot-product of 14 float64 features
- `banana_ml_classify_binary` via `banana_ml_binary.c` — parallel score accumulation for 2-class classifier

**Tier 2 — Measure first** (benefit depends on call frequency and data width):
- `banana_ml_classify_transformer` via `banana_ml_transformer.c` — 4-dim embedding + 8-dim attention
- Quantized embedding path — small vectors, SIMD benefit visible only under batch load

**Tier 3 — Scalar only** (SIMD adds overhead, no benefit):
- `banana_calculate_banana` / `banana_calculate_banana_with_breakdown` — pure integer, single call
- All stateful domains (batch/truck/harvest) — I/O-bound, no hot arithmetic

### SIMD Instruction Families Relevant to These Kernels

| Operation | WASM SIMD Instruction | Available Since |
|---|---|---|
| f64×2 multiply-add (dot product step) | `f64x2.mul`, `f64x2.add` | WASM SIMD MVP (2021) |
| f64×2 comparison (threshold) | `f64x2.gt`, `f64x2.lt` | WASM SIMD MVP |
| f32×4 multiply-add (if downcast to f32 acceptable) | `f32x4.mul`, `f32x4.add` | WASM SIMD MVP |
| i8×16 dot product (quantized embedding) | `i16x8.extmul_low_i8x16_s` + `i32x4.add` | WASM SIMD MVP |

All target instructions are in the WASM SIMD MVP, supported by Chromium ≥91, Firefox ≥90, Safari ≥16.4, and Node.js ≥16. Desktop (Electron ≥20) and React Native web-view (≥iOS 16.4, Android Chromium) support the same set.

---

## T002 — Representative Workload Shapes for Kernel Benchmarking

### Workload Sizes

| Size | Description | Input Shape | Call Count |
|---|---|---|---|
| **Small** | Single inference call (interactive UX path) | 1 JSON object, ~14 numeric features | 1 call |
| **Medium** | Batch quiz benchmark (6 prompts, CI gate) | 6 JSON objects sequentially | 6 calls |
| **Large** | Throughput stress (background bulk analysis) | 1 000 JSON objects, same feature distribution | 1 000 calls |

### Feature Vector Shape (for regression and binary kernels)

`BananaMlFeatureVector` has 14 numeric fields (8 size_t counts + 6 double ratios). For benchmarking, a fixed representative vector is used:

```json
{
  "token_count": 12,
  "unique_token_count": 9,
  "banana_hits": 3,
  "not_banana_hits": 1,
  "positive_bigram_hits": 2,
  "negative_bigram_hits": 0,
  "banana_signal_ratio": 0.25,
  "not_banana_signal_ratio": 0.083,
  "unique_token_ratio": 0.75,
  "length_ratio": 0.6,
  "positive_bigram_ratio": 0.167,
  "negative_bigram_ratio": 0.0,
  "banana_attention_ratio": 0.3,
  "not_banana_attention_ratio": 0.1
}
```

The benchmark harness wraps this in the expected JSON input string format.

---

## T003 — Kernel Catalog with SIMD/Scalar Pair Strategy

### Catalog

| Kernel ID | Function | SIMD Implementation | Scalar Fallback | Ownership |
|---|---|---|---|---|
| `KERN-001` | `banana_ml_predict_regression_score` | `banana_ml_regression_simd.c` (new) — uses `f64x2.mul`+`f64x2.add` unrolled dot product | Existing `banana_ml_regression.c` unchanged | `native-core-agent` |
| `KERN-002` | `banana_ml_classify_binary` | `banana_ml_binary_simd.c` (new) — parallel class score accumulation | Existing `banana_ml_binary.c` unchanged | `native-core-agent` |
| `KERN-003` | `banana_ml_classify_transformer` | Measured only in this spike; SIMD impl deferred to next slice if Tier 1 evidence justifies it | Existing `banana_ml_transformer.c` | `native-core-agent` |

### SIMD/Scalar Pair Strategy

Each kernel is compiled twice:
1. **Scalar build** (`-O3`, no SIMD flags): baseline measurement; guaranteed correctness.
2. **SIMD build** (`-O3 -msimd128`): Emscripten enables WASM SIMD; compiler auto-vectorizes tight loops.

At runtime, the host selects via capability detection (T004). Both builds are shipped in the WASM artifact as separate exported function variants:

```c
// Scalar variants (always exported)
banana_calculate_regression_score_scalar(...)
banana_classify_binary_scalar(...)

// SIMD variants (exported when built with -msimd128)
banana_calculate_regression_score_simd(...)
banana_classify_binary_simd(...)
```

The JS host wrapper dispatches to SIMD or scalar transparently.

---

## T004 — Capability Detection Contract and Fallback Behavior

### Detection Method

WASM SIMD support is detected at module init using the standard `WebAssembly.validate()` probe:

```typescript
const SIMD_PROBE_BYTES = new Uint8Array([
  0x00, 0x61, 0x73, 0x6d, // magic: \0asm
  0x01, 0x00, 0x00, 0x00, // version: 1
  0x01, 0x05, 0x01,       // type section: 1 entry
  0x60, 0x00, 0x01, 0x7b, // () -> v128 (SIMD return type)
  0x03, 0x02, 0x01, 0x00, // function section
  0x0a, 0x06, 0x01, 0x04, // code section: 1 function, 4 bytes
  0x00,                   // no locals
  0xfd, 0x0f,             // i32x4.splat (SIMD op)
  0x41, 0x00,             // i32.const 0
  0xfd, 0x0f,             // i32x4.splat
  0x0b,                   // end
]);

export function detectWasmSimd(): boolean {
  try {
    return WebAssembly.validate(SIMD_PROBE_BYTES);
  } catch {
    return false;
  }
}
```

### Dispatch Table

```typescript
export type KernelVariant = 'simd' | 'scalar';

export function selectKernelVariant(simdAvailable: boolean): KernelVariant {
  return simdAvailable ? 'simd' : 'scalar';
}

// Module initialization captures capability once
let _kernelVariant: KernelVariant | null = null;

export function getKernelVariant(): KernelVariant {
  if (_kernelVariant === null) {
    _kernelVariant = selectKernelVariant(detectWasmSimd());
  }
  return _kernelVariant;
}
```

### Fallback Invariants

1. **Never throw on SIMD unavailability** — silently dispatch to scalar variant.
2. **No user-visible difference** — scalar and SIMD produce numerically equivalent results within parity thresholds (T005).
3. **Capability is cached per module instance** — detection runs once at init, not per call.
4. **CI always tests both variants** — SIMD build runs in Chromium (SIMD available); scalar-forced run mocks `WebAssembly.validate` to return `false`.

---

## T005 — Numeric Parity Policy

### Policy Statement

SIMD kernels must produce outputs that are **within acceptable numeric error bounds** compared to scalar outputs for the same inputs. Perfect bit-identity is not required (IEEE 754 SIMD operations may reorder additions), but error must be bounded and non-accumulating.

### Error Bounds Per Kernel

| Kernel | Output Type | Absolute Error Bound | Relative Error Bound | Rationale |
|---|---|---|---|---|
| `KERN-001` regression score | `double` in `[0.0, 1.0]` | ≤ 1e-9 | N/A | Dot product of 14 terms; rounding at this precision cannot change label decision |
| `KERN-002` binary classification confidence | `double` in `[0.0, 1.0]` | ≤ 1e-9 | N/A | Same structure as regression |
| `KERN-003` transformer label | String enum | Exact match required | N/A | Label is a threshold decision; confidence ≤ 1e-9 absolute |

### Validation Protocol

For each kernel at each workload size:

1. Run scalar kernel on test vector → `scalar_output`
2. Run SIMD kernel on same test vector → `simd_output`
3. Assert `|simd_output - scalar_output| ≤ bound` for numeric outputs
4. Assert string label identity for label outputs
5. Fail the build if any vector exceeds the bound

This validation runs as part of the React-first gate (spec 257 T006 extended) and as a separate `parity-check` CI step.

---

## T006 — Microbenchmark Harness and Reporting Template

### Harness Structure

Each kernel microbenchmark is a standalone Playwright Chromium headless script:

```typescript
// scripts/wasm-kernel-microbench.ts
import { getWasmModule } from '../src/typescript/react/src/lib/wasmRuntime';
import { getKernelVariant } from '../src/typescript/react/src/lib/wasmKernels';

const WARMUP_CALLS = 100;
const MEASURED_CALLS = 1_000;

async function benchKernel(name: string, fn: () => void) {
  for (let i = 0; i < WARMUP_CALLS; i++) fn();

  const start = performance.now();
  for (let i = 0; i < MEASURED_CALLS; i++) fn();
  const elapsed = performance.now() - start;

  return {
    kernel: name,
    variant: getKernelVariant(),
    calls: MEASURED_CALLS,
    total_ms: elapsed,
    median_us: (elapsed / MEASURED_CALLS) * 1000,
    throughput_per_sec: Math.round((MEASURED_CALLS / elapsed) * 1000),
  };
}
```

### Report Format

```json
{
  "timestamp": "2026-05-05T00:00:00Z",
  "environment": { "browser": "Chromium", "simd": true },
  "kernels": [
    {
      "kernel": "KERN-001-regression",
      "variant": "simd",
      "calls": 1000,
      "total_ms": 12.4,
      "median_us": 12.4,
      "throughput_per_sec": 80645
    }
  ]
}
```

Report written to `artifacts/wasm/kernel-bench-<timestamp>.json` and uploaded as CI artifact `wasm-kernel-bench`.

---

## T007 — Scenario Benchmark Suite and Release-Gate Thresholds

### Scenario Suite

| Scenario | Description | Workload Size | Pass Criteria |
|---|---|---|---|
| `SCENE-001` interactive inference | Single call latency as seen by UX | Small (1 call) | ≤ 2 ms warm |
| `SCENE-002` quiz benchmark (CI gate) | 6 sequential calls, matches quiz-model-benchmark-gate.ts flow | Medium (6 calls) | ≤ 10 ms total |
| `SCENE-003` bulk analysis | Throughput under sustained load | Large (1 000 calls) | ≥ 50 000 calls/sec |
| `SCENE-004` SIMD vs scalar delta | Perf ratio comparison | Large | SIMD ≥ 1.5× scalar throughput for Tier 1 kernels to justify complexity |

### Release-Gate Thresholds Per Kernel Family

| Kernel | Gate Condition |
|---|---|
| `KERN-001` regression | SIMD/scalar throughput ratio ≥ 1.5 AND parity bound met AND React-first gate passed |
| `KERN-002` binary | Same as KERN-001 |
| `KERN-003` transformer | Measure only; no SIMD impl promotion unless ratio ≥ 1.3 (smaller gain acceptable given complexity) |

If the ratio falls below the threshold, ship the scalar WASM path only — no SIMD impl promotion.

---

## T008 — React-First Promotion Gate Per Kernel Family

### Deployment Note (Vercel + Fly)

- SIMD and scalar kernels are both browser-side WASM assets and are served as static files; Vercel can host both variants directly.
- Fly.io is not a worker host in this architecture; it remains the API fallback runtime when client-side WASM cannot initialize.
- Promotion evidence must include that both SIMD and scalar artifact pairs are deployed and reachable under `/wasm/` in the web deployment target.

### Gate Structure

Extends spec 257 T006 gate with SIMD-specific checks per kernel:

| Step | Check | Pass Condition |
|---|---|---|
| 1 | SIMD build | `artifacts/wasm/banana-wasm-simd.wasm` exists and `wasm-validate --enable-simd` passes |
| 2 | Capability probe | `detectWasmSimd()` returns `true` in Chromium headless; `false` in scalar-forced mock |
| 3 | Parity | All test vectors meet error bounds (T005) for both variants |
| 4 | Performance | SCENE-001/002/003/004 pass for SIMD variant |
| 5 | Fallback | Scalar variant produces correct outputs when SIMD detection returns `false` |

Desktop/mobile SIMD promotion is blocked until all 5 steps pass for a given kernel family in a React CI run. The passing CI run URL must be linked in the promotion PR.

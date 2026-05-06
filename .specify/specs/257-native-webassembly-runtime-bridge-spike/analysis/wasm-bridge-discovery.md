# WASM Bridge Discovery & Design — Spec 257 Phase 1

**Date**: 2026-05-05
**Status**: Complete (T001–T008)
**Branch**: `feature/wasm-native-integration-spikes`

---

## T001 — Native Calculation Entry Point Inventory

Candidate functions are sourced from `src/native/wrapper/banana_wrapper.h` (public ABI, ABI version 2.4) and classified by WASM suitability.

### Tier 1 — Primary Candidates (low complexity, pure compute, no I/O)

| Symbol | Core Function | Inputs | Output Shape | WASM Risk |
|---|---|---|---|---|
| `banana_calculate_banana` | `banana_core_calculate` | `int purchases, int multiplier` | `int* out_banana` | **Low** — pure arithmetic, stack-only output |
| `banana_calculate_banana_with_breakdown` | `banana_core_calculate` | `int purchases, int multiplier` | `int* out_banana, int* out_base, int* out_bonus` | **Low** — pure arithmetic, 3 int out-pointers |
| `banana_predict_banana_regression_score` | `banana_ml_predict_regression_score` | `const char* input_json` | `double* out_score` | **Low-Medium** — needs ML model weights in WASM heap |

These are the **Phase 1 WASM target functions**. They have no heap-string outputs, no malloc ownership, and no DB dependency.

### Tier 2 — Secondary Candidates (JSON output, WASM-heap allocation required)

| Symbol | Core Function | Output Shape | WASM Risk |
|---|---|---|---|
| `banana_classify_banana_binary` | `banana_ml_classify_binary` | `char** out_json` (callee-alloc) | **Medium** — requires WASM heap alloc + JS read + free |
| `banana_classify_banana_binary_with_threshold` | same + threshold | `char** out_json` | **Medium** |
| `banana_classify_banana_transformer` | `banana_ml_classify_transformer` | `char** out_json` | **Medium** — model weights larger (~4× binary) |
| `banana_classify_not_banana_junk` | `banana_ml_classify_not_banana_junk` | `char** out_json` | **Medium** |
| `banana_predict_banana_ripeness` | `banana_core_predict_ripeness` | `char** out_json` | **Medium** |
| `banana_create_banana_message` | formatting only | `char** out_message` | **Medium** |

Tier 2 expansion follows React-first gate passage (see T006).

### Tier 3 — Excluded from WASM

| Symbol | Reason |
|---|---|
| `banana_db_query_banana_profile` | Postgres-gated via `libpq`; no WASM equivalent |
| All batch domain (`banana_create_batch`, `banana_get_batch_status`, etc.) | Stateful server-side operations; WASM brings no benefit |
| All truck/logistics domain | Same: stateful, server-side |
| `banana_classify_banana_transformer_ex` / `_quant_embedding` | Diagnostics-only, large embedding arrays; post-gate expansion |
| `banana_native_version` | Platform query; implemented as JS constant in WASM module instead |

### Complexity/Risk Classification Rationale

The native C core has **no POSIX I/O**, no `pthread`, no dynamic linking beyond `libpq` (excluded). The DAL compile gate (`BANANA_ENABLE_DAL=OFF`, default) already produces a DAL-free shared library — the same build switch will produce a DAL-free WASM module. ML model weights are loaded from disk at startup via `banana_ml_models.c`; in WASM mode these must be embedded as data segments or fetched and loaded at module init time.

---

## T002 — Runtime Contracts and Status Mappings

### Native Status Codes (stable, from `banana_wrapper.h`)

| Code | Value | Meaning | WASM Handling |
|---|---|---|---|
| `BANANA_OK` | 0 | Success | Map to `{ ok: true }` |
| `BANANA_INVALID_ARGUMENT` | 1 | Bad inputs | Map to `{ ok: false, code: 1, message: "invalid_argument" }` |
| `BANANA_OVERFLOW` | 2 | Arithmetic overflow | Map to `{ ok: false, code: 2, message: "overflow" }` |
| `BANANA_INTERNAL_ERROR` | 3 | Unrecoverable internal | Map to `{ ok: false, code: 3, message: "internal_error" }` |
| `BANANA_DB_ERROR` | 4 | DB op failed | Not reachable in WASM tier (DB excluded) |
| `BANANA_DB_NOT_CONFIGURED` | 5 | No PG connection | Not reachable in WASM tier |
| `BANANA_NOT_FOUND` | 6 | Record missing | Potentially reachable in batch expansion |
| `BANANA_BUFFER_TOO_SMALL` | 7 | Fixed-size buffer | Map to `{ ok: false, code: 7, message: "buffer_too_small" }` |

### Contract Invariants That Must Survive WASM Mode

1. **Return value semantics**: every fallible export returns an `int` equal to `BananaStatusCode`. The JS wrapper must surface this as a typed `WasmResult<T>` — never silently discard non-zero returns.
2. **No errno, no side-channel errors**: consistent with native ABI. WASM callers must not poll `errno`.
3. **Memory ownership for out-pointers**: caller provides stack/heap pointers; WASM linear memory satisfies this via `Module._malloc` / `Module.setValue` patterns.
4. **String output ownership** (Tier 2): callee allocates via `malloc` inside WASM linear memory; JS caller reads the string via `UTF8ToString(ptr)` then calls `Module._banana_free(ptr)`. Leaks are observable via WASM heap growth and will be surfaced in benchmark.
5. **Deterministic behavior**: no randomness, no OS clock reads in Tier 1 functions. WASM execution should produce bit-identical results to native for the same inputs.

---

## T003 — Canonical WASM Toolchain and Build Flow

### Toolchain: Emscripten

**Emscripten** (`emcc`) is the canonical C→WASM compiler. It supports C11, produces `.wasm` + `.js` glue, and compiles the same source tree used for native builds.

**Pinned version**: `emscripten/emsdk:3.1.61` (current stable LTS). Pin the exact Docker image digest in CI.

### Build Flags

```sh
emcc \
  -O3 \
  -s MODULARIZE=1 \
  -s EXPORT_NAME="BananaWasm" \
  -s EXPORTED_FUNCTIONS="['_banana_calculate_banana','_banana_calculate_banana_with_breakdown','_banana_predict_banana_regression_score','_banana_free','_malloc','_free']" \
  -s EXPORTED_RUNTIME_METHODS="['UTF8ToString','stringToUTF8','lengthBytesUTF8','setValue','getValue']" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -s INITIAL_MEMORY=8mb \
  -s MAXIMUM_MEMORY=64mb \
  -s NO_EXIT_RUNTIME=1 \
  -s ASSERTIONS=0 \
  --no-entry \
  -DBANANA_BUILDING_DLL=0 \
  -DBANANA_ENABLE_DAL=0 \
  <sources> \
  -o artifacts/wasm/banana-wasm.js
```

This emits `banana-wasm.js` (ESM-compatible wrapper) and `banana-wasm.wasm`.

### Source Inclusion for WASM Build

Only DAL-free sources (matches `BANANA_CORE_SOURCES` minus `banana_dal.c`):

```
src/native/core/banana_status.c
src/native/core/banana_calc.c
src/native/core/banana_classify.c
src/native/core/banana_ripeness.c
src/native/core/domain/banana_ml_models.c
src/native/core/domain/banana_not_banana.c
src/native/core/domain/ml/shared/banana_ml_shared.c
src/native/core/domain/ml/regression/banana_ml_regression.c
src/native/core/domain/ml/binary/banana_ml_binary.c
src/native/core/domain/ml/transformer/banana_ml_transformer.c
src/native/wrapper/banana_wrapper.c
src/native/wrapper/domain/ml/shared/banana_wrapper_ml_json.c
src/native/wrapper/domain/ml/regression/banana_wrapper_ml_regression.c
src/native/wrapper/domain/ml/binary/banana_wrapper_ml_binary.c
src/native/wrapper/domain/ml/transformer/banana_wrapper_ml_transformer.c
```

### Artifact Naming and Versioning

```
artifacts/wasm/
  banana-wasm-<MAJOR>.<MINOR>.wasm   # content-hashed on publish
  banana-wasm-<MAJOR>.<MINOR>.js     # ESM wrapper
  banana-wasm.manifest.json          # { version, sha256, buildDate, abiMajor, abiMinor }
```

Version tracks `BANANA_WRAPPER_ABI_VERSION_MAJOR.MINOR` (currently `2.4`). The `.manifest.json` is consumed by the React loading contract to validate artifact freshness.

### CI Reproducibility

- WASM build runs in `docker/native-builder.Dockerfile` with Emscripten installed (or a dedicated `docker/wasm-builder.Dockerfile`).
- Build is deterministic: no timestamps embedded, `-O3` with fixed seed.
- Artifacts uploaded to GitHub Actions as `banana-wasm-artifacts` and optionally published to Vercel static serving alongside the React bundle.

---

## T004 — Host-to-WASM ABI Definition

### Function Signatures (JavaScript / TypeScript)

All exports are accessed via the `BananaWasm` module instance returned by the async factory.

#### Tier 1: Calculation (pure int, stack out-pointers)

```typescript
// banana_calculate_banana(purchases, multiplier, out_banana_ptr) → status
// JS usage: allocate int32 in WASM heap, call, read, free
function calculateBanana(purchases: number, multiplier: number): WasmResult<{ total: number }>;

// banana_calculate_banana_with_breakdown(purchases, multiplier, out_banana, out_base, out_bonus) → status
function calculateBananaWithBreakdown(
  purchases: number,
  multiplier: number
): WasmResult<{ total: number; base: number; bonus: number }>;

// banana_predict_banana_regression_score(input_json_ptr, out_score_ptr) → status
function predictRegressionScore(inputJson: string): WasmResult<{ score: number }>;
```

#### Buffer Ownership Protocol

For each call that requires a `char*` JSON input:
1. `const len = Module.lengthBytesUTF8(inputJson) + 1`
2. `const ptr = Module._malloc(len)`
3. `Module.stringToUTF8(inputJson, ptr, len)`
4. Call WASM export with `ptr`
5. `Module._free(ptr)` immediately after the call

For each call with `int*` out-pointers (Tier 1):
1. `const outPtr = Module._malloc(4)` (4 bytes for int32)
2. Call WASM export with `outPtr`
3. `const value = Module.getValue(outPtr, 'i32')`
4. `Module._free(outPtr)`

#### WasmResult Type Contract

```typescript
type WasmResult<T> =
  | { ok: true; data: T }
  | { ok: false; code: number; message: string };

const WASM_STATUS_MESSAGES: Record<number, string> = {
  0: 'ok',
  1: 'invalid_argument',
  2: 'overflow',
  3: 'internal_error',
  4: 'db_error',      // not reachable in WASM tier
  5: 'db_not_configured', // not reachable in WASM tier
  6: 'not_found',
  7: 'buffer_too_small',
};
```

#### ABI Stability Contract

- Do not change exported WASM function names once published. Breaking changes require a new ABI major version bump aligned with `BANANA_WRAPPER_ABI_VERSION_MAJOR`.
- The `banana-wasm.manifest.json` `abiMajor` field is read at load time by the React host; mismatches trigger fallback to server API.

---

## T005 — React, Desktop, and Mobile Loading Contracts

### React Web (Primary)

```typescript
// src/typescript/react/src/lib/wasmRuntime.ts (implementation target for 259)

const WASM_INIT_TIMEOUT_MS = 5_000;

let modulePromise: Promise<BananaWasmModule> | null = null;

export async function getWasmModule(): Promise<BananaWasmModule | null> {
  if (modulePromise) return modulePromise;

  modulePromise = Promise.race([
    loadBananaWasm(), // WebAssembly.instantiateStreaming(fetch('/wasm/banana-wasm.wasm'), ...)
    new Promise<null>((_, reject) =>
      setTimeout(() => reject(new Error('wasm_init_timeout')), WASM_INIT_TIMEOUT_MS)
    ),
  ]).catch((err) => {
    console.warn('[banana-wasm] init failed, falling back to server API:', err.message);
    modulePromise = null; // allow retry on next invocation
    return null;
  });

  return modulePromise;
}
```

**Fallback path**: when `getWasmModule()` returns `null`, callers transparently fall through to the existing `fetch('/api/...')` server path. No UI error is surfaced unless both paths fail.

**Artifact URL**: `/wasm/banana-wasm-2.4.wasm` served as a static asset from the React Vite build output. The `.js` wrapper is imported as a normal ESM module.

**Loading strategy**: lazy — the WASM module is not loaded until the first function call is attempted. The `QuizQaPage` and calculation-heavy routes trigger first load.

### Deployment Hosting Decision (Vercel + Fly)

- Browser workers are client-side assets. They are bundled by Vite and downloaded by the browser, so they are compatible with Vercel static hosting and do not require a separate worker host.
- Fly.io remains the server/API runtime (`fly.toml` app `banana-api`) and is the fallback path when WASM init fails or is unavailable.
- The current deployment contract requires publishing Emscripten outputs into the React static asset path at deploy time.

### Deployment Contract Requirements

1. Build WASM artifacts during CI/deploy via `bash scripts/build-wasm.sh --simd`.
2. Copy emitted files (`banana-wasm.js/.wasm`, `banana-wasm-simd.js/.wasm`, manifest files) into the React static output input path (`src/typescript/react/public/wasm/`) before `vite build`.
3. Ensure static hosting serves `/wasm/*` with file-extension pass-through (no SPA rewrite over `.js`/`.wasm` artifact requests).
4. If `/wasm/*` artifacts are missing in production, runtime must continue to fall back to API (Fly-hosted) without uncaught exceptions.

### Desktop (Electron — Secondary)

- The same `.wasm` and `.js` artifacts are bundled inside the Electron app's `resources/` directory.
- Load via `WebAssembly.instantiate(fs.readFileSync(wasmPath))` in the main or renderer process.
- Same `WASM_INIT_TIMEOUT_MS = 5000` and null-fallback to IPC → server API.
- No distinct artifact; reuses the React web artifact from the same build step.
- Desktop expansion is blocked until the React-first gate (T006) passes.

### Mobile (React Native — Secondary)

- React Native does not support `WebAssembly` natively in JSC. The correct approach is a **native module bridge** or running WASM inside a `WebView` via `react-native-webview`.
- Mobile path deferred to spec 259 worker integration. For this spike: document the constraint, confirm the React-first gate applies, and note that mobile will use server API until a JSC-compatible WASM polyfill or native module is evaluated.
- No mobile WASM artifact produced in Phase 1.

---

## T006 — React-First Compatibility Gate

### Gate Definition

The React-first gate is a CI check that **must pass before desktop or mobile WASM expansion** proceeds. It consists of:

1. **Build gate**: WASM artifact builds successfully with Emscripten (`emcc` exit code 0).
2. **Load gate**: React app loads the `.wasm` file in a headless Playwright browser (`WebAssembly` API available) without init timeout.
3. **Correctness gate**: All Tier 1 WASM exports return `BANANA_OK` with correct outputs for a fixed set of deterministic test vectors (identical to native unit test vectors).
4. **Fallback gate**: When the `.wasm` file is replaced with a corrupt/missing file, the React runtime falls back to the server API path without an uncaught error.

### Acceptance Criteria

| Gate | Pass Condition |
|---|---|
| Build | `artifacts/wasm/banana-wasm.wasm` exists and `wasm-validate` passes |
| Load | First `getWasmModule()` call resolves within `WASM_INIT_TIMEOUT_MS` |
| Correctness | `calculateBanana(3, 5)` → `{ ok: true, data: { total: 15 } }` |
| Correctness | `calculateBanana(-1, 5)` → `{ ok: false, code: 1 }` (invalid argument) |
| Correctness | `calculateBanana(2147483647, 2)` → `{ ok: false, code: 2 }` (overflow) |
| Fallback | Corrupt `.wasm` → fallback invoked, no thrown exception |

### Gate Blocking Rule

Desktop and mobile WASM rollout PRs must reference a passing React-first gate CI run in their PR description. Reviewers reject PRs that do not include this evidence.

---

## T007 — Benchmark Protocol

### Metrics

| Metric | Measurement Method | Acceptance Threshold |
|---|---|---|
| **WASM cold start** | Time from first `getWasmModule()` call to module ready (ms) | ≤ 3000 ms on mid-range device |
| **WASM warm latency** | Median of 1000 `calculateBanana` calls in-browser (μs) | ≤ 100 μs |
| **Server roundtrip latency** | Median of 100 `GET /api/banana/calculate` calls (ms) | Baseline measurement only |
| **Throughput** | `calculateBanana` calls/sec sustained over 5s | ≥ 10 000 calls/sec |
| **Heap growth** | WASM linear memory after 10 000 calls (MB) | ≤ initial allocation (no leak) |

### Test Vectors (Deterministic)

```json
[
  { "purchases": 3, "multiplier": 5, "expected_total": 15 },
  { "purchases": 100, "multiplier": 1, "expected_total": 100 },
  { "purchases": 0, "multiplier": 999, "expected_total": 0 },
  { "purchases": 1000, "multiplier": 1000, "expected_total": 1000000 }
]
```

### Benchmark Execution Environment

- **Web**: Playwright Chromium headless, `performance.now()` timings, Node.js 20+.
- **Baseline server**: `scripts/quiz-model-benchmark-gate.ts` pattern adapted for calculation endpoints.
- **Report format**: JSON artifact `artifacts/wasm/benchmark-report.json` consumed by CI gate step.

### Acceptance Decision

WASM rollout proceeds if:
- Cold start ≤ 3 000 ms
- Warm latency ≤ 100 μs (i.e., at least 10× faster than server roundtrip for calculation)
- No memory leak (heap stable across 10 000 calls)

---

## T008 — Evidence Template for Rollout Recommendation

```markdown
# WASM Rollout Evidence Report

**Date**: YYYY-MM-DD
**ABI Version**: 2.x
**Emscripten Version**: 3.1.xx

## React-First Gate
- [ ] Build gate: PASS / FAIL
- [ ] Load gate: PASS / FAIL (cold start: ___ ms)
- [ ] Correctness gate: PASS / FAIL (test vectors: ___/4)
- [ ] Fallback gate: PASS / FAIL

## Benchmark Results
| Metric | Measured | Threshold | Status |
|---|---|---|---|
| Cold start (ms) | | ≤ 3000 | |
| Warm latency (μs) | | ≤ 100 | |
| Throughput (calls/sec) | | ≥ 10 000 | |
| Heap growth (MB) | | ≤ initial | |

## Failure Modes Observed
_List any panics, out-of-memory conditions, or incorrect outputs here._

## Recommendation
- [ ] Proceed to desktop expansion (spec 259)
- [ ] Proceed to mobile evaluation (spec 259)
- [ ] Block — address findings before expansion

## CI Run Reference
_Link to passing GitHub Actions run that produced this report._
```

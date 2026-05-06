# WASM Worker Integration Discovery & Design — Spec 259 Phase 1

**Date**: 2026-05-05
**Status**: Complete (T001–T008)
**Branch**: `feature/wasm-native-integration-spikes`

---

## T001 — Frontend Compute-Triggering Flow Inventory

Sourced from `src/typescript/react/src` and spec 257/258 context.

### Flows That Should Move to Worker Execution

| Flow | Entry Point | Current Behavior | Worker Priority |
|---|---|---|---|
| **Quiz benchmark scoring** | `QuizQaPage.tsx` — mock attempt button → `POST /ml/ensemble` | Fetch to server API; blocks on network, not UI thread | Low (server-bound); WASM worker becomes relevant when WASM replaces fetch |
| **Binary classification** | Any inline `fetch('/api/banana/classify')` call | Server roundtrip; non-blocking by virtue of async fetch | **High** — once WASM replaces the fetch, synchronous WASM call blocks UI thread |
| **Regression scoring** | Direct calls in dashboard/analytics pages | Server roundtrip | **High** — same as binary classification |
| **Bulk / batch analysis** | Future: analytics pages with multiple prompts scored | Not yet implemented in React | **Critical** — 100+ WASM calls in a loop will freeze UI without worker offload |
| **Ripeness prediction** | Calls `banana_predict_banana_ripeness` on classify pages | Server roundtrip | **High** — same as binary |
| **Transformer inference** | Advanced classify path | Server roundtrip | **High** — largest compute; most important to move off main thread |

### Flows That Do NOT Need Worker Execution

| Flow | Reason |
|---|---|
| `calculateBanana` (arithmetic) | Pure fast math; ≤ 5 µs per call — no UI-thread risk |
| Navigation, form input, UI state | No WASM calls |
| Fetch/API calls to server endpoints | Already async; no WASM involvement |

---

## T002 — Existing Timeout / Retry / Fallback Behaviors and Gaps

### Current Behaviors (from spec 257 T005 analysis)

| Mechanism | Current State | Gap |
|---|---|---|
| WASM init timeout | 5 000 ms via `Promise.race`, returns `null` | No UX indicator while init is in flight |
| WASM init failure | `modulePromise` reset to `null`, transparent fall-through to server API | No telemetry event emitted on failure |
| Per-call timeout | Not defined | **Gap**: a slow WASM call inside a worker can hang indefinitely |
| Retry policy | Not defined | **Gap**: WASM init failure retries on next call, but no backoff or max-retry guard |
| Worker termination | Not defined | **Gap**: `worker.terminate()` on hung tasks is not implemented |
| Cancellation | Not defined | **Gap**: UI-triggered cancellation (user navigates away) is not propagated to worker |

### Integration Gap Priorities

1. **Per-call timeout in worker**: block the result promise with a worker-side timeout, then call `worker.terminate()`.
2. **UX loading state**: emit a progress message when worker accepts the task so the UI can show a spinner.
3. **Fallback telemetry**: emit a structured event when fallback to server API is triggered.
4. **Cancellation propagation**: send a `cancel` message to the worker before navigation/unmount.

---

## T003 — Typed Worker Message Protocol

### Hosting Clarification (Vercel + Fly)

- `bananaWasmWorker.ts` is a frontend Web Worker and executes in the end-user browser thread model, not on a server worker platform.
- Vercel deployment is compatible with this model because the worker bundle and WASM modules are static client assets.
- Fly.io is used for backend API hosting/fallback responses only, not for running browser worker code.

### Message Types

```typescript
// Outbound (main thread → worker)
type WorkerRequest =
  | { type: 'calculate'; id: string; fn: 'binary' | 'regression' | 'ripeness' | 'transformer'; payload: string }
  | { type: 'cancel'; id: string };

// Inbound (worker → main thread)
type WorkerResponse =
  | { type: 'progress'; id: string; stage: 'queued' | 'init' | 'executing' }
  | { type: 'result'; id: string; ok: true; data: unknown }
  | { type: 'error'; id: string; ok: false; code: number; message: string }
  | { type: 'fallback'; id: string; reason: 'wasm_init_failed' | 'wasm_timeout' | 'wasm_error' };
```

### Protocol Invariants

1. Every request carries a unique `id` (UUID v4 or crypto.randomUUID()).
2. Worker always emits `{ type: 'progress', stage: 'queued' }` immediately on receiving a request.
3. Worker emits `{ type: 'progress', stage: 'init' }` while WASM module is initializing (if not yet ready).
4. Worker emits exactly one terminal message per request: `result`, `error`, or `fallback`.
5. On receiving `{ type: 'cancel', id }`, the worker discards the pending result and emits no further messages for that `id`.
6. Worker never throws uncaught exceptions — all errors are reported via `{ type: 'error' }`.

### Worker File Location (implementation target)

`src/typescript/react/src/workers/bananaWasmWorker.ts`

Loaded via Vite worker import:
```typescript
const worker = new Worker(
  new URL('../workers/bananaWasmWorker.ts', import.meta.url),
  { type: 'module' }
);
```

---

## T004 — Timeout, Cancellation, and Retry Policy

### Timeout Policy

| Timeout | Value | Action |
|---|---|---|
| WASM module init | 5 000 ms (from spec 257) | Emit `{ type: 'fallback', reason: 'wasm_init_failed' }` |
| Per-call execution | 10 000 ms | Emit `{ type: 'error', code: 8, message: 'execution_timeout' }`, then `worker.terminate()` and recreate worker |
| Worker recreation backoff | 1st failure: immediate; 2nd: 500 ms; 3rd+: 2 000 ms | Prevents tight restart loop |

Add `BANANA_WASM_EXECUTION_TIMEOUT = 8` to `BananaStatusCode` extension for WASM-only codes (do not modify native ABI).

### Cancellation State Transitions

```
PENDING → QUEUED → INIT (optional) → EXECUTING → RESULT | ERROR | FALLBACK
                                        ↑
                              CANCEL message at any state:
                                → discard result, emit nothing further
                                → if EXECUTING: post cancel to worker, worker
                                   checks cancel flag after current call returns
```

### Retry Policy

- **WASM init failures**: retry on next invocation (already in spec 257 design); max 3 consecutive failures before permanently falling back to server API for the session.
- **Per-call errors** (non-timeout): no automatic retry — propagate `error` to caller; caller decides.
- **Execution timeouts**: terminate and recreate worker; increment failure counter toward permanent fallback.

---

## T005 — Telemetry Events and Payload Schema

### Event Catalog

| Event Name | Trigger | Payload |
|---|---|---|
| `wasm.worker.init.started` | Worker first message received | `{ timestamp, channel }` |
| `wasm.worker.init.ready` | WASM module loaded in worker | `{ timestamp, init_ms, variant: 'simd' \| 'scalar' }` |
| `wasm.worker.init.failed` | WASM init timeout or error | `{ timestamp, reason, channel }` |
| `wasm.worker.call.started` | Worker accepts a `calculate` request | `{ id, fn, channel, timestamp }` |
| `wasm.worker.call.completed` | Worker emits `result` | `{ id, fn, duration_ms, variant, channel }` |
| `wasm.worker.call.error` | Worker emits `error` | `{ id, fn, code, message, channel }` |
| `wasm.worker.fallback.triggered` | Worker emits `fallback` | `{ id, fn, reason, channel }` |
| `wasm.worker.cancelled` | Cancel message processed | `{ id, channel }` |

### Channel Values

- `web` — React browser context
- `desktop` — Electron renderer
- `mobile` — React Native WebView context (when applicable)

### Telemetry Transport

Events are emitted via a thin `telemetry.emit(event)` wrapper in `src/typescript/react/src/lib/telemetry.ts`. In CI/UAT, events are captured to `artifacts/wasm/telemetry-<run>.json` for gate assertions. In production, they extend the existing frontend analytics pathway (no new infrastructure required).

---

## T006 — UX State Model and Copy Guidelines

### UX State Machine

```
IDLE → QUEUED → LOADING → READY
                  ↓
              DEGRADED (fallback active)
                  ↓
              ERROR (both paths failed)
```

### States and UX Copy

| State | Visual | Copy |
|---|---|---|
| `IDLE` | No indicator | — |
| `QUEUED` | Spinner (subtle, 16px) | "Analyzing…" |
| `LOADING` | Spinner + progress bar (if init in flight) | "Loading advanced analysis…" |
| `READY` | Result rendered | — |
| `DEGRADED` | Inline badge: yellow warning icon | "Using standard analysis (advanced unavailable)" |
| `ERROR` | Inline red text | "Analysis failed. Try again or reload." |

### Copy Guidelines

- Never expose WASM-specific language to users. Write "advanced analysis", not "WASM kernel".
- `DEGRADED` mode is not an error — frame it as informational, not alarming.
- `ERROR` is reserved for complete failure (both WASM worker and server API failed).
- All copy strings are defined in `src/typescript/react/src/lib/wasmWorkerCopy.ts` for i18n-readiness.

### Component Contract

The `useWasmWorker(fn, payload)` hook returns:
```typescript
{
  state: 'idle' | 'queued' | 'loading' | 'ready' | 'degraded' | 'error';
  result: unknown | null;
  error: { code: number; message: string } | null;
  cancel: () => void;
}
```

Consumers render the `WasmWorkerStatus` component from `@banana/ui` which maps `state` to the appropriate visual + copy.

---

## T007 — UAT and CI Evidence Matrix

### UAT Test Cases

| Test ID | Scenario | Channel | Expected Outcome |
|---|---|---|---|
| `W-001` | Worker initializes and returns correct result | React web | `state: 'ready'`, correct output |
| `W-002` | Worker returns fallback when WASM init mocked to fail | React web | `state: 'degraded'`, server result returned |
| `W-003` | Cancellation stops pending work | React web | No result, `state: 'idle'` after cancel |
| `W-004` | Per-call timeout triggers worker termination and recreates | React web | Error state, then recovery on next call |
| `W-005` | SIMD available — SIMD variant used | React web (Chromium) | Telemetry event `variant: 'simd'` |
| `W-006` | SIMD unavailable — scalar variant used | React web (scalar mock) | Telemetry event `variant: 'scalar'` |
| `W-007` | Bulk 1 000 calls do not block UI thread | React web | `document.visibilityState` remains `visible`, no jank |

### CI Evidence Requirements

Each PR that promotes a worker-backed feature must include:
- Playwright run with W-001..W-004 passing (minimum)
- Telemetry artifact confirming `wasm.worker.init.ready` event emitted
- W-007 passing (long-poll test) for any bulk-compute feature

---

## T008 — React-First Worker Compatibility Gate

### Deployment Preconditions

1. Worker bundle emitted by Vite is included in the Vercel deploy output.
2. `/wasm/banana-wasm.js`, `/wasm/banana-wasm.wasm`, and SIMD counterparts are published as static assets.
3. SPA rewrites do not override `.js`/`.wasm` file requests.
4. Fallback API base URL points to the deployed API host (Fly or equivalent) so degraded mode remains functional.

### Gate Steps (must all pass before desktop/mobile promotion)

| Step | Check | Pass Condition |
|---|---|---|
| 1 | Worker file loads | `new Worker(bananaWasmWorker.ts)` — no errors in Chromium headless |
| 2 | WASM init in worker | `wasm.worker.init.ready` telemetry event emitted within 5 000 ms |
| 3 | Correct result via worker | `W-001` passes: correct output returned through message protocol |
| 4 | Fallback works | `W-002` passes: `state: 'degraded'` with server fallback result |
| 5 | Cancellation works | `W-003` passes |
| 6 | UI thread unblocked | `W-007` passes: no UI-thread blocking during bulk execution |

### Blocking Rule

Desktop (Electron) and mobile (React Native) worker integration PRs must reference a passing CI run that includes all 6 steps. The CI run must have been triggered against the `main` or the promotion PR's base branch — not a feature branch.

### Desktop Specifics (documentation only)

Electron uses `new Worker()` in the renderer process (Chromium engine). The same `bananaWasmWorker.ts` runs unmodified with a `file://` base URL. No changes needed to the worker file; only the Vite build config needs `worker.format: 'iife'` for Electron compatibility.

### Mobile Specifics (documentation only)

React Native with Hermes/JSC does not support `Worker` natively. Options evaluated in next slice:
1. `react-native-threads` (community package) — wraps JSI worker
2. WASM execution inside a hidden `WebView` with `postMessage` bridge
3. Native module exposing the WASM computation via JSI

Mobile remains server-API-only until one of these approaches is validated through the React-first gate pattern.

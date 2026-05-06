/**
 * bananaWasmWorker.ts — Web Worker that hosts WASM execution off the main thread.
 *
 * Spec 259 T003: Typed worker message protocol.
 * Spec 259 T004: Timeout, cancellation, and retry policy.
 * Spec 259 T005: Telemetry events.
 *
 * Load via Vite worker import (spec 259 T003):
 *   const worker = new Worker(new URL('./workers/bananaWasmWorker.ts', import.meta.url), { type:
 * 'module' });
 *
 * Message protocol:
 *   IN  → WorkerRequest  ({ type: 'calculate' | 'cancel', id, ... })
 *   OUT ← WorkerResponse ({ type: 'progress' | 'result' | 'error' | 'fallback', id, ... })
 */

import {
    calculateBanana,
    calculateBananaWithBreakdown,
    classifyBananaBinary,
    predictRegressionScore,
    predictRipeness,
} from '../lib/wasmKernels';
import {
    getKernelVariant,
    getWasmModule,
    WASM_INIT_TIMEOUT_MS,
} from '../lib/wasmRuntime';
import type {BananaWasmModule, KernelVariant, WasmResult} from '../lib/wasmTypes';
import type {WasmFn} from '../lib/wasmWorkerTypes';

interface WorkerCalculateRequest
{
    type: 'calculate';
    id: string;
    fn: WasmFn;
    payload: unknown; // fn-specific; see dispatch below
}
interface WorkerCancelRequest
{
    type: 'cancel';
    id: string;
}
type WorkerRequest = WorkerCalculateRequest|WorkerCancelRequest;

interface WorkerProgressResponse
{
    type: 'progress';
    id: string;
    stage: 'queued'|'init'|'executing';
}
interface WorkerResultResponse
{
    type: 'result';
    id: string;
    ok: true;
    data: unknown;
}
interface WorkerErrorResponse
{
    type: 'error';
    id: string;
    ok: false;
    code: number;
    message: string;
}
interface WorkerFallbackResponse
{
    type: 'fallback';
    id: string;
    reason: 'wasm_init_failed'|'wasm_timeout'|'wasm_error';
}
type WorkerResponse =
    |WorkerProgressResponse|WorkerResultResponse|WorkerErrorResponse|WorkerFallbackResponse;

// ── Per-call execution timeout (spec 259 T004) ────────────────────────────────
const CALL_TIMEOUT_MS = 10_000;

// ── Cancelled request set ─────────────────────────────────────────────────────
const _cancelled = new Set<string>();

function emit(response: WorkerResponse): void
{
    self.postMessage(response);
}

// ── Telemetry (spec 259 T005) ─────────────────────────────────────────────────
function telemetry(event: string, payload: Record<string, unknown>): void
{
    // In CI/UAT: telemetry captured via postMessage with type 'telemetry'.
    // In production: extend analytics pipeline.
    self.postMessage({type : 'telemetry', event, ...payload, timestamp : Date.now()});
}

// ── WASM module cache inside worker ──────────────────────────────────────────
let _mod: BananaWasmModule|null = null;
let _variant: KernelVariant|null = null;
let _initAttempts = 0;
const MAX_INIT_FAILURES = 3;
let _permanentFallback = false;

async function ensureModule(id: string): Promise<BananaWasmModule|null>
{
    if (_permanentFallback)
        return null;
    if (_mod)
        return _mod;

    emit({type : 'progress', id, stage : 'init'});
    telemetry('wasm.worker.init.started', {channel : 'web'});

    const t0 = Date.now();
    const mod = await getWasmModule();

    if (!mod)
    {
        _initAttempts++;
        if (_initAttempts >= MAX_INIT_FAILURES)
        {
            _permanentFallback = true;
        }
        telemetry('wasm.worker.init.failed', {
            reason : 'wasm_init_failed',
            channel : 'web',
            attempts : _initAttempts,
        });
        return null;
    }

    _mod = mod;
    _variant = getKernelVariant();
    telemetry('wasm.worker.init.ready', {
        init_ms : Date.now() - t0,
        variant : _variant,
        channel : 'web',
    });
    return mod;
}

// ── Kernel dispatch ───────────────────────────────────────────────────────────

function dispatch(
    mod: BananaWasmModule,
    fn: WasmFn,
    payload: unknown,
    ): WasmResult<unknown>
{
    switch (fn)
    {
    case 'calculate_banana':
    {
        const {purchases, multiplier} = payload as
        {
            purchases: number;
            multiplier: number
        };
        return calculateBanana(mod, purchases, multiplier);
    }
    case 'calculate_banana_breakdown':
    {
        const {purchases, multiplier} = payload as
        {
            purchases: number;
            multiplier: number
        };
        return calculateBananaWithBreakdown(mod, purchases, multiplier);
    }
    case 'regression':
    {
        const {inputJson} = payload as {inputJson : string};
        return predictRegressionScore(mod, inputJson);
    }
    case 'binary':
    {
        const {inputJson} = payload as {inputJson : string};
        return classifyBananaBinary(mod, inputJson);
    }
    case 'ripeness':
    {
        const {inputJson} = payload as {inputJson : string};
        return predictRipeness(mod, inputJson);
    }
    default:
        return {ok : false, code : 1, message : `unknown_fn: ${String(fn)}`};
    }
}

// ── Message handler ───────────────────────────────────────────────────────────

self.onmessage = async (evt: MessageEvent<WorkerRequest>) => {
    const msg = evt.data;

    if (msg.type === 'cancel')
    {
        _cancelled.add(msg.id);
        return;
    }

    const {id, fn, payload} = msg;

    // Acknowledge immediately (spec 259 T003 — protocol invariant 2)
    emit({type : 'progress', id, stage : 'queued'});
    telemetry('wasm.worker.call.started', {id, fn, channel : 'web'});

    const t0 = Date.now();

    // Ensure module with per-call execution timeout (spec 259 T004)
    const modOrTimeout = await Promise.race([
        ensureModule(id),
        new Promise<'timeout'>((resolve) =>
                                   setTimeout(() => resolve('timeout'), WASM_INIT_TIMEOUT_MS)),
    ]);

    if (modOrTimeout === 'timeout' || modOrTimeout === null)
    {
        if (_cancelled.has(id))
        {
            _cancelled.delete(id);
            return;
        }
        const reason = modOrTimeout === 'timeout' ? 'wasm_timeout' : 'wasm_init_failed';
        emit({type : 'fallback', id, reason});
        telemetry('wasm.worker.fallback.triggered', {id, fn, reason, channel : 'web'});
        return;
    }

    const mod = modOrTimeout;

    if (_cancelled.has(id))
    {
        _cancelled.delete(id);
        return;
    }

    emit({type : 'progress', id, stage : 'executing'});

    // Per-call execution timeout
    let timedOut = false;
    const callTimeoutId = setTimeout(() => { timedOut = true; }, CALL_TIMEOUT_MS);

    try
    {
        const result = dispatch(mod, fn, payload);

        clearTimeout(callTimeoutId);

        if (timedOut || _cancelled.has(id))
        {
            _cancelled.delete(id);
            return;
        }

        if (result.ok)
        {
            emit({type : 'result', id, ok : true, data : result.data});
            telemetry('wasm.worker.call.completed', {
                id,
                fn,
                duration_ms : Date.now() - t0,
                variant : _variant ?? 'scalar',
                channel : 'web',
            });
        }
        else
        {
            emit({type : 'error', id, ok : false, code : result.code, message : result.message});
            telemetry('wasm.worker.call.error',
                      {id, fn, code : result.code, message : result.message, channel : 'web'});
        }
    }
    catch (err)
    {
        clearTimeout(callTimeoutId);
        if (_cancelled.has(id))
        {
            _cancelled.delete(id);
            return;
        }
        const message = err instanceof Error ? err.message : String(err);
        emit({type : 'error', id, ok : false, code : 3, message});
        telemetry('wasm.worker.call.error', {id, fn, code : 3, message, channel : 'web'});
    }
};

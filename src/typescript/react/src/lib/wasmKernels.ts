/**
 * wasmKernels.ts — High-level typed kernel calls over the WASM module.
 *
 * Spec 257 T004: Host-to-WASM ABI, buffer ownership protocol.
 * Spec 258 T003/T004: Kernel catalog, SIMD/scalar dispatch.
 *
 * All functions:
 *  - Accept JS-native types (number, string)
 *  - Return WasmResult<T> — never throw
 *  - Manage WASM linear memory internally (alloc + free)
 */

import {wasmAllocString} from './wasmRuntime';
import {wasmStatusMessage} from './wasmTypes';
import type {BananaWasmModule, WasmResult} from './wasmTypes';

// ── Calculation domain ────────────────────────────────────────────────────────

export function calculateBanana(
    mod: BananaWasmModule,
    purchases: number,
    multiplier: number,
    ): WasmResult<{total : number}>
{
    const outPtr = mod._malloc(4);
    try
    {
        const status = mod._banana_calculate_banana(purchases, multiplier, outPtr);
        if (status !== 0)
            return {ok : false, code : status, message : wasmStatusMessage(status)};
        const total = mod.getValue(outPtr, 'i32');
        return {ok : true, data : {total}};
    }
    finally
    {
        mod._free(outPtr);
    }
}

export function calculateBananaWithBreakdown(
    mod: BananaWasmModule,
    purchases: number,
    multiplier: number,
    ): WasmResult<{total : number; base : number; bonus : number}>
{
    const outTotal = mod._malloc(4);
    const outBase = mod._malloc(4);
    const outBonus = mod._malloc(4);
    try
    {
        const status = mod._banana_calculate_banana_with_breakdown(
            purchases,
            multiplier,
            outTotal,
            outBase,
            outBonus,
        );
        if (status !== 0)
            return {ok : false, code : status, message : wasmStatusMessage(status)};
        return {
            ok : true,
            data : {
                total : mod.getValue(outTotal, 'i32'),
                base : mod.getValue(outBase, 'i32'),
                bonus : mod.getValue(outBonus, 'i32'),
            },
        };
    }
    finally
    {
        mod._free(outTotal);
        mod._free(outBase);
        mod._free(outBonus);
    }
}

// ── Regression scoring (KERN-001) ─────────────────────────────────────────────

export function predictRegressionScore(
    mod: BananaWasmModule,
    inputJson: string,
    ): WasmResult<{score : number}>
{
    const inputPtr = wasmAllocString(mod, inputJson);
    // out_score is a double (8 bytes, 8-byte aligned)
    const outPtr = mod._malloc(8);
    try
    {
        const status = mod._banana_predict_banana_regression_score(inputPtr, outPtr);
        if (status !== 0)
            return {ok : false, code : status, message : wasmStatusMessage(status)};
        const score = mod.getValue(outPtr, 'double');
        return {ok : true, data : {score}};
    }
    finally
    {
        mod._free(inputPtr);
        mod._free(outPtr);
    }
}

// ── Binary classification (KERN-002) ─────────────────────────────────────────

/** Result shape matching native BananaMlClassificationResult JSON output. */
export interface BinaryClassifyResult {
    label: 'banana'|'not_banana';
    confidence: number;
    banana_score: number;
    not_banana_score: number;
}

export function classifyBananaBinary(
    mod: BananaWasmModule,
    inputJson: string,
    ): WasmResult<BinaryClassifyResult>
{
    const inputPtr = wasmAllocString(mod, inputJson);
    // out_json is a char** — 4-byte pointer in WASM32
    const outPtrPtr = mod._malloc(4);
    mod.setValue(outPtrPtr, 0, 'i32'); // zero-initialize
    try
    {
        const status = mod._banana_classify_banana_binary(inputPtr, outPtrPtr);
        if (status !== 0)
            return {ok : false, code : status, message : wasmStatusMessage(status)};
        const jsonPtr = mod.getValue(outPtrPtr, 'i32');
        const jsonStr = mod.UTF8ToString(jsonPtr);
        mod._banana_free(jsonPtr);
        const parsed = JSON.parse(jsonStr) as BinaryClassifyResult;
        return {ok : true, data : parsed};
    }
    finally
    {
        mod._free(inputPtr);
        mod._free(outPtrPtr);
    }
}

export function classifyBananaBinaryWithThreshold(
    mod: BananaWasmModule,
    inputJson: string,
    threshold: number,
    ): WasmResult<BinaryClassifyResult>
{
    const inputPtr = wasmAllocString(mod, inputJson);
    const outPtrPtr = mod._malloc(4);
    mod.setValue(outPtrPtr, 0, 'i32');
    try
    {
        const status =
            mod._banana_classify_banana_binary_with_threshold(inputPtr, threshold, outPtrPtr);
        if (status !== 0)
            return {ok : false, code : status, message : wasmStatusMessage(status)};
        const jsonPtr = mod.getValue(outPtrPtr, 'i32');
        const jsonStr = mod.UTF8ToString(jsonPtr);
        mod._banana_free(jsonPtr);
        return {ok : true, data : JSON.parse(jsonStr) as BinaryClassifyResult};
    }
    finally
    {
        mod._free(inputPtr);
        mod._free(outPtrPtr);
    }
}

// ── Ripeness prediction ───────────────────────────────────────────────────────

export interface RipenessResult {
    label: 'unripe'|'ripe'|'overripe';
    confidence: number;
    model: string;
}

export function predictRipeness(
    mod: BananaWasmModule,
    inputJson: string,
    ): WasmResult<RipenessResult>
{
    const inputPtr = wasmAllocString(mod, inputJson);
    const outPtrPtr = mod._malloc(4);
    mod.setValue(outPtrPtr, 0, 'i32');
    try
    {
        const status = mod._banana_predict_banana_ripeness(inputPtr, outPtrPtr);
        if (status !== 0)
            return {ok : false, code : status, message : wasmStatusMessage(status)};
        const jsonPtr = mod.getValue(outPtrPtr, 'i32');
        const jsonStr = mod.UTF8ToString(jsonPtr);
        mod._banana_free(jsonPtr);
        return {ok : true, data : JSON.parse(jsonStr) as RipenessResult};
    }
    finally
    {
        mod._free(inputPtr);
        mod._free(outPtrPtr);
    }
}

// ── Re-export status helper so callers don't need to import wasmRuntime directly ─
export {wasmStatusMessage};

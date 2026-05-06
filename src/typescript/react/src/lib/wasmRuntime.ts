/**
 * wasmRuntime.ts — React WASM module loader with timeout and fallback.
 *
 * Spec 257 T005: React web loading contract.
 * Spec 258 T004: SIMD capability detection and variant dispatch.
 *
 * Usage:
 *   const mod = await getWasmModule();
 *   if (mod === null) { // fall back to server API }
 */

import type { BananaWasmModule, KernelVariant, WasmModuleFactory } from './wasmTypes';

/** Init timeout in ms (spec 257 T005). */
export const WASM_INIT_TIMEOUT_MS = 5_000;

/** WASM SIMD probe bytes — validates f64x2 support via WebAssembly.validate(). */
const SIMD_PROBE = new Uint8Array([
  0x00, 0x61, 0x73, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x05, 0x01, 0x60, 0x00, 0x01, 0x7b,
  0x03, 0x02, 0x01, 0x00, 0x0a, 0x06, 0x01, 0x04, 0x00, 0xfd, 0x0f, 0x41, 0x00, 0xfd, 0x0f, 0x0b,
]);

let _simdAvailable: boolean | null = null;

/** Detect WASM SIMD support once and cache. */
export function detectWasmSimd(): boolean {
  if (_simdAvailable !== null) return _simdAvailable;
  try {
    _simdAvailable = WebAssembly.validate(SIMD_PROBE);
  } catch {
    _simdAvailable = false;
  }
  return _simdAvailable;
}

/** Active kernel variant based on SIMD capability. */
export function getKernelVariant(): KernelVariant {
  return detectWasmSimd() ? 'simd' : 'scalar';
}

// Module promise cache — null means "not yet tried or last attempt failed".
let _modulePromise: Promise<BananaWasmModule | null> | null = null;

async function loadModule(): Promise<BananaWasmModule | null> {
  const variant = getKernelVariant();
  const jsFile = variant === 'simd' ? '/wasm/banana-wasm-simd.js' : '/wasm/banana-wasm.js';

  try {
    // Dynamic import of the Emscripten-generated JS wrapper.
    const factory: WasmModuleFactory = (await import(/* @vite-ignore */ jsFile)).default;
    const mod = await Promise.race<BananaWasmModule | null>([
      factory(),
      new Promise<null>((_, reject) =>
        setTimeout(() => reject(new Error('wasm_init_timeout')), WASM_INIT_TIMEOUT_MS),
      ),
    ]);
    return mod;
  } catch (err) {
    const message = err instanceof Error ? err.message : String(err);
    console.warn('[banana-wasm] init failed, falling back to server API:', message);
    return null;
  }
}

/**
 * Get (or lazy-load) the Banana WASM module.
 * Returns null if init fails or times out — callers must fall back to server API.
 * On failure the promise cache is cleared so next call retries.
 */
export async function getWasmModule(): Promise<BananaWasmModule | null> {
  if (_modulePromise) return _modulePromise;

  _modulePromise = loadModule().then((mod) => {
    if (mod === null) {
      // Clear cache so callers can retry on next invocation.
      _modulePromise = null;
    }
    return mod;
  });

  return _modulePromise;
}

/** Allocate a UTF-8 C string in WASM linear memory. Caller must free with mod._free(). */
export function wasmAllocString(mod: BananaWasmModule, str: string): number {
  const len = mod.lengthBytesUTF8(str) + 1;
  const ptr = mod._malloc(len);
  mod.stringToUTF8(str, ptr, len);
  return ptr;
}

/** Read a UTF-8 string from WASM linear memory (callee-allocated; frees via banana_free). */
export function wasmReadAndFreeString(mod: BananaWasmModule, ptrPtr: number): string {
  const ptr = mod.getValue(ptrPtr, 'i32');
  const str = mod.UTF8ToString(ptr);
  mod._banana_free(ptr);
  return str;
}

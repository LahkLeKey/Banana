/**
 * wasmTypes.ts — Shared type contracts for the Banana WASM runtime.
 *
 * Spec 257 T004: WasmResult<T> type and status code map.
 * Spec 258 T004: KernelVariant type.
 */

/** Every fallible WASM call returns one of these shapes. */
export type WasmResult<T> =
  | {
      ok: true;
      data: T;
    }
  | {
      ok: false;
      code: number;
      message: string;
    };

/** Mirrors BananaStatusCode from banana_wrapper.h (stable; do not renumber). */
export const WASM_STATUS_MESSAGES: Record<number, string> = {
  0: "ok",
  1: "invalid_argument",
  2: "overflow",
  3: "internal_error",
  4: "db_error",
  5: "db_not_configured",
  6: "not_found",
  7: "buffer_too_small",
  8: "execution_timeout", // WASM-host-only; not in native ABI
};

export function wasmStatusMessage(code: number): string {
  return WASM_STATUS_MESSAGES[code] ?? `unknown_${code}`;
}

/** Kernel variant selected at runtime based on SIMD capability detection. */
export type KernelVariant = "simd" | "scalar";

/** Opaque handle to the loaded BananaWasm Emscripten module. */
export interface BananaWasmModule {
  // Scalar exports
  _banana_calculate_banana: (purchases: number, multiplier: number, outPtr: number) => number;
  _banana_calculate_banana_with_breakdown: (
    purchases: number,
    multiplier: number,
    outTotalPtr: number,
    outBasePtr: number,
    outBonusPtr: number
  ) => number;
  _banana_predict_banana_regression_score: (inputJsonPtr: number, outScorePtr: number) => number;
  _banana_classify_banana_binary: (inputJsonPtr: number, outJsonPtr: number) => number;
  _banana_classify_banana_binary_with_threshold: (
    inputJsonPtr: number,
    threshold: number,
    outJsonPtr: number
  ) => number;
  _banana_predict_banana_ripeness: (inputJsonPtr: number, outJsonPtr: number) => number;
  _banana_free: (ptr: number) => void;
  _malloc: (size: number) => number;
  _free: (ptr: number) => void;

  // SIMD-build-only extras (present when loaded from banana-wasm-simd.js)
  _banana_ml_regression_predict_simd?: (featuresPtr: number, outScorePtr: number) => number;
  _banana_ml_binary_classify_simd?: (featuresPtr: number, outResultPtr: number) => number;

  // Emscripten runtime helpers
  UTF8ToString: (ptr: number, maxBytes?: number) => string;
  stringToUTF8: (str: string, outPtr: number, maxBytesToWrite: number) => void;
  lengthBytesUTF8: (str: string) => number;
  setValue: (ptr: number, value: number, type: string) => void;
  getValue: (ptr: number, type: string) => number;
}

/** Shape returned from the BananaWasm / BananaWasmSimd factory function. */
export type WasmModuleFactory = (opts?: Record<string, unknown>) => Promise<BananaWasmModule>;

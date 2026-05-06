/**
 * wasmWorkerTypes.ts — Re-exported worker fn type for use outside the worker file.
 *
 * Spec 259 T003.
 */

export type WasmFn =
  | 'calculate_banana'
  | 'calculate_banana_breakdown'
  | 'regression'
  | 'binary'
  | 'ripeness';

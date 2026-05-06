/**
 * wasmWorkerCopy.ts — User-facing copy strings for WASM worker UX states.
 *
 * Spec 259 T006: UX copy guidelines.
 * Never expose WASM-specific language to users.
 */

export const WASM_WORKER_COPY = {
    queued : 'Analyzing\u2026',
    loading : 'Loading advanced analysis\u2026',
    degraded : 'Using standard analysis (advanced unavailable)',
    error : 'Analysis failed. Try again or reload.',
} as const;

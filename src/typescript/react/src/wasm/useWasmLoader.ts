/**
 * useWasmLoader.ts — Fetch and instantiate the WASM engine module.
 *
 * Spec 261 T008: WASM loader module — fetch .wasm, instantiate, expose engine_tick.
 *
 * Exports the WasmEngine interface which the game loop and telemetry hooks consume.
 */

import { useCallback, useEffect, useRef, useState } from "react";

export type WasmLoadState = "idle" | "loading" | "ready" | "error";

export interface WasmEngine {
  engine_tick: (dt: number) => number;
  engine_render_frame: () => void;
  engine_get_frame_buffer: () => Uint8Array;
}

export interface WasmLoaderResult {
  state: WasmLoadState;
  engine: WasmEngine | null;
  error: string | null;
}

const WASM_URL = "/engine.wasm";

export function useWasmLoader(): WasmLoaderResult {
  const [state, setState] = useState<WasmLoadState>("idle");
  const [engine, setEngine] = useState<WasmEngine | null>(null);
  const [error, setError] = useState<string | null>(null);
  const abortRef = useRef<AbortController | null>(null);

  const load = useCallback(async () => {
    setState("loading");
    setError(null);

    abortRef.current = new AbortController();
    const signal = abortRef.current.signal;

    try {
      const response = await fetch(WASM_URL, { signal });
      if (!response.ok) {
        throw new Error(`Failed to fetch WASM: ${response.status} ${response.statusText}`);
      }

      const bytes = await response.arrayBuffer();
      if (signal.aborted) return;

      const result = await WebAssembly.instantiate(bytes, {
        env: {
          memory: new WebAssembly.Memory({ initial: 16 }),
        },
      });

      if (signal.aborted) return;

      const exports = result.instance.exports as Record<string, unknown>;

      const resolved: WasmEngine = {
        engine_tick: exports.engine_tick as (dt: number) => number,
        engine_render_frame: exports.engine_render_frame as () => void,
        engine_get_frame_buffer: exports.engine_get_frame_buffer as () => Uint8Array,
      };

      setEngine(resolved);
      setState("ready");
    } catch (err) {
      if (signal.aborted) return;
      const message = err instanceof Error ? err.message : String(err);
      setError(message);
      setState("error");
    }
  }, []);

  useEffect(() => {
    void load();
    return () => {
      abortRef.current?.abort();
    };
  }, [load]);

  return { state, engine, error };
}

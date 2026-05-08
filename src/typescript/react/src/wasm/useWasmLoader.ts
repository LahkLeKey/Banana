/**
 * useWasmLoader.ts — Fetch and instantiate the WASM engine module.
 *
 * Spec 261 T004/T007/T008: Full viewport lifecycle state machine with retry,
 * timeout detection, and telemetry emission.
 *
 * Lifecycle states: booting → ready → (caller drives running)
 *                   booting/ready → degraded → recovering → ready | degraded
 *                   any → stopped (on unmount)
 */

import { useCallback, useEffect, useRef, useState } from "react";

import { emitTelemetry } from "./viewportTelemetry";

export type ViewportLifecycleState =
  | "booting"
  | "ready"
  | "running"
  | "degraded"
  | "recovering"
  | "stopped";

export interface WasmEngine {
  engine_tick: (dt: number) => number;
  engine_render_frame: () => void;
  engine_get_frame_buffer: () => Uint8Array;
}

export interface WasmLoaderResult {
  lifecycleState: ViewportLifecycleState;
  engine: WasmEngine | null;
  error: string | null;
  retryCount: number;
  retry: () => void;
}

const WASM_URL = "/engine.wasm";
const FETCH_TIMEOUT_MS = 10_000;
const MAX_RETRY_ATTEMPTS = 3;

function withTimeout<T>(promise: Promise<T>, ms: number, label: string): Promise<T> {
  return Promise.race([
    promise,
    new Promise<never>((_, reject) =>
      setTimeout(() => reject(new Error(`${label} timed out after ${ms}ms`)), ms)
    ),
  ]);
}

export function useWasmLoader(): WasmLoaderResult {
  const [lifecycleState, setLifecycleState] = useState<ViewportLifecycleState>("booting");
  const [engine, setEngine] = useState<WasmEngine | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [retryCount, setRetryCount] = useState(0);
  const abortRef = useRef<AbortController | null>(null);
  const sessionIdRef = useRef<string>(crypto.randomUUID());
  const bootStartRef = useRef<number>(Date.now());

  const load = useCallback(async (attempt: number) => {
    const sessionId = sessionIdRef.current;
    const fetchStart = Date.now();

    setLifecycleState(attempt === 0 ? "booting" : "recovering");
    setError(null);

    if (attempt > 0) {
      emitTelemetry({ kind: "recovery_attempt", sessionId, payload: { attempt } });
    }

    abortRef.current = new AbortController();
    const signal = abortRef.current.signal;

    try {
      const response = await withTimeout(
        fetch(WASM_URL, { signal }),
        FETCH_TIMEOUT_MS,
        "WASM fetch"
      );

      if (!response.ok) {
        throw new Error(`Failed to fetch WASM: ${response.status} ${response.statusText}`);
      }

      const bytes = await response.arrayBuffer();
      if (signal.aborted) return;

      const fetchMs = Date.now() - fetchStart;
      const instantiateStart = Date.now();

      const result = await WebAssembly.instantiate(bytes, {
        env: {
          memory: new WebAssembly.Memory({ initial: 16 }),
        },
      });

      if (signal.aborted) return;

      const instantiateMs = Date.now() - instantiateStart;

      const exports = result.instance.exports as Record<string, unknown>;
      const resolved: WasmEngine = {
        engine_tick: exports.engine_tick as (dt: number) => number,
        engine_render_frame: exports.engine_render_frame as () => void,
        engine_get_frame_buffer: exports.engine_get_frame_buffer as () => Uint8Array,
      };

      emitTelemetry({
        kind: "viewport_start",
        sessionId,
        payload: { fetch_ms: fetchMs, instantiate_ms: instantiateMs, attempt },
      });

      if (attempt > 0) {
        emitTelemetry({
          kind: "recovery_success",
          sessionId,
          payload: { attempt, downtime_ms: Date.now() - bootStartRef.current },
        });
      }

      setEngine(resolved);
      setLifecycleState("ready");
    } catch (err) {
      if (signal.aborted) return;

      const message = err instanceof Error ? err.message : String(err);
      setError(message);

      if (attempt > 0) {
        emitTelemetry({
          kind: "recovery_failure",
          sessionId,
          payload: { attempt, reason: message },
        });
      }

      setLifecycleState("degraded");
    }
  }, []);

  const retry = useCallback(() => {
    if (retryCount >= MAX_RETRY_ATTEMPTS) return;
    abortRef.current?.abort();
    const next = retryCount + 1;
    setRetryCount(next);
    void load(next);
  }, [retryCount, load]);

  useEffect(() => {
    void load(0);
    return () => {
      abortRef.current?.abort();
      emitTelemetry({
        kind: "stopped",
        sessionId: sessionIdRef.current,
        payload: { uptime_ms: Date.now() - bootStartRef.current },
      });
    };
  }, [load]);

  return { lifecycleState, engine, error, retryCount, retry };
}

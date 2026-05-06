/**
 * useWasmWorker.ts — React hook for off-thread WASM kernel execution.
 *
 * Spec 259 T003/T004/T006: worker protocol, timeout/cancel, UX state model.
 *
 * Usage:
 *   const { state, result, error, cancel } = useWasmWorker('binary', { inputJson });
 */

import { useCallback, useEffect, useReducer, useRef } from "react";

import type { WasmFn } from "../wasmWorkerTypes";

// ── State machine (spec 259 T006) ─────────────────────────────────────────────
export type WasmWorkerState = "idle" | "queued" | "loading" | "ready" | "degraded" | "error";

interface WorkerHookState {
  state: WasmWorkerState;
  result: unknown | null;
  error: { code: number; message: string } | null;
}

type WorkerAction =
  | { type: "QUEUED" }
  | { type: "INIT" }
  | { type: "EXECUTING" }
  | {
      type: "RESULT";
      data: unknown;
    }
  | {
      type: "ERROR";
      code: number;
      message: string;
    }
  | { type: "FALLBACK" }
  | { type: "RESET" };

function reducer(state: WorkerHookState, action: WorkerAction): WorkerHookState {
  switch (action.type) {
    case "QUEUED":
      return { ...state, state: "queued", result: null, error: null };
    case "INIT":
      return { ...state, state: "loading" };
    case "EXECUTING":
      return { ...state, state: "loading" };
    case "RESULT":
      return { state: "ready", result: action.data, error: null };
    case "ERROR":
      return {
        state: "error",
        result: null,
        error: { code: action.code, message: action.message },
      };
    case "FALLBACK":
      return { ...state, state: "degraded", error: null };
    case "RESET":
      return { state: "idle", result: null, error: null };
    default:
      return state;
  }
}

const INITIAL: WorkerHookState = {
  state: "idle",
  result: null,
  error: null,
};

// ── Worker factory (singleton per hook instance) ──────────────────────────────
function createWorker(): Worker {
  return new Worker(new URL("../../workers/bananaWasmWorker.ts", import.meta.url), {
    type: "module",
  });
}

// ── Hook ─────────────────────────────────────────────────────────────────────

export function useWasmWorker(fn: WasmFn | null, payload: unknown) {
  const [hookState, dispatch] = useReducer(reducer, INITIAL);
  const workerRef = useRef<Worker | null>(null);
  const pendingIdRef = useRef<string | null>(null);

  // Lazy-create the worker
  function getWorker(): Worker {
    if (!workerRef.current) {
      workerRef.current = createWorker();
      workerRef.current.onmessage = (evt: MessageEvent) => {
        const msg = evt.data as {
          type: string;
          id?: string;
          [k: string]: unknown;
        };
        if (msg.type === "telemetry") return; // handled separately
        if (msg.id !== pendingIdRef.current) return; // stale response

        switch (msg.type) {
          case "progress":
            if (msg.stage === "queued") dispatch({ type: "QUEUED" });
            else if (msg.stage === "init") dispatch({ type: "INIT" });
            else dispatch({ type: "EXECUTING" });
            break;
          case "result":
            pendingIdRef.current = null;
            dispatch({ type: "RESULT", data: msg.data });
            break;
          case "error":
            pendingIdRef.current = null;
            dispatch({
              type: "ERROR",
              code: msg.code as number,
              message: msg.message as string,
            });
            break;
          case "fallback":
            pendingIdRef.current = null;
            dispatch({ type: "FALLBACK" });
            break;
        }
      };
    }
    return workerRef.current;
  }

  // Trigger execution when fn/payload change
  useEffect(() => {
    if (!fn || payload === null || payload === undefined) return;

    const id = crypto.randomUUID();
    pendingIdRef.current = id;
    dispatch({ type: "QUEUED" });

    const worker = getWorker();
    worker.postMessage({ type: "calculate", id, fn, payload });

    return () => {
      // Cancel on unmount or if deps change before response
      if (pendingIdRef.current === id) {
        worker.postMessage({ type: "cancel", id });
        pendingIdRef.current = null;
      }
    };
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [fn, JSON.stringify(payload)]);

  // Terminate worker on hook unmount
  useEffect(() => {
    return () => {
      workerRef.current?.terminate();
      workerRef.current = null;
    };
  }, []);

  const cancel = useCallback(() => {
    if (pendingIdRef.current) {
      workerRef.current?.postMessage({ type: "cancel", id: pendingIdRef.current });
      pendingIdRef.current = null;
      dispatch({ type: "RESET" });
    }
  }, []);

  return {
    state: hookState.state,
    result: hookState.result,
    error: hookState.error,
    cancel,
  };
}

/**
 * useGameLoop.ts — requestAnimationFrame game loop driving engine_tick.
 *
 * Spec 261 T009: Wire requestAnimationFrame game loop calling engine_tick.
 *
 * Calls engine_tick(dt) every frame and exposes the last tick return code
 * so callers (e.g. telemetry) can observe frame timing.
 */

import { useEffect, useRef } from "react";

import type { WasmEngine } from "./useWasmLoader";

export interface GameLoopHandle {
  /** Last frame duration in milliseconds. Updated each tick. */
  lastFrameMs: React.MutableRefObject<number>;
  /** Frame counter incremented each tick. */
  frameCount: React.MutableRefObject<number>;
}

export function useGameLoop(engine: WasmEngine | null): GameLoopHandle {
  const rafRef = useRef<number>(0);
  const lastTimeRef = useRef<number>(0);
  const lastFrameMs = useRef<number>(0);
  const frameCount = useRef<number>(0);

  useEffect(() => {
    if (!engine) return;

    let running = true;

    function tick(now: number) {
      if (!running) return;

      const dt = lastTimeRef.current === 0 ? 0 : now - lastTimeRef.current;
      lastTimeRef.current = now;
      lastFrameMs.current = dt;
      frameCount.current += 1;

      engine?.engine_tick(dt);

      rafRef.current = requestAnimationFrame(tick);
    }

    rafRef.current = requestAnimationFrame(tick);

    return () => {
      running = false;
      cancelAnimationFrame(rafRef.current);
    };
  }, [engine]);

  return { lastFrameMs, frameCount };
}

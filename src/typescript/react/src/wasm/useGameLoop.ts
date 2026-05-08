/**
 * useGameLoop.ts — requestAnimationFrame game loop driving engine_tick.
 *
 * Spec 261 T004/T006: Game loop with first-frame telemetry, tick exception budget,
 * and onDegraded callback so viewport can transition lifecycle state.
 *
 * Calls engine_tick(dt) every frame. Three consecutive tick exceptions trigger
 * onDegraded (degraded lifecycle transition per spec 261 FR-003).
 */

import { useEffect, useRef } from "react";
import { recordDegradedEntry, recordFirstFrame, recordFrameTick } from "./performanceTracker";
import type { WasmEngine } from "./useWasmLoader";
import { emitTelemetry } from "./viewportTelemetry";

export interface GameLoopHandle {
  /** Last frame duration in milliseconds. Updated each tick. */
  lastFrameMs: React.MutableRefObject<number>;
  /** Frame counter incremented each tick. */
  frameCount: React.MutableRefObject<number>;
}

export interface GameLoopOptions {
  sessionId: string;
  bootStartMs: number;
  onDegraded?: (reason: string) => void;
}

const MAX_TICK_FAILURES = 3;

export function useGameLoop(engine: WasmEngine | null, options?: GameLoopOptions): GameLoopHandle {
  const rafRef = useRef<number>(0);
  const lastTimeRef = useRef<number>(0);
  const lastFrameMs = useRef<number>(0);
  const frameCount = useRef<number>(0);
  const firstFrameEmittedRef = useRef(false);
  const tickFailuresRef = useRef(0);

  useEffect(() => {
    if (!engine) return;

    let running = true;

    function tick(now: number) {
      if (!running) return;

      const dt = lastTimeRef.current === 0 ? 0 : now - lastTimeRef.current;
      lastTimeRef.current = now;
      lastFrameMs.current = dt;
      frameCount.current += 1;

      try {
        engine?.engine_tick(dt);
        tickFailuresRef.current = 0;
        recordFrameTick();
      } catch (err) {
        tickFailuresRef.current += 1;
        if (tickFailuresRef.current >= MAX_TICK_FAILURES) {
          running = false;
          recordDegradedEntry();
          const reason =
            err instanceof Error
              ? err.message
              : `tick exception after ${frameCount.current} frames`;
          if (options) {
            emitTelemetry({
              kind: "degraded_entry",
              sessionId: options.sessionId,
              payload: { reason, tick_count: frameCount.current },
            });
            options.onDegraded?.(reason);
          }
          return;
        }
      }

      // Emit first-frame telemetry once
      if (!firstFrameEmittedRef.current && options) {
        firstFrameEmittedRef.current = true;
        recordFirstFrame();
        emitTelemetry({
          kind: "first_frame",
          sessionId: options.sessionId,
          payload: { first_frame_ms: now - options.bootStartMs },
        });
      }

      rafRef.current = requestAnimationFrame(tick);
    }

    rafRef.current = requestAnimationFrame(tick);

    return () => {
      running = false;
      cancelAnimationFrame(rafRef.current);
    };
  }, [engine, options]);

  return { lastFrameMs, frameCount };
}

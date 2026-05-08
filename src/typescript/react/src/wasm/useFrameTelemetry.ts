/**
 * useFrameTelemetry.ts — Sparse frame telemetry hook.
 *
 * Spec 261 T010: Emits frame_interval telemetry every 60 frames via
 * the structured viewportTelemetry module.
 */

import { useEffect } from "react";

import type { GameLoopHandle } from "./useGameLoop";
import { emitTelemetry } from "./viewportTelemetry";

const EMIT_INTERVAL_FRAMES = 60;

export function useFrameTelemetry(loop: GameLoopHandle, sessionId: string): void {
  const { lastFrameMs, frameCount } = loop;

  useEffect(() => {
    let lastEmittedFrame = 0;
    let accumulatedMs = 0;
    let sampleCount = 0;

    const interval = setInterval(() => {
      const current = frameCount.current;
      const delta = current - lastEmittedFrame;
      if (delta < EMIT_INTERVAL_FRAMES) return;

      // Compute average over the window
      const avgFrameMs = sampleCount > 0 ? accumulatedMs / sampleCount : lastFrameMs.current;

      lastEmittedFrame = current;
      accumulatedMs = 0;
      sampleCount = 0;

      emitTelemetry({
        kind: "frame_interval",
        sessionId,
        payload: {
          avg_frame_ms: Math.round(avgFrameMs * 100) / 100,
          frame: current,
        },
      });
    }, 500);

    // Accumulate frame_ms samples between emissions
    const sampleInterval = setInterval(() => {
      accumulatedMs += lastFrameMs.current;
      sampleCount += 1;
    }, 16); // ~60fps sampling rate

    return () => {
      clearInterval(interval);
      clearInterval(sampleInterval);
    };
  }, [lastFrameMs, frameCount, sessionId]);
}
